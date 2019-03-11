#include "Console.h"
#include <iostream>
#include <vector>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>
#include <math.h>
#include <csignal>
#include <algorithm>

//#include "control_interface.h"

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

void printAWGHelp() {
	cout << "AWG command usage list:" << endl;
	cout << "awg connect" << endl;
	cout << "awg gain" << endl;
	cout << "awg gain [new value]" << endl;
	cout << "awg timeout" << endl;
	cout << "awg timeout [new value]" << endl;
	cout << "awg load_waveform [filename]" << endl;
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

void processRunCommand(vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printRunHelp();
	} else if (commandTokens[1].compare("rearrangement") == 0) {

		double moveDuration = 3.0; // In milliseconds

		// Default name of trap configurations to start and end with.
		string starting_configuration = trapControllerHandler.lastLoadedConfiguration;
		string ending_configuration = trapControllerHandler.lastLoadedConfiguration;

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

		runRearrangementSequence(trapController, awgController, moveDuration, starting_configuration, ending_configuration);
	} else {
		cout << "Run command unknown: " << commandTokens[1] << endl;
		printRunHelp();
	}
}

bool compareTrapFrequencies(Trap i, Trap j) {
	return i.frequency < j.frequency;
}


void runRearrangementSequence(TrapController &trapController, SDRController &sdrController,
							  double moveDuration, string starting_configuration, string ending_configuration) {
	// cout << "Summoning Maxwell's demon..." << endl;
	// cout << "Rearranging from " << starting_configuration << " configuration to " << ending_configuration << " in ";
	// cout << fixed << setprecision(1) << moveDuration << " ms." << endl;
  //
  //
	// if (trapController.mostRecentlyLoadedCorrectWaveforms(moveDuration, starting_configuration, ending_configuration)) {
	// 	cout << "Found precomputed rearrangement waveforms already loaded!" << endl;
	// } else {
	// 	cout << "Didn't find waveforms already in memory... reading from disk." << endl;
  //
	// 	bool readWaveforms = trapController.loadPrecomputedWaveforms(moveDuration, starting_configuration, ending_configuration);
  //
	// 	if (!readWaveforms) {
	// 		return;
	// 	}
	// }
  //
  //
	// enum rearrange_mode mode;
	// int modeArgument;
	// pickRearrangementMode(mode, modeArgument);
  //
  //
	// if (!sdrController.isConnected()) {
	// 	cout << "Error: Not connected to SDR!" << endl;
	// 	cout << "***Aborting sequence***" << endl;
	// 	return;
	// }
  //
  //
	// string lineDisplayText;
  //
  //
	// if (mode == REARRANGE_MODE_LINE_DISPLAY) {
	// 	cin.ignore(numeric_limits<streamsize>::max(), '\n');
  //
	// 	cout << "Enter text to display:" << endl;
	// 	getline(cin, lineDisplayText);
	// 	trapController.prepareLineDisplay(lineDisplayText);
	// 	trapController.printLineDisplay();
	// } else if (mode == REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR) {
	// 	int reservoirSeparation = promptForInteger("reservoir separation from array");
  //
	// 	trapController.reservoirSeparation = reservoirSeparation;
	// } else if (mode == REARRANGE_MODE_CLUSTERS || mode == REARRANGE_MODE_OPTIMIZED_CLUSTERS) {
	// 	vector<int> pattern = promptForIntegerSequence("cluster size (or pattern)");
	// 	int separation = promptForInteger("separation between clusters");
	// 	trapController.calculateClusterProperties(pattern, separation);
	// }
  //
	// Waveform slowRearrangement;
	// if (mode == REARRANGE_MODE_SLOW_VIDEO) {
	// 	slowRearrangement.initializeFromBinaryFile("../TrapController/WaveformGeneration/slow_rearrangement_0.5s");
	// }
  //
  //
  //
	// // Start sending waveform to SDR.
	// Waveform startingWaveform = trapController.staticStartingWaveform;
	// Waveform endingWaveform = trapController.staticEndingWaveform;
  //
  //
	// CameraServer cameraServer;
	// cameraServer.startServer();
  //
	// while (true) {
	// 	cout << endl << endl;
  //
	// 	cout << "Waiting for connection from CameraController..." << endl;
	// 	int successFindingCamerController = cameraServer.acceptConnection();
  //
	// 	if (!successFindingCamerController) {
	// 		cout << "Error: Failure starting server!" << endl;
	// 		cout << "***Aborting sequence***" << endl;
	// 		return;
	// 	}
  //
	// 	cout << "Received connection. Preparing for rearrangement..." << endl;
  //
  //
	// 	// Record durations.
	// 	vector<double> durations;
	// 	vector<bool> underflowRecords;
  //
  //
  //
  //
	// 	sdrController.pushWaveform(startingWaveform);
  //
  //
	// 	int numRearrangementsPerformed = 0;
	// 	while (true) {
	// 		// Start of sequence: stream static initial waveform.
	// 		sdrController.pushWaveform(startingWaveform);
  //
	// 		// Find atoms in new picture on camera:
	// 		vector<bool> atomsPresent = cameraServer.receiveIdentifiedAtomList(trapController.trapFrequencies().size());
	// 		if (atomsPresent.size() == 0) {
	// 			break;
	// 		}
  //
	// 		numRearrangementsPerformed++;
  //
	// 		resetSendTrigger();
	// 		trapController.resetForRearrangement();
  //
	// 		has_underflow = false;
  //
	// 		startTimer();
  //
  //
	// 		// Rearrange traps:
	// 		vector<Waveform *> rearrangementWaveforms = trapController.rearrangeTraps(atomsPresent, mode, modeArgument);
  //
	// 		// We want to be notified when the last waveform is pushed through to the SDR, so set a notification on the last segment of rearrangement.
	// 		// Currently not actually implemented.
	// 		//rearrangementWaveforms[0]->writeToBinaryFile("sample_rearrangement");
  //
	// 		rearrangementWaveforms[0]->shouldNotifyAfterSending = true;
	// 		sdrController.pushWaveforms(rearrangementWaveforms);
  //
	// 		if (mode == REARRANGE_MODE_SLOW_VIDEO) {
	// 			sdrController.pushWaveform(&slowRearrangement);
	// 			sdrController.pushWaveform(startingWaveform);
	// 		}
  //
  //
	// 		//sdrController.waitOnWaveformNotification();
	// 		// Record metadata
	// 		double duration = timeElapsed();
	// 		sendTrigger();
  //
	// 		//cout << "Duration from trigger -> trigger: " << duration << " ms" << endl;
	// 		durations.push_back(duration);
	// 		underflowRecords.push_back(has_underflow);
  //
	// 		cout << "Performed rearrangement " << numRearrangementsPerformed << ": ";
	// 		for (int i = 0; i < atomsPresent.size(); i++) {
	// 			if (atomsPresent[i]) {
	// 				cout << "1";
	// 			} else {
	// 				cout << "0";
	// 			}
	// 		}
	// 		cout << endl;
	// 	}
  //
	// 	//recordMetadata(seqNumber, numRepetitions, durations, underflowRecords);
  //
	// 	int numUnderflows = 0;
	// 	for (int i = 0; i < underflowRecords.size(); i++) {
	// 		if (underflowRecords[i]) {
	// 			numUnderflows++;
	// 		}
	// 	}
	// 	cout << "Number of underflows: " << numUnderflows << " out of " << underflowRecords.size() << " repetitions." << endl;
  //
  //
	// 	double avgDuration = 0;
	// 	for (int i = 0; i < durations.size(); i++) {
	// 		avgDuration += durations[i];
	// 	}
	// 	avgDuration /= (1.0 * durations.size());
  //
	// 	double durationVariance = 0;
	// 	for (int i = 0; i < durations.size(); i++) {
	// 		durationVariance += pow(avgDuration - durations[i], 2.0);
	// 	}
	// 	double stdDev = sqrt(durationVariance / durations.size());
  //
	// 	cout << "Duration from recv trigger -> send trigger: " << avgDuration << " +/- " << stdDev << endl;
  //
  //
	// }

}

// Returns whether the waveform is different now.
bool processTrapsInput(vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, StaticController &staticController, AWGController &awgController) {

	std::vector<std::vector<Trap>> previousTraps;
	for (int i = 0; i < trapControllerHandler.size; i++){
		std::vector<Trap> traps = trapControllerHandler.tcxList[i].traps;
		previousTraps.push_back(traps);
	}
	bool waveformShouldChange = false;

	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printTrapHelp();
	} else if (commandTokens[1].compare("list") == 0) {
		staticController.printTraps();
	} else if (commandTokens[1].compare("sort") == 0) {
		for (int i = 0; i < trapControllerHandler.size; i++){
			sort(trapControllerHandler.tcxList[i].traps.begin(), trapControllerHandler.tcxList[i].traps.end(), compareTrapFrequencies);
			trapControllerHandler.tcxList[i].printTraps();
		}
	} else if (commandTokens[1].compare("add") == 0) {
		if (commandTokens.size() >= 4) {
			try {
				double freqx = stod(commandTokens[2]);
				int x = stod(commandTokens[3]);
				double freqy = stod(commandTokens[4]);
				int y = stod(commandTokens[5]);
				double ampl = stod(commandTokens[6]);
				trapControllerHandler.tcxList[x].addTrap(freqx * 1.0E6, ampl);
				trapControllerHandler.tcyList[y].addTrap(freqy * 1.0E6, ampl);
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
				int x = stod(commandTokens[3]);
				int y = stod(commandTokens[5]);
				if (x < 0 || y < 0 ||  y >= trapControllerHandler.tcxList[x].traps.size() || x >= trapControllerHandler.tcyList[y].traps.size()) {
					cout << "Index out of range!" << endl;
				}
				else {
					trapControllerHandler.tcxList[x].traps.erase(	trapControllerHandler.tcxList[x].traps.begin() + x);
					trapControllerHandler.tcyList[y].traps.erase(	trapControllerHandler.tcxList[y].traps.begin() + y);
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
				int indx = stoi(commandTokens[2]);
				int indy = stoi(commandTokens[3]);
				string prop = commandTokens[4];
				double newVal = stod(commandTokens[5]);

				bool err = false;
				if (indx < 0 || indx >= trapControllerHandler.tcxList[indx].traps.size() || indy >= trapControllerHandler.tcyList[indy].traps.size()) {
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
						trapControllerHandler.tcxList[indy].traps[indx].frequency = newVal * 1.0E6;
					}
					else if (prop.compare("amp") == 0) {
						trapControllerHandler.tcxList[indy].traps[indx].amplitude = newVal;
					}
					else if (prop.compare("phase") == 0) {
						trapControllerHandler.tcxList[indy].traps[indx].setPhase(newVal);
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
			if (trapControllerHandler.loadDefaultTrapConfiguration(configuration_filename)) {
				// If a static waveform has been precomputed for this set of traps,
				// load the precomputed version rather than computing a new waveform for the same set of traps.
				Waveform static_waveform;

				bool loadedPrecomputedStaticWaveform = static_waveform.initializeFromStaticWaveform(configuration_filename);

				// If we loaded the file, then no need to generate a new waveform.
				// if (loadedPrecomputedStaticWaveform && awgController.isConnected()) {
				// 	cout << "Loaded pre-computed static waveform." << endl;
				// 	awgController.pushWaveform(static_waveform);
				// } else {
				// 	waveformShouldChange = true;
				// }
			}
		}
		else {
			cout << "Usage: load_default [default_name]" << endl;
			trapControllerHandler.printAvailableDefaultTrapConfigurations();
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
			for (int j = 0; j < trapControllerHandler.size; j++){
				for (int i = 0; i < trapControllerHandler.tcxList[j].traps.size(); i++) {
					double phase = (rand() % 10000) / 10000.0;
					trapControllerHandler.tcxList[j].traps[i].setPhase(phase);
				}
			}
		}
	else if (commandTokens[2].compare("pattern") == 0) {
			// try {
			// 	// Load pattern vector.
			// 	vector<double> phasePattern;
			// 	for (int i = 3; i < commandTokens.size(); i++) {
			// 		phasePattern.push_back(stod(commandTokens[i]));
			// 	}
			//
			// 	// If no arguments provided, set all to 0.
			// 	if (phasePattern.size() == 0) {
			// 		phasePattern.push_back(0);
			// 	}
			//
			// 	// Update phases.
			// 	for (int i = 0; i < trapController.traps.size(); i++) {
			// 		int phasePatternIndex = i % phasePattern.size();
			// 		trapController.traps[i].setPhase(phasePattern[phasePatternIndex]);
			// 	}
			// 	waveformShouldChange = true;
			//
			// }
			// catch (const std::invalid_argument&) {
			// 	cout << "Unable to parse." << endl;
			// }
		}
	else if (commandTokens[2].compare("step") == 0) {
		// try {
		// 	if (commandTokens.size() < 5) {
		// 		cout << "Usage: set_phases step [offset] [step]" << endl;
		// 	}
		// 	else {
		// 		double offset = stod(commandTokens[3]);
		// 		double step = stod(commandTokens[4]);
		//
		// 		double phase = offset;
		// 		for (int i = 0; i < trapController.traps.size(); i++) {
		// 			trapController.traps[i].setPhase(phase);
		// 			phase += step * i;
		// 			while (phase >= 1.0) {
		// 				phase -= 1.0;
		// 			}
		// 		}
		// 		waveformShouldChange = true;
		//
		// 	}
		// }
		// catch (const std::invalid_argument&) {
		// 	cout << "Unable to parse." << endl;
		// }
	}
	else {
			cout << "Unable to parse: " << commandTokens[1] << endl;
		}
	}
	else {
		if (commandTokens[1].compare("help") != 0) {
			cout << commandTokens[1] << ": trap command not found!" << endl;
		}
		printTrapHelp();
	}

	if (waveformShouldChange) {
		for (int i = 0; i < trapControllerHandler.size; i++){
				if (!trapControllerHandler.tcxList[i].sanitizeTraps()){
					trapControllerHandler.tcxList[i].traps = previousTraps[i];
					waveformShouldChange = false;
				}
		}
	}

	return waveformShouldChange;
}

bool process1DInput(vector<string> &commandTokens, TrapController &trapController, AWGController &awgController) {
	string mainCommand = commandTokens[0];

	// if (mainCommand.compare("traps") == 0) {
	// 	bool waveformShouldBeRecalculated = processTrapsInput(commandTokens, trapController, awgController);
	// 	if (waveformShouldBeRecalculated && awgController.isConnected()) {
	// 		awgController.pushWaveform(trapController.generateWaveform());
	// 	}
	// } else if (mainCommand.compare("awg") == 0) {
	// 	processawgInput(commandTokens, trapController, awgController);
	// } else if (mainCommand.compare("run") == 0) {
	// 	processRunCommand(commandTokens, trapController, awgController);
	// } else if (mainCommand.compare("adwin") == 0) {
	// 	processAdwinCommand(commandTokens);
	// } else if (mainCommand.compare("help") == 0) {
	// 	cout << "Console help menu:" << endl;
	// 	cout << "traps [trap command] [arguments]" << endl;
	// 	cout << "awg [awg command] [arguments]" << endl;
	// 	cout << "run [run command] [arguments]" << endl;
	// 	cout << "adwin [adwin command]" << endl;
	// } else if (mainCommand.compare("exit") == 0) {
	// 	awgController.disconnect();
	// 	cout << "Bye!" << endl;
	// 	return true;
	// } else {
	// 	cout << "Command unknown: " << commandTokens[0] << endl;
	// 	cout << "Type 'help' for more information." << endl;
	// }

	return false;
}

bool process2DInput(vector<string> &commandTokens, StaticController &staticController, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
	string mainCommand = commandTokens[0];

	if (mainCommand.compare("traps") == 0) {
		bool waveformShouldBeRecalculated = processTrapsInput(commandTokens, trapControllerHandler, staticController, awgController);
		// if (waveformShouldBeRecalculated && awgController.isConnected()) {
		// 	awgController.pushWaveform(trapController.generateWaveform());
		// }
	// } else if (mainCommand.compare("awg") == 0) {
	// 	processAWGInput(commandTokens, trapController, awgController);
	} else if (mainCommand.compare("run") == 0) {
		processRunCommand(commandTokens, trapControllerHandler, awgController);
	// } else if (mainCommand.compare("adwin") == 0) {
	// 	processAdwinCommand(commandTokens);
	} else if (mainCommand.compare("help") == 0) {
		cout << "Console help menu:" << endl;
		cout << "traps [trap command] [arguments]" << endl;
		cout << "awg [awg command] [arguments]" << endl;
		cout << "run [run command] [arguments]" << endl;
		cout << "adwin [adwin command]" << endl;
	} else if (mainCommand.compare("exit") == 0) {
		awgController.disconnect();
		cout << "Bye!" << endl;
		return true;
	} else {
		cout << "Command unknown: " << commandTokens[0] << endl;
		cout << "Type 'help' for more information." << endl;
	}

	return false;
}

void run1DConsole(TrapController &trapController, AWGController &awgController) {
	bool shouldExit = false;
	while (!shouldExit) {
		cout << endl;
		vector<string> commandTokens = takeInput();

		if (commandTokens.size() == 0) {
			continue;
		}

		shouldExit = process1DInput(commandTokens, trapController, awgController);
	}
}

void run2DConsole(StaticController &staticController, TrapControllerHandler trapControllerHandler, AWGController &awgController) {
	bool shouldExit = false;
	while (!shouldExit) {
		cout << endl;
		vector<string> commandTokens = takeInput();

		if (commandTokens.size() == 0) {
			continue;
		}
		trapControllerHandler.printTraps();
		shouldExit = process2DInput(commandTokens, staticController, trapControllerHandler, awgController);
	}
}
