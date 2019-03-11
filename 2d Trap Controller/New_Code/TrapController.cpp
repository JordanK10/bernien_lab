/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#include "TrapController.h"
#include <iomanip>
#include <thread>
#include <stdlib.h>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <iomanip>
#include <dirent.h>
#include <fstream>

using namespace std;

TrapController::TrapController(double centerFx, double centerFy, double sampleRate,	double gain, bool axis) {
	srand(time(NULL));

  //The frequency of the wavetable
	long int waveTableFreq = 1E3;
	waveTable = new WaveTable((long int)sampleRate, waveTableFreq);

	majorAxisx = axis;

	awg_gain = gain;
  yAxisCenterFreq = centerFy;
  xAxisCenterFreq = centerFx;

}

void TrapController::setCenter(double xaxis, double freq){
    if(xaxis)
        xAxisCenterFreq = freq;

    else
        yAxisCenterFreq = freq;

}



void TrapController::addTrap(double frequency, double amplitude, double phase) {
	traps.push_back(Trap(waveTable, frequency, amplitude, phase));
	printTraps();
}



bool TrapController::loadDefaultTrapConfiguration(std::vector<std::vector<string>> tokenList, int groupSize) {

	vector<Trap> previousTraps = traps;

	traps.clear();
	string line;

	int numTokensParsed = 0;
	for(int i=0;i<groupSize; i++){
		vector<string> tokens  = tokenList[i];
		try {
			double freqx = stod(tokens[0]) * 1.0E6;
			double freqy = stod(tokens[1]) * 1.0E6;
			double amplitude = stod(tokens[2]);
			double phase = stod(tokens[3]);

			addTrap(freqx - xAxisCenterFreq, amplitude, phase);
		} catch (const invalid_argument&) {
		}
	}

	if (!sanitizeTraps(awg_gain, false)) {
		cout << "Unable to load trap configuration: not sanitized." << endl;
		traps = previousTraps;
		return false;
	}

	return true;

}

//Checks to see if the traps are acceptable
bool TrapController::sanitizeTraps(double new_gain,
	bool shouldPrintTotalPower) {
	double totalPower = 0.0;
	double totalAmplitude = 0.0;

	for (int i = 0; i < traps.size(); i++) {
		double phase = traps[i].getPhase();
		if (phase < 0.0 || phase >= 1.0) {
			cout << "Trap #" << i << ": phase " << phase << " out of bounds [0, 1)" << endl;
			return false;
		}

		double freq = traps[i].frequency / 1.0E6;
		if (freq < -51 || freq > 51) {
			cout << "Trap #" << i << ": frequency " << freq << " out of bounds [-51, 51]" << endl;
			return false;
		}

		double amp = traps[i].amplitude;
		if (amp < 0 || amp > 0.5) {
			cout << "Trap #" << i << ": amplitude" << amp << " out of bounds [0, 0.5]" << endl;
			return false;
		}

		double powerInMode = amp * amp * 5300; // Conversion from amplitude -> RF power
		totalPower += powerInMode;
		totalAmplitude += amp;
	}

	if (new_gain == -1) {
		new_gain = awg_gain;
	}
	double gainFactor = pow(10.0, (new_gain - 1.0)/ 10.0);

	if (shouldPrintTotalPower) {
		cout << "Total power: " << totalPower * gainFactor << " mW" << endl;
	}

	if (totalPower * gainFactor > 1600) { // Upper limit on power we can deliver to the AOD.
		cout << "Total power " << totalPower * gainFactor << " out of bounds: must be < 1400 mW." << endl;
		return false;
	}

	if (totalAmplitude > 0.99) { // Upper limit on total amplitude.
		cout << "Total amplitude " << totalAmplitude << " out of bounds: must be <= 0.99" << endl;
		return false;
	}

	return true;
}

void TrapController::printTraps() {
	// cout << "SDR center frequency: " << centerFrequency << " MHz" << endl;
	// cout << "SDR gain: " << sdr_gain << endl;
	cout << traps.size() << "\n";
	for (int i = 0; i < traps.size(); i++) {
		cout << i << " " << traps[i].frequency << " " << traps[i].amplitude << endl;
	}
}
