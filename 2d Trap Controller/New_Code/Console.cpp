#include "Console.h"

//#include "control_interface.h"

extern struct RearrangementMove;


static const int DYNAMIC_AWG_X = 1;
static const int DYNAMIC_AWG_Y = 2;
static const int STATIC_AWG_X = 3;
static const int STATIC_AWG_Y = 4;

std::vector<string> parseCommand(string &cmd) {
	if (cmd[cmd.size() - 1] == '\n') {
		cmd.pop_back();
	}

	std::vector<string> tokens;

	stringstream ss(cmd);
	string item;

	while (getline(ss, item, ' ')) {
		if (item.size() > 0) {
			tokens.push_back(item);
		}
	}


	return tokens;
}

static chrono::high_resolution_clock::time_point start_timer;
void startTimer() {
	start_timer = chrono::high_resolution_clock::now();
}

double timeElapsed() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_timer).count();
}

std::vector<string> takeInput() {
	cout << "#: ";
	string input;
	getline(cin,input);

	if (input[input.size() - 1] == '\n') {
		input.pop_back();
	}

	return parseCommand(input);
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

std::vector<int> promptForIntegerSequence(string attr) {

	while (true) {
		cout << "Enter " << attr << ": ";
		string input;
		getline(cin,input);
		std::vector<string> tokens = parseCommand(input);

		if (tokens.size() > 0) {
			bool allTokensValid = true;
			std::vector<int> intSequence;

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

void runRearrangementSequence(TrapControllerHandler &trapControllerHandler, AWGController &awgController,
							  double moveDuration, string starting_configuration, string ending_configuration) {
									return;
	cout << "Summoning Maxwell's demon..." << endl;
	cout << "Rearranging from " << starting_configuration << " configuration to " << ending_configuration << " in ";

	enum rearrange_mode mode;
	int modeArgument;
	pickRearrangementMode(mode, modeArgument);


	 if (!awgController.isConnected()) {
	 	cout << "Error: Not connected to AWG!" << endl;
	 	cout << "***Aborting sequence***" << endl;
		return;
	}

	// Start sending waveform to AWG.
	Waveform startingXWaveform = trapControllerHandler.staticXWaveform;
	Waveform startingYWaveform = trapControllerHandler.staticYWaveform;


	CameraServer cameraServer;
	cameraServer.startServer();


	while (true) {


		// Record durations.
		std::vector<double> durations;
		std::vector<bool> underflowRecords;

		// Keeping track of number of rearrangements	
		int numRearrangementsPerformed = 0;
		while (true) {


			// Find atoms in new picture on camera:
			std::vector<std::vector<bool>> atomsPresent = cameraServer.receiveIdentifiedAtomList(trapControllerHandler.trapFrequencies().size(),trapControllerHandler.tchLen);
			if (atomsPresent.size() == 0) {
				break;
			}

			numRearrangementsPerformed++;

			trapControllerHandler.resetForRearrangement();


			startTimer();

			std::vector <RearrangementMove> moves = trapControllerHandler.generateRearrangementMoves(atomsPresent,mode);

			// Rearrange traps:
			std::vector<Waveform *> rearrangementWaveforms = trapControllerHandler.rearrangeTraps(atomsPresent, mode, modeArgument);

			// We want to be notified when the last waveform is pushed through to the SDR, so set a notification on the last segment of rearrangement.
			// Currently not actually implemented.
			//rearrangementWaveforms[0]->writeToBinaryFile("sample_rearrangement");

			rearrangementWaveforms[0]->shouldNotifyAfterSending = true;
			awgController.pushWaveforms(rearrangementWaveforms);

			//SlowMo Rearrangement
			// if (mode == REARRANGE_MODE_SLOW_VIDEO) {
			// 	awgController.pushWaveform(&slowRearrangement);
			// 	awgController.pushWaveform(startingWaveform);
			// }

			//sdrController.waitOnWaveformNotification();
			// Record metadata
			double duration = timeElapsed();


			//cout << "Duration from trigger -> trigger: " << duration << " ms" << endl;
			durations.push_back(duration);

			cout << "Performed rearrangement " << numRearrangementsPerformed << ": ";
			for (int i = 0; i < atomsPresent.size(); i++) {
				for (int j = 0; j < atomsPresent.size(); j++){
					if (atomsPresent[i][j]) {
						cout << "1";
					} else {
						cout << "0";
					}
				}
				cout << endl;
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


void processRunCommand(std::vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
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

		runRearrangementSequence(trapControllerHandler, awgController, moveDuration, starting_configuration, ending_configuration);
	} else {
		cout << "Run command unknown: " << commandTokens[1] << endl;
		printRunHelp();
	}
}

bool compareTrapFrequencies(Trap i, Trap j) {
	return i.frequency < j.frequency;
}


// Returns whether the waveform is different now.
bool processTrapsInput(std::vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {

	std::vector<std::vector<Trap>> previousTraps;
	for (int i = 0; i < trapControllerHandler.size; i++){
		std::vector<Trap> traps = trapControllerHandler.tcxList[i].traps;
		previousTraps.push_back(traps);
	}
	bool waveformShouldChange = false;

	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printTrapHelp();
	} else if (commandTokens[1].compare("list") == 0) {
		//printTraps
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

bool process2DInput(std::vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
	string mainCommand = commandTokens[0];

	if (mainCommand.compare("traps") == 0) {
		bool waveformShouldBeRecalculated = processTrapsInput(commandTokens, trapControllerHandler, awgController);
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

void run2DConsole( TrapControllerHandler trapControllerHandler, AWGController &awgController) {
	bool shouldExit = false;
	while (!shouldExit) {
		cout << endl;
		std::vector<string> commandTokens = takeInput();

		if (commandTokens.size() == 0) {
			continue;
		}
		trapControllerHandler.printTraps();
		shouldExit = process2DInput(commandTokens, trapControllerHandler, awgController);
	}
}
