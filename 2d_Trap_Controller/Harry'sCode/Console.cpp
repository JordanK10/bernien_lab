// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "Console.h"
#include <iostream>
#include <vector>
#include <string>
#include <readline.h>
#include <history.h>
#include <math.h>
#include <csignal>
#include <algorithm>

#include "control_interface.h"
#include "CameraServer.h"

using namespace std;

vector<string> parseCommand(string &cmd) {
	if (cmd[cmd.size() - 1] == '\n') {
		cmd.pop_back();
	}

	vector<string> tokens;

	stringstream ss(cmd);
	string item;

	while (getline(ss, item, ' ')) {
		if (item.size() > 0) {
			tokens.push_back(item);
		}
	}


	return tokens;
}

vector<string> takeInput() {
	string input(readline("#: "));
	add_history(input.c_str());

	if (input[input.size() - 1] == '\n') {
		input.pop_back();
	}

	return parseCommand(input);
}

void printTrapHelp() {
	cout << "Trap command usage list:" << endl;
	cout << "traps list" << endl;
	cout << "traps sort" << endl;
	cout << "traps add [freq] [amp]" << endl;
	cout << "traps change [index] [property: freq, amp, phase] [new value]" << endl;
	cout << "traps delete [index]" << endl;
	cout << "traps load_default [default_name: 100(0.49).txt, 70(0.70).txt]" << endl;
	cout << "traps set_phases [phase arguments]" << endl;
}

void printCameraHelp() {
	cout << "Camera command usage list:" << endl;
	cout << "camera connect" << endl;
}

void printSDRHelp() {
	cout << "SDR command usage list:" << endl;
	cout << "sdr connect" << endl;
	cout << "sdr gain" << endl;
	cout << "sdr gain [new value]" << endl;
	cout << "sdr timeout" << endl;
	cout << "sdr timeout [new value]" << endl;
	cout << "sdr load_waveform [filename]" << endl;
}

void printRunHelp() {
	cout << "Run command usage list:" << endl;
	cout << "run rearrangement" << endl;
	cout << "run rearrangement [duration (ms)]" << endl;
	cout << "run rearrangement [fade duration] [move duration]" << endl;
}

void printAdwinHelp() {
	cout << "Adwin command usage list:" << endl;
	cout << "adwin send_trigger" << endl;
}

bool compareTrapFrequencies(Trap i, Trap j) {
	return i.frequency < j.frequency;
}

// Returns whether the waveform is different now.
bool processTrapsInput(vector<string> &commandTokens, TrapController &trapController, SDRController &sdrController) {
	vector<Trap> previousTraps = trapController.traps;
	bool waveformShouldChange = false;


	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printTrapHelp();
	} else if (commandTokens[1].compare("list") == 0) {
		trapController.printTraps();
	} else if (commandTokens[1].compare("sort") == 0) {
		sort(trapController.traps.begin(), trapController.traps.end(), compareTrapFrequencies);
		trapController.printTraps();
	} else if (commandTokens[1].compare("add") == 0) {
		if (commandTokens.size() >= 4) {
			try {
				double freq = stod(commandTokens[2]);
				double ampl = stod(commandTokens[3]);
				trapController.addTrap(freq * 1.0E6, ampl);
				waveformShouldChange = true;
			}
			catch (const invalid_argument&) {
				cout << "Unable to parse values as numbers!" << endl;
			}
		}
		else {
			cout << "Usage: add [freq in MHz] [amplitude]" << endl;
		}
	} else if (commandTokens[1].compare("delete") == 0) {
		if (commandTokens.size() >= 3) {
			try {
				int index = stoi(commandTokens[2]);
				if (index < 0 || index >= trapController.traps.size()) {
					cout << "Index out of range!" << endl;
				}
				else {
					trapController.traps.erase(trapController.traps.begin() + index);
					waveformShouldChange = true;
				}
			}
			catch (const std::invalid_argument&) {
				cout << "Unable to parse index!" << endl;
			}
		}
		else {
			cout << "Usage: delete [trap index]" << endl;
		}
	} else if (commandTokens[1].compare("change") == 0) {
		if (commandTokens.size() >= 5) {
			try {
				int index = stoi(commandTokens[2]);
				string prop = commandTokens[3];
				double newVal = stod(commandTokens[4]);

				bool err = false;
				if (index < 0 || index >= trapController.traps.size()) {
					cout << "Index out of range!" << endl;
					err = true;
				}
				else if (prop.compare("freq") != 0 && prop.compare("amp") != 0 && prop.compare("phase") != 0) {
					cout << "Must select property freq, amp, or phase!" << endl;
					err = true;
				}

				if (err) {
					cout << "Usage: change [trap index] [property: freq, amp, or phase] [new value]" << endl;
				}
				else {
					waveformShouldChange = true;
					if (prop.compare("freq") == 0) {
						trapController.traps[index].frequency = newVal * 1.0E6;
					}
					else if (prop.compare("amp") == 0) {
						trapController.traps[index].amplitude = newVal;
					}
					else if (prop.compare("phase") == 0) {
						trapController.traps[index].setPhase(newVal);
					}
				}
			}
			catch (const std::invalid_argument&) {
				cout << "Unable to parse values!" << endl;
			}
		}
		else {
			cout << "Usage: change [trap index] [property: freq,amp,phase] [new value]" << endl;
		}
	}
	else if (commandTokens[1].compare("load_default") == 0) {
		if (commandTokens.size() >= 3) {
			string configuration_filename = commandTokens[2];
			if (trapController.loadDefaultTrapConfiguration(configuration_filename)) {
				// If a static waveform has been precomputed for this set of traps,
				// load the precomputed version rather than computing a new waveform for the same set of traps.
				Waveform static_waveform;

				bool loadedPrecomputedStaticWaveform = static_waveform.initializeFromStaticWaveform(configuration_filename);

				// If we loaded the file, then no need to generate a new waveform.
				if (loadedPrecomputedStaticWaveform && sdrController.isConnected()) {
					cout << "Loaded pre-computed static waveform." << endl;
					sdrController.pushWaveform(static_waveform);
				} else {
					waveformShouldChange = true;
				}
			}
		}
		else {
			cout << "Usage: load_default [default_name]" << endl;
			trapController.printAvailableDefaultTrapConfigurations();
		}
	}
	else if (commandTokens[1].compare("set_phases") == 0) {
		if (commandTokens.size() < 3) {
			cout << "Usages:" << endl;
			cout << "Random phases: set_phases random" << endl;
			cout << "Cyclic pattern: set_phases pattern [phase1] [phase2] [phase3] ..." << endl;
			cout << "Phase step: set_phases step [first_phase] [phase_step]" << endl;
		}
		else if (commandTokens[2].compare("random") == 0) {
			waveformShouldChange = true;
			for (int i = 0; i < trapController.traps.size(); i++) {
				double phase = (rand() % 10000) / 10000.0;
				trapController.traps[i].setPhase(phase);
			}
		}
		else if (commandTokens[2].compare("pattern") == 0) {
			try {
				// Load pattern vector.
				vector<double> phasePattern;
				for (int i = 3; i < commandTokens.size(); i++) {
					phasePattern.push_back(stod(commandTokens[i]));
				}

				// If no arguments provided, set all to 0.
				if (phasePattern.size() == 0) {
					phasePattern.push_back(0);
				}

				// Update phases.
				for (int i = 0; i < trapController.traps.size(); i++) {
					int phasePatternIndex = i % phasePattern.size();
					trapController.traps[i].setPhase(phasePattern[phasePatternIndex]);
				}
				waveformShouldChange = true;

			}
			catch (const std::invalid_argument&) {
				cout << "Unable to parse." << endl;
			}
		}
		else if (commandTokens[2].compare("step") == 0) {
			try {
				if (commandTokens.size() < 5) {
					cout << "Usage: set_phases step [offset] [step]" << endl;
				}
				else {
					double offset = stod(commandTokens[3]);
					double step = stod(commandTokens[4]);

					double phase = offset;
					for (int i = 0; i < trapController.traps.size(); i++) {
						trapController.traps[i].setPhase(phase);
						phase += step * i;
						while (phase >= 1.0) {
							phase -= 1.0;
						}
					}
					waveformShouldChange = true;

				}
			}
			catch (const std::invalid_argument&) {
				cout << "Unable to parse." << endl;
			}
		}
		else {
			cout << "Unable to parse: " << commandTokens[1] << endl;
		}
	} else if (commandTokens[1].compare("precompute_waveforms") == 0) {
		cout << "This command actually no longer works! Sorry." << endl;
		//trapController.precomputeWaveforms();
		//trapController.loadPrecomputedWaveforms();
	} else {
		if (commandTokens[1].compare("help") != 0) {
			cout << commandTokens[1] << ": trap command not found!" << endl;
		}
		printTrapHelp();
	}


	if (waveformShouldChange && !trapController.sanitizeTraps()) {
		trapController.traps = previousTraps;
		waveformShouldChange = false;
	}

	return waveformShouldChange;
}


void processSDRInput(vector<string> &commandTokens, TrapController &trapController, SDRController &sdrController) {
	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printSDRHelp();
	} else if (commandTokens[1].compare("connect") == 0) {
		if (sdrController.isConnected()) {
			cout << "SDR already connected!" << endl;
		} else {
			sdrController.initializeUHD();
			sdrController.startStreaming();

			sdrController.pushWaveform(trapController.generateWaveform());

		}
	} else if (commandTokens[1].compare("disconnect") == 0) {
		if (sdrController.isConnected()) {
			sdrController.disconnect();
		} else {
			cout << "Already disconnected!" << endl;
		}
	} else if (commandTokens[1].compare("gain") == 0) {
		if (commandTokens.size() == 2) {
			cout << "SDR Gain: " << sdrController.getGain() << endl;
		} else if (commandTokens.size() >= 3) {
			try {
				double gain = stod(commandTokens[2]);
				if (trapController.sanitizeTraps(gain)) {

					bool success = sdrController.changeGain(gain);

					if (success) {
						cout << "Set SDR Gain to " << sdrController.getGain() << endl;
						trapController.sdr_gain = sdrController.getGain();
					} else {
						cout << "Unable to change gain - not connected to SDR!" << endl;
					}
				} else {
					cout << "Unable to change gain - too much power!" << endl;
				}
			} catch (const invalid_argument&) {
				cout << "Unable to parse gain!" << endl;
			}
		}
	} else if (commandTokens[1].compare("load_waveform") == 0) {
		if (commandTokens.size() >= 3) {
			string filename = commandTokens[2];
			Waveform w(filename);

			sdrController.pushWaveform(w);
		} else {
			cout << "Usage: sdr load_waveform [filename]" << endl;
			cout << "NOTE: Please make sure that the waveform is sanitized" << endl;
			cout << " and that the gain is good before loading a waveform." << endl;
		}
	} else {
		printSDRHelp();
	}
}


static chrono::high_resolution_clock::time_point start_timer;
void startTimer() {
	start_timer = chrono::high_resolution_clock::now();
}

double timeElapsed() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_timer).count();
}

int promptForInteger(string attr) {
	int val;
	cout << "Enter " << attr << ": ";
	while (!(cin >> val)) {
		cout << "Unable to parse value." << endl;
		cin.clear();
		cin.ignore(0xfffff, '\n');
		cout << "Enter " << attr << ": " << endl;
	}

	return val;
}

vector<int> promptForIntegerSequence(string attr) {

	while (true) {
		cout << "Enter " << attr << ": ";
		string input(readline(""));
		vector<string> tokens = parseCommand(input);

		if (tokens.size() > 0) {
			bool allTokensValid = true;
			vector<int> intSequence;

			for (int i = 0; i < tokens.size(); i++) {
				try {
					int val = stoi(tokens[i]);
					intSequence.push_back(val);
				}
				catch (const std::invalid_argument&) {
					cout << "Unable to parse as integer: " << tokens[i] << endl;
					allTokensValid = false;
					break;
				}
			}

			if (allTokensValid) {
				return intSequence;
			}

		} else {
			cout << "Unable to parse cluster integer sequence. Example: 2 3 2" << endl;
		}
	}
}

double promptForDouble(string attr) {
	double val;
	cout << "Enter " << attr << ": ";
	while (!(cin >> val)) {
		cout << "Unable to parse value." << endl;
		cin.clear();
		cin.ignore(0xfffff, '\n');
		cout << "Enter " << attr << ": " << endl;
	}

	return val;
}

void pickRearrangementMode(enum rearrange_mode &mode, int &modeArgument) {
	cout << "Please select a rearrangement mode:" << endl;
	cout << "0: Stack atoms from the left." << endl;
	cout << "1: Stack atoms from the right." << endl;
	cout << "2: Stack leftmost N atoms on the left." << endl;
	cout << "3: Stack leftmost N atoms on the left, and build a reservoir on the far right." << endl;
	cout << "4: Stack leftmost N atoms on the left, and build a reservoir nearby." << endl;
	cout << "5: Line display!!!" << endl;
	cout << "6: Prepare clusters of atoms (superlattice)." << endl;
	cout << "7: Prepare clusters of atoms (improved routing!)." << endl;
	cout << "8: Quick rearrangement to 'random loading', then slow rearrangement to 40 atom array." << endl;


	int enteredMode = -1;
	while (true) {
		enteredMode = promptForInteger("rearrangement mode");

		if (enteredMode >= 0 && enteredMode <= 8) {
			break;
		}
		cout << "Please pick a mode from 0 to 8." << endl;
	}
	mode = (enum rearrange_mode)enteredMode;

	// The mode argument is only used for options 2 and 3.
	if (mode == REARRANGE_MODE_FIXED_ARRAY_WITHOUT_RESERVOIR || mode == REARRANGE_MODE_FIXED_ARRAY_WITH_RESERVOIR || mode == REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR) {
		int enteredTargetNum = -1;

		while (true) {
			enteredTargetNum = promptForInteger("target array size");

			if (enteredTargetNum >= 0) {
				break;
			}
			cout << "Please pick a non-negative target size." << endl;
		}

		modeArgument = enteredTargetNum;
	}
}


void runRearrangementSequence(TrapController &trapController, SDRController &sdrController,
							  double moveDuration, string starting_configuration, string ending_configuration) {
	cout << "Summoning Maxwell's demon..." << endl;
	cout << "Rearranging from " << starting_configuration << " configuration to " << ending_configuration << " in ";
	cout << fixed << setprecision(1) << moveDuration << " ms." << endl;


	if (trapController.mostRecentlyLoadedCorrectWaveforms(moveDuration, starting_configuration, ending_configuration)) {
		cout << "Found precomputed rearrangement waveforms already loaded!" << endl;
	} else {
		cout << "Didn't find waveforms already in memory... reading from disk." << endl;

		bool readWaveforms = trapController.loadPrecomputedWaveforms(moveDuration, starting_configuration, ending_configuration);

		if (!readWaveforms) {
			return;
		}
	}


	enum rearrange_mode mode;
	int modeArgument;
	pickRearrangementMode(mode, modeArgument);


	if (!sdrController.isConnected()) {
		cout << "Error: Not connected to SDR!" << endl;
		cout << "***Aborting sequence***" << endl;
		return;
	}


	string lineDisplayText;


	if (mode == REARRANGE_MODE_LINE_DISPLAY) {
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		cout << "Enter text to display:" << endl;
		getline(cin, lineDisplayText);
		trapController.prepareLineDisplay(lineDisplayText);
		trapController.printLineDisplay();
	} else if (mode == REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR) {
		int reservoirSeparation = promptForInteger("reservoir separation from array");

		trapController.reservoirSeparation = reservoirSeparation;
	} else if (mode == REARRANGE_MODE_CLUSTERS || mode == REARRANGE_MODE_OPTIMIZED_CLUSTERS) {
		vector<int> pattern = promptForIntegerSequence("cluster size (or pattern)");
		int separation = promptForInteger("separation between clusters");
		trapController.calculateClusterProperties(pattern, separation);
	}

	Waveform slowRearrangement;
	if (mode == REARRANGE_MODE_SLOW_VIDEO) {
		slowRearrangement.initializeFromBinaryFile("../TrapController/WaveformGeneration/slow_rearrangement_0.5s");
	}



	// Start sending waveform to SDR.
	Waveform startingWaveform = trapController.staticStartingWaveform;
	Waveform endingWaveform = trapController.staticEndingWaveform;


	CameraServer cameraServer;
	cameraServer.startServer();

	while (true) {
		cout << endl << endl;

		cout << "Waiting for connection from CameraController..." << endl;
		int successFindingCamerController = cameraServer.acceptConnection();

		if (!successFindingCamerController) {
			cout << "Error: Failure starting server!" << endl;
			cout << "***Aborting sequence***" << endl;
			return;
		}

		cout << "Received connection. Preparing for rearrangement..." << endl;


		// Record durations.
		vector<double> durations;
		vector<bool> underflowRecords;




		sdrController.pushWaveform(startingWaveform);


		int numRearrangementsPerformed = 0;
		while (true) {
			// Start of sequence: stream static initial waveform.
			sdrController.pushWaveform(startingWaveform);

			// Find atoms in new picture on camera:
			vector<bool> atomsPresent = cameraServer.receiveIdentifiedAtomList(trapController.trapFrequencies().size());
			if (atomsPresent.size() == 0) {
				break;
			}

			numRearrangementsPerformed++;

			resetSendTrigger();
			trapController.resetForRearrangement();

			has_underflow = false;

			startTimer();


			// Rearrange traps:
			vector<Waveform *> rearrangementWaveforms = trapController.rearrangeTraps(atomsPresent, mode, modeArgument);

			// We want to be notified when the last waveform is pushed through to the SDR, so set a notification on the last segment of rearrangement.
			// Currently not actually implemented.
			//rearrangementWaveforms[0]->writeToBinaryFile("sample_rearrangement");

			rearrangementWaveforms[0]->shouldNotifyAfterSending = true;
			sdrController.pushWaveforms(rearrangementWaveforms);

			if (mode == REARRANGE_MODE_SLOW_VIDEO) {
				sdrController.pushWaveform(&slowRearrangement);
				sdrController.pushWaveform(startingWaveform);
			}


			//sdrController.waitOnWaveformNotification();
			// Record metadata
			double duration = timeElapsed();
			sendTrigger();

			//cout << "Duration from trigger -> trigger: " << duration << " ms" << endl;
			durations.push_back(duration);
			underflowRecords.push_back(has_underflow);

			cout << "Performed rearrangement " << numRearrangementsPerformed << ": ";
			for (int i = 0; i < atomsPresent.size(); i++) {
				if (atomsPresent[i]) {
					cout << "1";
				} else {
					cout << "0";
				}
			}
			cout << endl;
		}

		//recordMetadata(seqNumber, numRepetitions, durations, underflowRecords);

		int numUnderflows = 0;
		for (int i = 0; i < underflowRecords.size(); i++) {
			if (underflowRecords[i]) {
				numUnderflows++;
			}
		}
		cout << "Number of underflows: " << numUnderflows << " out of " << underflowRecords.size() << " repetitions." << endl;


		double avgDuration = 0;
		for (int i = 0; i < durations.size(); i++) {
			avgDuration += durations[i];
		}
		avgDuration /= (1.0 * durations.size());

		double durationVariance = 0;
		for (int i = 0; i < durations.size(); i++) {
			durationVariance += pow(avgDuration - durations[i], 2.0);
		}
		double stdDev = sqrt(durationVariance / durations.size());

		cout << "Duration from recv trigger -> send trigger: " << avgDuration << " +/- " << stdDev << endl;


	}

}


void processRunCommand(vector<string> &commandTokens, TrapController &trapController, SDRController &sdrController) {
	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printRunHelp();
	} else if (commandTokens[1].compare("rearrangement") == 0) {

		double moveDuration = 3.0; // In milliseconds

		// Default name of trap configurations to start and end with.
		string starting_configuration = trapController.lastLoadedConfiguration;
		string ending_configuration = trapController.lastLoadedConfiguration;

		// If parameters passed to call, then replace the default values.
		for (int tokenIndex = 2; tokenIndex + 1 < commandTokens.size(); tokenIndex += 2) {
			string indicator = commandTokens[tokenIndex];
			string arg = commandTokens[tokenIndex + 1];

			if (indicator.compare("-d") == 0) {
				try {
					double dur = stod(arg);
					moveDuration = dur;
				}
				catch (const std::invalid_argument&) {
					cout << "Unable to parse duration!" << endl;
				}
			} else if (indicator.compare("-start") == 0) {
				starting_configuration = arg;
			} else if (indicator.compare("-end") == 0) {
				ending_configuration = arg;
			}
		}

		runRearrangementSequence(trapController, sdrController, moveDuration, starting_configuration, ending_configuration);
	} else {
		cout << "Run command unknown: " << commandTokens[1] << endl;
		printRunHelp();
	}
}

void processAdwinCommand(vector<string> &commandTokens) {
	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printAdwinHelp();
	} else if (commandTokens[1].compare("send_trigger") == 0) {
		sendTrigger();
	}
}

bool processInput(vector<string> &commandTokens, TrapController &trapController, SDRController &sdrController) {
	string mainCommand = commandTokens[0];

	if (mainCommand.compare("traps") == 0) {
		bool waveformShouldBeRecalculated = processTrapsInput(commandTokens, trapController, sdrController);
		if (waveformShouldBeRecalculated && sdrController.isConnected()) {
			sdrController.pushWaveform(trapController.generateWaveform());
		}
	} else if (mainCommand.compare("sdr") == 0) {
		processSDRInput(commandTokens, trapController, sdrController);
	} else if (mainCommand.compare("run") == 0) {
		processRunCommand(commandTokens, trapController, sdrController);
	} else if (mainCommand.compare("adwin") == 0) {
		processAdwinCommand(commandTokens);
	} else if (mainCommand.compare("help") == 0) {
		cout << "Console help menu:" << endl;
		cout << "traps [trap command] [arguments]" << endl;
		cout << "sdr [sdr command] [arguments]" << endl;
		cout << "run [run command] [arguments]" << endl;
		cout << "adwin [adwin command]" << endl;
	} else if (mainCommand.compare("exit") == 0) {
		sdrController.disconnect();
		cout << "Bye!" << endl;
		return true;
	} else {
		cout << "Command unknown: " << commandTokens[0] << endl;
		cout << "Type 'help' for more information." << endl;
	}

	return false;
}

void runConsole(TrapController &trapController, SDRController &sdrController) {
	bool shouldExit = false;
	while (!shouldExit) {
		cout << endl;
		vector<string> commandTokens = takeInput();

		if (commandTokens.size() == 0) {
			continue;
		}

		shouldExit = processInput(commandTokens, trapController, sdrController);
	}
}
