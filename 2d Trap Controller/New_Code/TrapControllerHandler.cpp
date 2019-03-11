
#include "TrapControllerHandler.h"

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
#include <vector>


/* Generate width+length trap controllers. The length primary TCs are centered
on the width axis, and create a trap per x-increment starting at the
lowest frequency (x-y axis). Same for width, but along y-x axis */
TrapControllerHandler::TrapControllerHandler(int len, int wid, double startFx, double fxIncrement, double startFy, double fyIncrement, double sampleRate, double gain){

  for(int i = 0; i <len; i++){
    tcxList.push_back(TrapController( (startFx + i*fxIncrement), (startFy+fyIncrement*wid/2), sampleRate, gain, true));
  }
  for(int i = 0; i <wid; i++){
    tcyList.push_back(TrapController( (startFx +fyIncrement*len/2), (startFy+i*fyIncrement), sampleRate, gain, false));
  }
  size = tcxList.size();


}

void TrapControllerHandler::printAvailableDefaultTrapConfigurations() {
	DIR *dir;
	struct dirent *epdf;


	dir = opendir("./DefaultTrapConfigurations");

	vector<string> filenames;

	if (dir != NULL) {
		cout << "Available default trap configurations:" << endl;
		while ((epdf = readdir(dir))) {
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

// Loads a trap configuration from a file
bool TrapControllerHandler::loadDefaultTrapConfiguration(std::string filename){

  //Opens filestream from a given file. First two inputs are L/wW
  ifstream config_file("./DefaultTrapConfigurations/" + filename);
  config_file >> tchLen;  config_file >> tchWid;
  int numTokensParsed=0; int numLinesParsed = 0; int numGroupsParsed = 0;

  //Generates list of token sets by group size, then sends them to corresponding trap
  vector<vector<string>> tokenList;
  tokenList.resize(tchLen, std::vector<string>(numTokens, " "));

  //Inputs trap informationuntill EOF
  while (config_file >> tokenList[numLinesParsed][numTokensParsed]){
    numTokensParsed ++;
    if (numTokensParsed == 4) {
      numTokensParsed = 0;
      numLinesParsed ++;
      if (numLinesParsed ==tchLen) {
        if(!tcxList[numGroupsParsed].loadDefaultTrapConfiguration(tokenList,tchLen))
          return false;
        numLinesParsed = 0;
        numGroupsParsed ++;
        }
      }
    }

  lastLoadedConfiguration = filename;

  config_file.close();

  return true;

}

void TrapControllerHandler::printTraps(){

  for(int i = 0; i < size; i ++){
    tcxList[i].printTraps();
  }

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
