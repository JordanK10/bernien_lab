// Created by H.Levine with Atom Array team, Lukin group (2016).


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

TrapController::TrapController(double centerFreq, double sampleRate, double gain) {
	srand(time(NULL));

	long int waveTableFreq = 1E3;
	waveTable = new WaveTable((long int)sampleRate, waveTableFreq);
	centerFrequency = centerFreq;
	sdr_gain = gain;
}


void TrapController::addTrap(double frequency, double amplitude, double phase) {
	traps.push_back(Trap(waveTable, frequency, amplitude, phase));
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

vector<double> TrapController::trapFrequencies() {
	vector<double> frequencies;
	for (int i = 0; i < traps.size(); i++) {
		frequencies.push_back(traps[i].frequency);
	}
	return frequencies;
}

void TrapController::printAvailableDefaultTrapConfigurations() {
	DIR *dir;
	struct dirent *epdf;


	dir = opendir("./DefaultTrapConfigurations");

	vector<string> filenames;

	if (dir != NULL) {
		cout << "Available default trap configurations:" << endl;
		while (epdf = readdir(dir)) {
			if (epdf->d_name[0] != '.') {
				filenames.push_back(epdf->d_name);
			}
		}

		sort(filenames.begin(), filenames.end());
		for (int i = 0; i < filenames.size(); i++) {
			cout << " " << filenames[i] << endl;
		}
	} else {
		cout << "Unable to open file." << endl;
	}
}


bool TrapController::loadDefaultTrapConfiguration(string filename) {
	ifstream config_file("./DefaultTrapConfigurations/" + filename);

	if (!config_file.is_open()) {
		cout << "Unable to open file: " << filename << endl;
		printAvailableDefaultTrapConfigurations();
		return false;
	}

	vector<Trap> previousTraps = traps;

	traps.clear();

	string tokens[3];
	int numTokensParsed = 0;
	while (config_file >> tokens[numTokensParsed]) {
		numTokensParsed++;

		if (numTokensParsed == 3) {
			numTokensParsed = 0;
			try {
				double freq = stod(tokens[0]) * 1.0E6;
				double amplitude = stod(tokens[1]);
				double phase = stod(tokens[2]);

				addTrap(freq - centerFrequency, amplitude, phase);
			} catch (const invalid_argument&) {
			}
		}
	}

	config_file.close();

	if (!sanitizeTraps(sdr_gain, false)) {
		cout << "Unable to load trap configuration: not sanitized." << endl;
		traps = previousTraps;
		return false;
	}

	lastLoadedConfiguration = filename;

	return true;
}

void TrapController::prepareLineDisplay(string text) {
	lineDisplayCounter = 0;
	lineDisplay.setText(text);
}


void TrapController::printLineDisplay() {
	lineDisplay.printDisplayToConsole();
}

vector<Waveform *> TrapController::rearrangeTraps(std::vector<bool> atomsPresent, enum rearrange_mode mode, int modeArgument) {
	// The main object to determine how we piece together the rearrangement waveforms is the "destination" vector, which defines
	// the destination for each trap (or -1 if the trap should disappear). The "mode" argument defines how we will create this
	// destination vector, with "modeArgument" as an optional parameter if we need to specify how to operate in the mode.

	// The mode for rearrangement will determine how we build this destination vector, but then we can pass whatever vector we create
	// to combineRearrangeWaveform and it will be computed.

	vector<int> destinations;

	// Start by counting how many atoms are present.
	int numLoaded = 0;
	for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
		if (atomsPresent[trap_index]) {
			numLoaded++;
		}
	}

	if (mode == REARRANGE_MODE_STACK_FROM_LEFT) {
		// Stack all atoms to the left.

		int numPresentToLeft = 0;
		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			if (atomsPresent[trap_index]) {
				destinations.push_back(numPresentToLeft++);
			} else {
				destinations.push_back(-1);
			}
		}

	} else if (mode == REARRANGE_MODE_STACK_FROM_RIGHT) {
		// Stack all atoms to the right.

		int numPresentToRight = numLoaded;
		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			if (atomsPresent[trap_index]) {
				destinations.push_back(atomsPresent.size() - numPresentToRight--);
			} else {
				destinations.push_back(-1);
			}
		}

	} else if (mode == REARRANGE_MODE_FIXED_ARRAY_WITHOUT_RESERVOIR) {
		int targetArraySize = modeArgument;

		// Stack leftmost "targetArraySize" to the left, and drop the rest of the atoms.
		int numPresentToLeft = 0;
		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			// If we have not yet come across sufficiently many atoms, then send this one to the left.
			// Otherwise, just pretend we didn't see it in the first place.
			if (atomsPresent[trap_index] && numPresentToLeft < targetArraySize) {
				destinations.push_back(numPresentToLeft++);
			} else {
				destinations.push_back(-1);
			}
		}

	} else if (mode == REARRANGE_MODE_FIXED_ARRAY_WITH_RESERVOIR) {
		int targetArraySize = modeArgument;

		// Stack leftmost "targetArraySize" to the left, and move the rest of the atoms to the right.

		int numPresentToLeft = 0;
		int numPresentToRight = numLoaded;

		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			// If we have not yet come across sufficiently many atoms, then send this one to the left.
			// Otherwise, send it to the right.
			if (atomsPresent[trap_index]) {
				if (numPresentToLeft < targetArraySize) {
					destinations.push_back(numPresentToLeft);
				} else {
					destinations.push_back(atomsPresent.size() - numPresentToRight);
				}

				numPresentToLeft++;
				numPresentToRight--;
			} else {
				destinations.push_back(-1);
			}
		}
	} else if (mode == REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR) {
		int targetArraySize = modeArgument;

		int numPresentToLeft = 0;
		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			if (atomsPresent[trap_index]) {
				if (numPresentToLeft < targetArraySize) {
					destinations.push_back(numPresentToLeft);
				} else if (numPresentToLeft + reservoirSeparation < atomsPresent.size()) {
					destinations.push_back(numPresentToLeft + reservoirSeparation);
				} else {
					destinations.push_back(-1);
				}

				numPresentToLeft++;
			} else {
				destinations.push_back(-1);
			}
		}
	} else if (mode == REARRANGE_MODE_LINE_DISPLAY) {
		vector<bool> lineToDisplay = lineDisplay.getLine(lineDisplayCounter);

		// Calculate how many atoms we need for this line.
		vector<int> targetIndices;
		int numAtomsNeeded = 0;
		for (int i = 0; i < lineToDisplay.size(); i++) {
			if (lineToDisplay[i]) {
				numAtomsNeeded++;

				targetIndices.push_back(i);
			}
		}

		// Go through the array and count the first N atoms that are present and send them to the
		// targeted spots. For the remaining atoms we can pretend we didn't even see them.
		int numCounted = 0;
		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			if (atomsPresent[trap_index] && numCounted < numAtomsNeeded) {
				destinations.push_back(targetIndices[numCounted]);
				numCounted++;
			} else {
				destinations.push_back(-1);
			}
		}

		lineDisplayCounter = (lineDisplayCounter + 1) % 6;
	} else if (mode == REARRANGE_MODE_CLUSTERS) {
		if (periodicClusterPattern.size() == 0) {
			for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
				destinations.push_back(-1);
			}
		} else {
			// Calculate how many atoms we need for this line.
	
			int numAtomsNeeded = atomsPerCluster * numClustersToBuild;

			// Go through the array and count the first N atoms that are present and send them to the
			// targeted spots. For the remaining atoms we can pretend we didn't even see them.
			int numCounted = 0;
			for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
				if (atomsPresent[trap_index] && numCounted < numAtomsNeeded) {
					destinations.push_back(clusterTargetIndices[numCounted]);
					numCounted++;
				} else {
					destinations.push_back(-1);
				}
			}
		}

	} else if (mode == REARRANGE_MODE_OPTIMIZED_CLUSTERS) { // Same operation as other cluster mode, but smarter routing.
		if (periodicClusterPattern.size() == 0) {
			for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
				destinations.push_back(-1);
			}
		} else {
			int numLoadedAtoms = 0;
			for (int i = 0; i < atomsPresent.size(); i++) {
				if (atomsPresent[i]) {
					numLoadedAtoms++;
				}
			}

			int totalExcessAtoms = numLoadedAtoms - numClustersToBuild * atomsPerCluster;
			int excessAtomsPerCluster = totalExcessAtoms / numClustersToBuild;

			int numAtomsNeeded = atomsPerCluster * numClustersToBuild;

			// Go through the array and count the first N atoms that are present and send them to the
			// targeted spots. For the remaining atoms we can pretend we didn't even see them.
			int numCounted = 0;
			int numCountedInCluster = 0;
			bool skippingExcessAtoms = false;
			int skippedExcessAtoms = 0;

			for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
				if (atomsPresent[trap_index] && numCounted < numAtomsNeeded) {
					if (skippingExcessAtoms) {
						destinations.push_back(-1);
						skippedExcessAtoms++;
						if (skippedExcessAtoms == excessAtomsPerCluster) {
							skippingExcessAtoms = false;
							numCountedInCluster = 0;
						}
					} else {
						destinations.push_back(clusterTargetIndices[numCounted]);
						numCounted++;
						numCountedInCluster++;

						if (numCountedInCluster == atomsPerCluster) {
							skippingExcessAtoms = true;
							skippedExcessAtoms = 0;
						}
					}

				} else {
					destinations.push_back(-1);
				}

				if (trap_index + 1 > clusterTargetIndices[numCounted]) {
					// prematurely break skip mode
					skippingExcessAtoms = false;
					numCountedInCluster = 0;
				}
			}
			for (int i = 0; i < destinations.size(); i++) {
				if (destinations[i] != -1) {
					cout << "destination " << i << ": " << destinations[i] << endl;
				}
			}
		}
	} else if (mode == REARRANGE_MODE_SLOW_VIDEO) {
		vector<int> targetIndices;
		int numAtomsNeeded = 40;

		targetIndices.push_back(2);
		targetIndices.push_back(4);
		targetIndices.push_back(7);
		targetIndices.push_back(8);
		targetIndices.push_back(9);
		targetIndices.push_back(11);
		targetIndices.push_back(12);
		targetIndices.push_back(13);
		targetIndices.push_back(14);
		targetIndices.push_back(16);
		targetIndices.push_back(24);
		targetIndices.push_back(27);
		targetIndices.push_back(29);
		targetIndices.push_back(32);
		targetIndices.push_back(34);
		targetIndices.push_back(35);
		targetIndices.push_back(36);
		targetIndices.push_back(40);
		targetIndices.push_back(42);
		targetIndices.push_back(46);
		targetIndices.push_back(48);
		targetIndices.push_back(50);
		targetIndices.push_back(55);
		targetIndices.push_back(57);
		targetIndices.push_back(58);
		targetIndices.push_back(59);
		targetIndices.push_back(61);
		targetIndices.push_back(64);
		targetIndices.push_back(68);
		targetIndices.push_back(73);
		targetIndices.push_back(79);
		targetIndices.push_back(80);
		targetIndices.push_back(83);
		targetIndices.push_back(85);
		targetIndices.push_back(87);
		targetIndices.push_back(90);
		targetIndices.push_back(92);
		targetIndices.push_back(96);
		targetIndices.push_back(97);
		targetIndices.push_back(99);


		// Go through the array and count the first N atoms that are present and send them to the
		// targeted spots. For the remaining atoms we can pretend we didn't even see them.
		int numCounted = 0;
		for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
			if (atomsPresent[trap_index] && numCounted < numAtomsNeeded) {
				destinations.push_back(targetIndices[numCounted]);
				numCounted++;
			} else {
				destinations.push_back(-1);
			}
		}
	}

	return combinePrecomputedWaveforms(destinations);
	//return vector<Waveform *>();
}

void TrapController::combineRearrangeWaveform(complex<float> *movingWaveform, int worker, vector<int> *destinations, const size_t movingWaveformSize) {
	int chunkSize = movingWaveformSize / numWorkers;
	int startIndex = chunkSize * worker;
	int endIndex = chunkSize * (worker + 1);

	for (int trap_index = 0; trap_index < destinations->size(); trap_index++) {
		int dest_index = (*destinations)[trap_index];

		if (dest_index == -1) {
			continue;
		}

		for (int sample_index = startIndex; sample_index < endIndex; sample_index++) {
			movingWaveform[sample_index] += loadedTrapWaveforms[trap_index][dest_index].dataVector[sample_index];
		}
	}
}


void TrapController::resetForRearrangement() {
	const size_t movingWaveformSize = loadedTrapWaveforms[0][0].dataVector.size();
	memset(rearrangeWaveform.dataVector.data(), 0, movingWaveformSize * sizeof(complex<float>));
}


void TrapController::calculateClusterProperties(vector<int> pattern, int separation) {
	clusterSeparation = separation;
	periodicClusterPattern = pattern;

	clusterSize = 0;
	atomsPerCluster = 0;

	bool addingAtoms = true;
	for (int i = 0; i < periodicClusterPattern.size(); i++) {
		clusterSize += periodicClusterPattern[i];

		if (addingAtoms) {
			atomsPerCluster += periodicClusterPattern[i];
		}
		addingAtoms = !addingAtoms;
	}

	clusterPeriodicity = clusterSeparation + clusterSize;

	numClustersToBuild = 0;
	while (numClustersToBuild * clusterPeriodicity < traps.size()) {
		if (numClustersToBuild * clusterPeriodicity + clusterSize <= traps.size()) {
			numClustersToBuild++;
		} else {
			break;
		}
	}

	clusterTargetIndices.clear();
	int targetPos = 0;
	while (targetPos < traps.size()) {
		bool clusterSectionShouldBeFull = true;
		for (int i = 0; i < periodicClusterPattern.size(); i++) {
			if (clusterSectionShouldBeFull) {
				for (int j = 0; j < periodicClusterPattern[i]; j++) {

					if (targetPos < traps.size()) {
						clusterTargetIndices.push_back(targetPos++);
					}
				}
			} else {
				targetPos += periodicClusterPattern[i];
			}

			clusterSectionShouldBeFull = !clusterSectionShouldBeFull;
			if (targetPos >= traps.size()) {
				break;
			}
		}
		targetPos += clusterSeparation;
	}
}



// Moving traps:
// This will be the sum of the "loaded trap" waveforms for each moving trap, designated by a start
// position and end position.
//
vector<Waveform *> TrapController::combinePrecomputedWaveforms(vector<int> &destinations) {
	const size_t movingWaveformSize = loadedTrapWaveforms[0][0].dataVector.size();

	thread *workers[numWorkers];
	

	// Moving traps:
	complex<float> *movingWaveform = rearrangeWaveform.dataVector.data();

	// Add each moving waveform separately.
	for (int worker = 0; worker < numWorkers; worker++) {
		workers[worker] = new thread(&TrapController::combineRearrangeWaveform, this, movingWaveform, worker, &destinations, movingWaveformSize);
	}

	// Wait for all workers to finish combining waveforms.
	for (int worker = 0; worker < numWorkers; worker++) {
		workers[worker]->join();
	}
	// Done with rearrangement!


	vector<Waveform *> waveforms;
	waveforms.push_back(&rearrangeWaveform);
	waveforms.push_back(&staticEndingWaveform);


	return waveforms;
}


void TrapController::printTraps() {
	cout << "SDR center frequency: " << centerFrequency << " MHz" << endl;
	cout << "SDR gain: " << sdr_gain << endl;
	for (int i = 0; i < traps.size(); i++) {
		cout << i << ": " << traps[i] << endl;
	}
}


bool TrapController::sanitizeTraps(double new_gain, bool shouldPrintTotalPower) {
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
		new_gain = sdr_gain;
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

void TrapController::saveTrapStates() {
	for (int i = 0; i < traps.size(); i++) {
		traps[i].saveState();
	}
}

void TrapController::restoreTrapStates() {
	for (int i = 0; i < traps.size(); i++) {
		traps[i].restoreState();
	}
}

string filenameForLoadedTrap(int startIndex, int endIndex, int numTraps, double duration) {
	stringstream stream;
	stream << "rearrange_" << startIndex << "_to_" << endIndex << "_waveform_";
	stream << fixed << setprecision(1) << duration << "ms_" << numTraps;
	return stream.str();
}

// Assumes filenames are of the form N(A).txt, where N is the number of traps and A is the spacing in MHz.
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
		cout << "Unable to to find static waveform for: " << starting_configuration << endl;
		cout << "Aborting!!!" << endl;

		return false;
	}

	if (!staticEndingWaveform.initializeFromStaticWaveform(ending_configuration)) {
		cout << "Unable to to find static waveform for: " << ending_configuration << endl;
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

	cout << "Loading precomputed waveforms..." << flush;

	chrono::high_resolution_clock::time_point start_timer = chrono::high_resolution_clock::now();


	// Rearranging waveforms:
	for (int start_index = 0; start_index < numStartingTraps; start_index++) {
		cout << "\rLoading precomputed waveforms... moving waveforms, starting index " << start_index << "        " << flush;

		for (int dest_index = 0; dest_index < numEndingTraps; dest_index++) {
			loadedTrapWaveforms[start_index][dest_index].initializeFromMovingWaveform(moveDuration,
																					 starting_configuration, ending_configuration,
																					 start_index, dest_index);
		}
	}
	
	rearrangeWaveform.dataVector.resize(loadedTrapWaveforms[0][0].dataVector.size());

	chrono::high_resolution_clock::time_point end_timer = chrono::high_resolution_clock::now();
	double msElapsed = (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_timer)).count();

	lastLoadedWaveformProperties.duration = moveDuration;
	lastLoadedWaveformProperties.starting_configuration = starting_configuration;
	lastLoadedWaveformProperties.ending_configuration = ending_configuration;


	cout << "\rLoading precomputed waveforms... done! (Time elapsed: " << (msElapsed / 1000.0) << " s)                      " << endl;

	if (loadedTrapWaveforms[0][0].dataVector.size() == 0) {
		cout << "Unable to read waveforms in from disk!" << endl;
		cout << "Aborting!!!" << endl;
		return false;
	}
	cout << "Waveform size: " << loadedTrapWaveforms[0][0].dataVector.size() << " samples." << endl;
	return true;
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


void TrapController::testPrecomputedWaveforms() {
	//loadPrecomputedWaveforms(1.0);

	cout << "Not implemented correctly! Talk to Harry." << endl;
	return;
	
	while (true) {
		this_thread::sleep_for(chrono::seconds(2));

		vector<bool> atomsPresent;
		for (int i = 0; i < traps.size(); i++) {
			if (rand() % 3 == 0) {
				atomsPresent.push_back(true);
			}
			else {
				atomsPresent.push_back(false);
			}
		}

		cout << "Combining rearrangement waveforms for " << traps.size() << " atoms..." << endl;
		resetForRearrangement();

		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		vector<Waveform *> waveforms = rearrangeTraps(atomsPresent, REARRANGE_MODE_STACK_FROM_LEFT);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		cout << "Done combining waveforms! Time: " <<
			chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;

		cout << "Writing to file..." << endl;
		for (int i = 0; i < waveforms.size(); i++) {
			string fn("waveform");
			cout << fn + to_string(i) << endl;
			waveforms[i]->writeToBinaryFile(fn + to_string(i));
		}
		cout << "Done writing to file..." << endl;
	}
}

void TrapController::testOnTheFlyComputedWaveforms() {
	vector<bool> atomsPresent;
	for (int i = 0; i < traps.size(); i++) {
		if (rand() % 2 == 0) {
			atomsPresent.push_back(true);
		}
		else {
			atomsPresent.push_back(false);
		}
	}

	while (true) {
		this_thread::sleep_for(chrono::seconds(2));

		cout << "Combining rearrangement waveforms..." << endl;

		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		vector<Waveform *> waveforms = rearrangeTraps(atomsPresent, REARRANGE_MODE_STACK_FROM_LEFT);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		cout << "Done combining waveforms! Time: " <<
			chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;

	}
}