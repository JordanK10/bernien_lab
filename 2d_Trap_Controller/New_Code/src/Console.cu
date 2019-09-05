#include "Console.h"

double m_dur =1;

//#include "control_interface.h"

struct RearrangementMove;

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
	cout << "traps clean" << endl;
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
	cout << "0: Arrang atoms in the Center." << endl;
	cout << "1: Stack from upper left corner." << endl;
	cout << "2: Stack from upper right corner." << endl;
	cout << "3: Stack from lower left corner." << endl;
	cout << "4: Stack from lower right corner." << endl;
	cout << "5: Stack from closest corner." << endl;
	cout << "6: Rectangular from the left." << endl;
	cout << "7: Rectangular from the right." << endl;
	cout << "8: Rectangular from the center." << endl;


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
	// if (mode == REARRANGE_MODE_FIXED_ARRAY_WITHOUT_RESERVOIR || mode == REARRANGE_MODE_FIXED_ARRAY_WITH_RESERVOIR || mode == REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR) {
	// 	int enteredTargetNum = -1;
	//
	// 	while (true) {
	// 		enteredTargetNum = promptForInteger("target array size");
	//
	// 		if (enteredTargetNum >= 0) {
	// 			break;
	// 		}
	// 		cout << "Please pick a non-negative target size." << endl;
	// 	}
	//
	// 	modeArgument = enteredTargetNum;
	// }
}

void runRearrangementSequence(TrapControllerHandler &trapControllerHandler, AWGController &awgController,
							  double moveDuration, string starting_configuration, string ending_configuration) {

	if (trapControllerHandler.tchWid>0){
		cout << "Summoning Maxwell's demon..." << endl;
		cout << "Rearranging from " << starting_configuration << " configuration to " << ending_configuration << " in ";
		cout << fixed << setprecision(1) << moveDuration << " ms." << endl;

		rearrange_method method = BALANCE_COMPRESS;
		rearrange_mode mode;
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

			// We push the static waveforms to the static traps
			awgController.pushStaticWaveforms(trapControllerHandler.generateStaticWaveform(),true);
			// #include <fstream>
			// ofstream myFile;
			// myFile.open("c:/users/bernien_lab/desktop/38_moves_comp_time_data.csv");
			// // Keeping track of number of rearrangements
			int numRearrangementsPerformed = 0;
			while (true) {
				// Find atoms in new picture on camera:
				// for(int i = 0;i<10000;i++){
				std::vector<std::vector<bool>> atomsPresent = cameraServer.receiveIdentifiedAtomList(trapControllerHandler.trapFrequencies().size(),trapControllerHandler.tchLen);
				if (atomsPresent.size() == 0) {
					cout << "Camera server returned an empty list. Aborting...\n";
					break;
				}

				numRearrangementsPerformed++;

				//Setting for rearrangement
				trapControllerHandler.resetForRearrangement();

				startTimer();
				vector<RearrangementMove> moves = rearrange(atomsPresent,method,mode);
				// RearrangementMove temp_move;
				// vector<RearrangementMove> moves;
				// temp_move.startingConfig = {1,1,1,1,1,1,1,1,1,1};
				// temp_move.endingConfig = {9,-1,-1,-1,-1,-1,-1,-1,-1,-1};
				// temp_move.row = false;
				// temp_move.dim = 0;
				// moves.push_back(temp_move);
				//
				// temp_move.startingConfig = {1,1,1,1,1,1,1,1,1,1};
				// temp_move.endingConfig = {-1,-1,-1,-1,-1,-1,-1,-1,-1,0};
				// temp_move.row = false;
				// temp_move.dim = 0;
				// moves.push_back(temp_move);

				int duration = timeElapsed();



				int move_len = trapControllerHandler.rearrangeWaveforms(moves,mode,awgController.getDynamicBuffer(),awgController.getCudaBuffer(),awgController.getCudaBuffer2());
				int duration2 = timeElapsed();
				 // myFile << moves.size() << "," << duration2-duration << endl;
				 // }
				awgController.pushRearrangeWaveforms(moves.size(),move_len);
				int duration3 = timeElapsed();
				cout << "Software rearrange time: " << duration << " ms" << endl;
				cout << "Waveform generation  time: " << duration2-duration << " ms" << endl; //this took 13 ms longer
				cout << "Pushing waveform time -> trigger: " << duration3-duration2-duration << " ms" << endl;
				durations.push_back(duration);
				break;
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
			break;

		}
	}else{
		cout << "No traps present... Aborting\n";
	}

}

void processRunCommand(std::vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printRunHelp();
	} else if (commandTokens[1].compare("rearrangement") == 0) {

		double moveDuration = m_dur; //in milliseconds


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

void processAWGInput(vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printAWGHelp();
	} else if (commandTokens[1].compare("connect") == 0) {
		if (awgController.isConnected()) {
			cout << "AWG already connected!" << endl;
		} else {
			awgController.pushStaticWaveforms(trapControllerHandler.generateStaticWaveform(),true);
		}
	} else if (commandTokens[1].compare("disconnect") == 0) {
		if (awgController.isConnected()) {
			awgController.disconnect();
		} else {
			cout << "Already disconnected!" << endl;
		}
	} else if (commandTokens[1].compare("gain") == 0) {
			cout << "There is currently no gain functionality\n";
	// 	if (commandTokens.size() == 2) {
	// 		cout << "AWG Gain: " << awgController.getGain() << endl;
	// 	} else if (commandTokens.size() >= 3) {
	// 		try {
	// 			double gain = stod(commandTokens[2]);
	// 			if (trapControllerHandler.sanitizeTraps(gain)) {
	//
	// 				bool success = awgController.changeGain(gain);
	//
	// 				if (success) {
	// 					cout << "Set AWG Gain to " << awgController.getGain() << endl;
	// 					trapControllerHandler.awg_gain = awgController.getGain();
	// 				} else {
	// 					cout << "Unable to change gain - not connected to AWG!" << endl;
	// 				}
	// 			} else {
	// 				cout << "Unable to change gain - too much power!" << endl;
	// 			}
	// 		} catch (const invalid_argument&) {
	// 			cout << "Unable to parse gain!" << endl;
	// 		}
	// 	}
	} else if (commandTokens[1].compare("load_waveform") == 0) {
		if (commandTokens.size() >= 3) {
			string filename = commandTokens[2];
			Waveform w(filename);

			// awgController.pushWaveform(w);
		} else {
			cout << "Usage: awg load_waveform [filename]" << endl;
			cout << "NOTE: Please make sure that the waveform is sanitized" << endl;
			cout << " and that the gain is good before loading a waveform." << endl;
		}
	} else if (commandTokens[1].compare("push_waveform") == 0) {
		awgController.pushStaticWaveforms(trapControllerHandler.generateStaticWaveform(),true);
	} else if (commandTokens[1].compare("push_trans_waveform") == 0) {
		awgController.pushStaticWaveforms(trapControllerHandler.generateStaticWaveform(),false);
	} else if (commandTokens[1].compare("trigger") == 0) {
		awgController.triggerSequence();
	}else if (commandTokens[1].compare("switch") == 0) {
		if(commandTokens[2].compare("fifo") == 0)
			awgController.changeMode(FIFO);
		else if(commandTokens[2].compare("single") == 0)
			awgController.changeMode(SINGLE);
	} else if (commandTokens[1].compare("run") == 0) {
		awgController.run(0,1);
		return;
	} else if (commandTokens[1].compare("stop") == 0){
		awgController.stop();
	}else {
		printAWGHelp();
	}
}

// Returns whether the waveform is different now.
bool processTrapsInput(std::vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {

	std::vector<std::vector<Trap>> previousTraps;

	previousTraps.push_back(trapControllerHandler.statHandler.x->traps);
	previousTraps.push_back(trapControllerHandler.statHandler.y->traps);

	bool waveformShouldChange = false;

	if (commandTokens.size() == 1 || commandTokens[1].compare("help") == 0) {
		printTrapHelp();
	} else if (commandTokens[1].compare("list") == 0) {
		trapControllerHandler.printTraps();
	} else if (commandTokens[1].compare("sort") == 0) {
			sort(trapControllerHandler.statHandler.x->traps.begin(), trapControllerHandler.statHandler.x->traps.end(), compareTrapFrequencies);
			trapControllerHandler.statHandler.x->printTraps();
			sort(trapControllerHandler.statHandler.y->traps.begin(), trapControllerHandler.statHandler.y->traps.end(), compareTrapFrequencies);
			trapControllerHandler.statHandler.y->printTraps();
	} else if (commandTokens[1].compare("add") == 0) {
		if (commandTokens.size() >= 4) {
			try {
				double freqx = stod(commandTokens[2]);
				int x = stod(commandTokens[3]);
				double freqy = stod(commandTokens[4]);
				int y = stod(commandTokens[5]);
				double ampl = stod(commandTokens[6]);
				trapControllerHandler.statHandler.x->addTrap(freqx * 1.0E6, ampl);
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
				if (x < 0 || y < 0 ||  y >= trapControllerHandler.statHandler.x->traps.size()) {
					cout << "Index out of range!" << endl;
				}
				else {
					trapControllerHandler.statHandler.x->traps.erase(	trapControllerHandler.statHandler.x->traps.begin() + x);
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
	} else if (commandTokens[1].compare("clean") == 0) {
		for(int i = 0;i<trapControllerHandler.tchLen;i++){
			for(int j = 0;j<trapControllerHandler.tchLen;j++){
				if(numDevices == 1){
					cudaSetDevice(defaultDevice);
					cudaFree(trapControllerHandler.statHandler.x->loadedCudaWaveforms[i][j]);
				}
				if(numDevices == 2){
					cudaSetDevice(0);
					cudaFree(trapControllerHandler.statHandler.x->loadedCudaWaveforms[i][j]);
					cudaSetDevice(1);
					cudaFree(trapControllerHandler.statHandler.x->loadedCudaWaveforms2[i][j]);
				}
			}
		}
		awgController.cleanCudaBuffer();
		trapControllerHandler.cleanCudaModes();

		for(int i=0; i<trapControllerHandler.tchLen; i++){
			trapControllerHandler.statHandler.x->traps.erase(	trapControllerHandler.statHandler.x->traps.begin());
		}
		// for(int i=0; i<trapControllerHandler.tchWid; i++){
		// 	trapControllerHandler.statHandler.y->traps.erase(	trapControllerHandler.statHandler.y->traps.begin());
		// }

	} else if (commandTokens[1].compare("change") == 0) {
		if (commandTokens.size() >= 5) {
			try {
				int indx = stoi(commandTokens[2]);
				int indy = stoi(commandTokens[3]);
				string prop = commandTokens[4];
				double newVal = stod(commandTokens[5]);

				bool err = false;
				if (indx < 0 || indx >= trapControllerHandler.statHandler.x->traps.size()) {
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
						trapControllerHandler.statHandler.x->traps[indx].frequency = newVal * 1.0E6;
					}
					else if (prop.compare("amp") == 0) {
						trapControllerHandler.statHandler.x->traps[indx].amplitude = newVal;
					}
					else if (prop.compare("phase") == 0) {
						trapControllerHandler.statHandler.x->traps[indx].setPhase(newVal);
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
				//continue;

				awgController.allocateDynamicWFBuffer(m_dur, trapControllerHandler.statHandler.x->traps.size(),trapControllerHandler.statHandler.x->traps.size());
				cout << "Loading precomputed waveforms from disk." << endl;

				// Default name of trap configurations to start and end with.
				string starting_configuration = trapControllerHandler.lastLoadedConfiguration;
				string ending_configuration = trapControllerHandler.lastLoadedConfiguration;

				if (!trapControllerHandler.loadPrecomputedWaveforms(m_dur, starting_configuration, ending_configuration)){
					return false;
				}
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
			for (int i = 0; i < trapControllerHandler.statHandler.x->traps.size(); i++) {
				double phase = (rand() % 10000) / 10000.0;
				trapControllerHandler.statHandler.x->traps[i].setPhase(phase);
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
		trapControllerHandler.sanitizeTraps();
		waveformShouldChange = false;
		}
		return waveformShouldChange;
}

bool process2DInput(std::vector<string> &commandTokens, TrapControllerHandler &trapControllerHandler, AWGController &awgController) {
	string mainCommand = commandTokens[0];

	if (mainCommand.compare("traps") == 0) {
		bool waveformShouldBeRecalculated = processTrapsInput(commandTokens, trapControllerHandler, awgController);
		if (waveformShouldBeRecalculated && awgController.isConnected()) {
				awgController.pushStaticWaveforms(trapControllerHandler.generateStaticWaveform(),true);
		}
 } else if (mainCommand.compare("awg") == 0) {
	 	processAWGInput(commandTokens, trapControllerHandler, awgController);
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
		for(int i = 0;i<trapControllerHandler.tchLen;i++){
			for(int j = 0;j<trapControllerHandler.tchLen;j++){
				//free(trapControllerHandler.statHandler.x->loadedTrapWaveforms[i][j]);
				cudaFree(trapControllerHandler.statHandler.x->loadedCudaWaveforms[i][j]);
				if(numDevices == 2){
					cudaFree(trapControllerHandler.statHandler.x->loadedCudaWaveforms2[i][j]);
				}
			}
		}
		awgController.cleanCudaBuffer();
		trapControllerHandler.cleanCudaModes();
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
			std::vector<string> commandTokens = takeInput();
		if (commandTokens.size() == 0) {
			continue;
		}
		shouldExit = process2DInput(commandTokens, trapControllerHandler, awgController);
	}
}
