/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#include "TrapController.h"
#include<iostream>

using namespace std;

TrapController::TrapController(double sampleRate,	double g, bool axis, int wt_freq) {
	srand(time(NULL));

  //The frequency of the wavetable
	long int waveTableFreq = wt_freq;

	waveTable = new WaveTable((long int)sampleRate, waveTableFreq);

	majorAxisx = axis;

	gain = g;

}

void TrapController::setCenter(double xaxis, double freq){
    if(xaxis)
        xAxisCenterFreq = freq;

    else
        yAxisCenterFreq = freq;

}


void TrapController::addTrap(double frequency, double amplitude, double phase) {
	traps.push_back(Trap(waveTable, frequency, amplitude, phase));
}



bool TrapController::loadDefaultTrapConfiguration(std::vector<std::vector<string>> tokenList, int groupSize) {

	vector<Trap> previousTraps = traps;

	traps.clear();
	string line;

	double freqx;
	double freqy;
	double amplitude;
	double phase;

	int numTokensParsed = 0;
	for(int i=0;i<groupSize; i++){
		vector<string> tokens  = tokenList[i];

		try {
			freqx = stod(tokens[0]) * 1.0E6;
			amplitude = stod(tokens[1])*gain;

			phase = stod(tokens[2]);

			addTrap(freqx, amplitude, phase);

		} catch (const invalid_argument&) {
			cout << "Invalid trap data";
		}
	}

	if (!sanitizeTraps(gain, false)) {
		cout << "Unable to load trap configuration: not sanitized." << endl;
		traps = previousTraps;
		return false;
	}

	return true;

}

Waveform TrapController::generateWaveform(double duration) {
	size_t num_samples = (size_t)(duration * waveTable->sampleRate);

	std::vector<short> waveform;
	waveform.resize(num_samples);

	for (size_t sample_index = 0; sample_index < num_samples; sample_index++) {
		float sample = 0;
		for (int trap_index = 0; trap_index < traps.size(); trap_index++)
			sample += real(traps[trap_index].nextSample());

		waveform[sample_index] = short(sample);
	}

	staticStartingWaveform = Waveform(waveform);
	return staticStartingWaveform;
}

vector<vector<short>> TrapController::generateModes() {
	size_t num_samples;

	vector<short> tempWaveform;
	vector<vector<short>> waveforms;

	for (int trap_index = 0; trap_index < traps.size(); trap_index++) {
	 	tempWaveform.clear();
		num_samples = (size_t)(10000*waveTable->tableLength*waveTable->tableFrequency/traps[trap_index].frequency);
		for (size_t sample_index = 0; sample_index < num_samples; sample_index++)
					tempWaveform.push_back((short)(real(traps[trap_index].nextSample())));
		waveforms.push_back(tempWaveform);
	}

	return waveforms;
}

//Checks to see if the traps are acceptable
bool TrapController::sanitizeTraps(double new_gain,
	bool shouldPrintTotalPower) {
	double totalPower = 0.0;
	double totalAmplitude = 0.0;

	for (int i = 0; i < traps.size(); i++) {
		double phase = traps[i].getPhase();
		if (phase < 0.0 || phase >= 10.0) {
			cout << "Trap #" << i << ": phase " << phase << " out of bounds [0, 1)" << endl;
			return false;
		}

		double freq = traps[i].frequency / 1.0E6;
		if (freq < 75 || freq > 105) {
			cout << "Trap #" << i << ": frequency " << freq << " out of bounds [75, 105]" << endl;
			return false;
		}

		double amp = traps[i].amplitude;
		if (amp < 0 || amp > 32767) {
			cout << "Trap #" << i << ": amplitude" << amp << " out of bounds [0, 32767]" << endl;
			return false;
		}

		double powerInMode = amp * amp; // Conversion from amplitude -> RF power
		totalPower += powerInMode;
		totalAmplitude += amp;
	}

	if (new_gain == -1) {
		new_gain = gain;
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

	cout << traps.size() << "\n";
	for (int i = 0; i < traps.size(); i++) {
		cout << i << " " << traps[i].frequency << " " << traps[i].amplitude  << " " << traps[i].phase << endl;
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


bool TrapController::mostRecentlyLoadedCorrectWaveforms(double duration, string starting_configuration, string ending_configuration) {
	if (lastLoadedWaveformProperties.starting_configuration.compare(starting_configuration) != 0) {
		return false;
	}

	if (lastLoadedWaveformProperties.ending_configuration.compare(ending_configuration) != 0) {
		return false;
	}

	if (lastLoadedWaveformProperties.duration != duration) {
		return false;
	}

	return true;
}

void TrapController::combineRearrangeWaveform(short *movingWaveform,
	int worker, vector<int> *destinations, const size_t movingWaveformSize, std::vector<short> *mode, int move_block, short* pvBuffer, bool row, int mode_len) {
	int chunkSize = movingWaveformSize / numWorkers;
	int startIndex = (chunkSize * worker)+(move_block);
	int endIndex = (chunkSize * (worker + 1)) + (move_block) ;

	int trap_index; int dest_index; int sample_index; vector<short>* dataArr;
	// auto start = chrono::high_resolution_clock::now(); //start

	for (trap_index = 0; trap_index < destinations->size(); trap_index++) {
		dest_index = (*destinations)[trap_index];
		dataArr = &loadedTrapWaveforms[trap_index][dest_index].dataVectorShort;

		if (dest_index == -1) {
			continue;
		}
		if(row){
			for (sample_index = startIndex; sample_index < endIndex; sample_index++)
				pvBuffer[sample_index*2] += (*dataArr)[sample_index%movingWaveformSize];
				cout << "HERE " << (*dataArr)[sample_index%movingWaveformSize] << endl;
		}
		else{
			for (sample_index = startIndex; sample_index < endIndex; sample_index++)
				pvBuffer[sample_index*2+1] += (*dataArr)[sample_index%movingWaveformSize];
		}
	}
	if(row){
		for(sample_index = startIndex; sample_index < endIndex; sample_index++){
			pvBuffer[sample_index*2+1] += (*mode)[sample_index%mode_len];
		}
	}else{
		for(sample_index = startIndex; sample_index < endIndex; sample_index++){
			pvBuffer[sample_index*2] += (*mode)[sample_index%mode_len];
		}
	}

	// cout << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << "ms to combine move" << endl;

}

/* Moving traps: This will be the sum of the "loaded trap" waveforms for each
moving trap, designated by a start position and end position.
*/
void TrapController::combinePrecomputedWaveform(vector<int> &destinations, std::vector<short> &mode, int move_ind, short* pvBuffer, bool row, int mode_ln,const size_t movingWaveformSize) {

	thread *workers[numWorkers];
	int mode_len = mode.size();
	// Moving traps:
	short *movingWaveform = rearrangeWaveform.dataVectorShort.data();

	int block_ind = move_ind*movingWaveformSize;
	// Add each moving waveform separately.
	for (int worker = 0; worker < numWorkers; worker++) {
		workers[worker] = new thread(&TrapController::combineRearrangeWaveform, this, movingWaveform, worker, &destinations, movingWaveformSize, &mode, block_ind, pvBuffer, row, mode_len);
	}

	// Wait for all workers to finish combining waveforms.
	for (int worker = 0; worker < numWorkers; worker++) {
		workers[worker]->join();
	}
}


int numTrapsForConfigurationName(string config_name) {
	int index_of_parens = config_name.find_first_of('(');
	if (index_of_parens < 0) {
		return -1;
	}

	string num_traps_substring = config_name.substr(0, index_of_parens);

	return stoi(num_traps_substring);
}

bool TrapController::loadPrecomputedWaveforms(double moveDuration, string starting_configuration, string ending_configuration) {
	// Load static waveforms for before and after rearrangement.

	if (!staticStartingWaveform.initializeFromStaticWaveform(starting_configuration)) {
		cout << "Unable to find static waveform for: " << starting_configuration << endl;
		cout << "Aborting!!!" << endl;

		return false;
	}

	if (!staticEndingWaveform.initializeFromStaticWaveform(ending_configuration)) {
		cout << "Unable to find static waveform for: " << ending_configuration << endl;
		cout << "Aborting!!!" << endl;

		return false;
	}

	// Use configuration names (up to first open parens) to determine the number of traps before and after rearrangement.
	numStartingTraps = numTrapsForConfigurationName(starting_configuration);
	numEndingTraps = numTrapsForConfigurationName(ending_configuration);

	if (numStartingTraps <= 0 || numEndingTraps <= 0) {
		cout << "Error: starting with " << numStartingTraps << " and ending with " << numEndingTraps << "!" << endl;
		cout << "Aborting!!!" << endl;

		return false;
	}


	cout << "Loading precomputed waveforms from " << numStartingTraps << " traps to " << numEndingTraps << " traps." << endl;
	cout << "\t" << fixed << setprecision(1) << moveDuration << " ms move time" << endl << endl;

	chrono::high_resolution_clock::time_point start_timer = chrono::high_resolution_clock::now();

	// Rearranging waveforms:

	for (int start_index = 0; start_index < numStartingTraps; start_index++) {
		for (int dest_index = 0; dest_index < numEndingTraps; dest_index++) {
			loadedTrapWaveforms[start_index][dest_index].initializeFromMovingWaveform(moveDuration,
																					 starting_configuration, ending_configuration,
																					 start_index, dest_index);
			// loadedTrapWaveformsShort[start_index][dest_index].initializeShortFromFloatWaveform(loadedTrapWaveforms[start_index][dest_index].dataVector);
		}

	}

	rearrangeWaveform.dataVector.resize(loadedTrapWaveforms[0][0].dataVector.size());

	chrono::high_resolution_clock::time_point end_timer = chrono::high_resolution_clock::now();
	double msElapsed = (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_timer)).count();

	lastLoadedWaveformProperties.duration = moveDuration;
	lastLoadedWaveformProperties.starting_configuration = starting_configuration;
	lastLoadedWaveformProperties.ending_configuration = ending_configuration;


	cout << "\rLoading precomputed waveforms... done! (Time elapsed: " << (msElapsed / 1000.0) << " s)                      " << endl;

	if (loadedTrapWaveforms[0][0].dataVectorShort.size() == 0) {
		cout << "Unable to read waveforms in from disk!" << endl;
		cout << "Aborting!!!" << endl;
		return false;
	}
	cout << "Waveform size: " << loadedTrapWaveforms[0][0].dataVector.size() << " samples." << endl;
	return true;
}

std::vector<std::complex<float>> TrapController::getWaveTable(){
	return waveTable->waveTable;
}

int TrapController::getWFSize(){
	return  loadedTrapWaveforms[0][0].dataVectorShort.size()-1;
}
