/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#include "TrapController.h"
#include<iostream>

using namespace std;

TrapController::TrapController(double centerFx, double centerFy, double sampleRate,	double gain, bool axis, int wt_freq) {
	srand(time(NULL));

  //The frequency of the wavetable
	long int waveTableFreq = wt_freq;

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

// void TrapController::combineRearrangeWaveform(complex<float> *movingWaveform,
// 	int worker, vector<int> *destinations, const size_t movingWaveformSize) {
// 	int chunkSize = movingWaveformSize / numWorkers;
// 	int startIndex = chunkSize * worker;
// 	int endIndex = chunkSize * (worker + 1);
//
// 	for (int trap_index = 0; trap_index < destinations->size(); trap_index++) {
// 		int dest_index = (*destinations)[trap_index];
//
// 		if (dest_index == -1) {
// 			continue;
// 		}
//
// 		for (int sample_index = startIndex; sample_index < endIndex; sample_index++) {
// 			movingWaveform[sample_index] += loadedTrapWaveforms[trap_index][dest_index].dataVector[sample_index];
// 		}
// 	}
// }

void TrapController::addTrap(double frequency, double amplitude, double phase) {
	traps.push_back(Trap(waveTable, frequency, amplitude, phase));
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

			addTrap(freqx, amplitude, phase);
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

Waveform TrapController::generateWaveform(double duration) {
	size_t num_samples = (size_t)(duration * waveTable->sampleRate);

	std::vector<std::complex<float>> waveform;
	waveform.resize(num_samples);

	for (size_t sample_index = 0; sample_index < num_samples; sample_index++) {
		std::complex<float> sample = 0;
		for (int trap_index = 0; trap_index < traps.size(); trap_index++) {
			sample += traps[trap_index].nextSample();
		}

		waveform[sample_index] = sample;
	}

	return Waveform(waveform);
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

		// double freq = traps[i].frequency / 1.0E6;
		// if (freq < -51 || freq > 51) {
		// 	cout << "Trap #" << i << ": frequency " << freq << " out of bounds [-51, 51]" << endl;
		// 	return false;
		// }

		double amp = traps[i].amplitude;
		// if (amp < 0 || amp > 0.5) {
		// 	cout << "Trap #" << i << ": amplitude" << amp << " out of bounds [0, 0.5]" << endl;
		// 	return false;
		// }

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

	// if (totalPower * gainFactor > 1600) { // Upper limit on power we can deliver to the AOD.
	// 	cout << "Total power " << totalPower * gainFactor << " out of bounds: must be < 1400 mW." << endl;
	// 	return false;
	// }

	// if (totalAmplitude > 0.99) { // Upper limit on total amplitude.
	// 	cout << "Total amplitude " << totalAmplitude << " out of bounds: must be <= 0.99" << endl;
	// 	return false;
	// }

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

vector<double> TrapController::trapFrequencies() {
	vector<double> frequencies;
	for (int i = 0; i < traps.size(); i++) {
		frequencies.push_back(traps[i].frequency);
	}
	return frequencies;
}

void TrapController::resetForRearrangement() {
	const size_t movingWaveformSize = loadedTrapWaveforms[0][0].dataVector.size();
	memset(rearrangeWaveform.dataVector.data(), 0, movingWaveformSize * sizeof(complex<float>));
}


vector<Waveform *> TrapController::rearrangeTraps(std::vector<bool> atomsPresent, enum rearrange_mode mode, int modeArgument) {
/* The main object to determine how we piece together the rearrangement waveforms
is the "destination" vector, which defines the destination for each trap (or -1
if the trap should disappear). The "mode" argument defines how we will create
this destination vector, with "modeArgument" as an optional parameter if we need
to specify how to operate in the mode.

The mode for rearrangement will determine how we build this destination vector,
but then we can pass whatever vector we createto combineRearrangeWaveform and
it will be computed.
*/
	vector<int> destinations;
	vector <Waveform *> blank;
	return blank;
}

std::vector<std::complex<float>> TrapController::getWaveTable(){
	return waveTable->waveTable;
}
