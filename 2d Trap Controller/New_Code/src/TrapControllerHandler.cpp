
#include "TrapControllerHandler.h"


struct RearrangementMove;

/* Generate width+length trap controllers. The length primary TCs are centered
on the width axis, and create a trap per x-increment starting at the
lowest frequency (x-y axis). Same for width, but along y-x axis */
TrapControllerHandler::TrapControllerHandler(int len, double sampleRate, double gain, int wt_freq){

  for(int i = 0; i <len; i++){
    staticHandler.push_back(TrapController( i, i, sampleRate, gain, true,wt_freq));
  }
  size = staticHandler.size();

  tchLen = 0;
  tchWid = 0;
}

vector<vector<double>> TrapControllerHandler::trapFrequencies() {

  vector<vector<double>> frequenciesList;
  for (int i = 0; i < staticHandler.size(); i++){
    frequenciesList.push_back(staticHandler[i].trapFrequencies());
  }

	return frequenciesList;
}

void TrapControllerHandler::resetForRearrangement() {
  for (int i = 0; i < staticHandler.size(); i++){
    staticHandler[i].resetForRearrangement();
  }
}

bool TrapControllerHandler::sanitizeTraps(double new_gain,
	bool shouldPrintTotalPower){
  for (int i = 0; i < size; i++){
      if (!staticHandler[i].sanitizeTraps(new_gain, shouldPrintTotalPower)){
        staticHandler[i].traps = staticHandler[i].previousTraps;
        return false;
      }
  }
  return true;
}

void TrapControllerHandler::saveTraps(){
  for(int i = 0; i<staticHandler.size();i++){
    staticHandler[i].previousTraps = staticHandler[i].traps;
  }
}

void TrapControllerHandler::printAvailableDefaultTrapConfigurations() {
	DIR *dir;
	struct dirent *epdf;


	dir = opendir("../DefaultTrapConfigurations");

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
  ifstream config_file("../bin/DefaultTrapConfigurations/" + filename);

  if (!config_file.is_open()) {
	  cout << "Unable to open file: " << filename << endl;
	  printAvailableDefaultTrapConfigurations();
	  return false;
  }

  saveTraps();
  for(int i =0; i<staticHandler.size(); i++){
    staticHandler[i].traps.clear();
  }

  config_file >> tchLen;  config_file >> tchWid;
  int numTokensParsed=0; int numLinesParsed = 0; int numGroupsParsed = 0;

  //Generates list of token sets by group size, then sends them to corresponding trap
  vector<vector<string>> tokenList(tchLen);
  string temp;
  //Inputs trap informationuntill EOF
  while (config_file >> temp){
	tokenList[numLinesParsed].push_back(temp);
	numTokensParsed ++;
    if (numTokensParsed == 4) {
      numTokensParsed = 0;
      numLinesParsed ++;
      if (numLinesParsed ==tchLen) {
        if(!staticHandler[numGroupsParsed].loadDefaultTrapConfiguration(tokenList,tchLen))
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
    staticHandler[i].printTraps();
  }

}

/* Assumes filenames are of the form N(A).txt, where N is the number of traps
and A is the pacing in MHz.*/
int numTrapsForConfigurationName(string config_name) {
	int index_of_parens = config_name.find_first_of('(');

	if (index_of_parens < 0) {
		return -1;
	}

	string num_traps_substring = config_name.substr(0, index_of_parens);

	return stoi(num_traps_substring);
}

void TrapControllerHandler::initializeFromBinaryFile(string binaryFilename) {
	string path(dir);
	path.append(binaryFilename);

	ifstream binaryFile(path, ios::in | ios::binary);
	unsigned int numElements;
	vector<complex<float>> dataVector;
	vector<complex<float>> tempVector;
	binaryFile.read(reinterpret_cast<char*>(&numElements), sizeof(unsigned int));

	dataVector.resize(numElements);
	binaryFile.read(reinterpret_cast<char*>(&dataVector[0]), numElements * sizeof(dataVector[0]));

  int j=0;
  for(int i=0; i<dataVector.size(); i++){
    break;
  }
}

bool fileExists(string filename) {
	string path(dir);
	path.append(filename);

	ifstream file(path, ios::in | ios::binary);
	return file.good();
}

bool TrapControllerHandler::initializeFromStaticWaveform(string trap_configuration_file) {
	int length = trap_configuration_file.length();
	string waveform_filename = trap_configuration_file.substr(0, length - 4) + "_static"; // Remove .txt, replace with _static.

	string waveform_path = static_waveforms + waveform_filename;

	if (fileExists(waveform_path)) {
		initializeFromBinaryFile(waveform_path);
		return true;
	} else {
		return false;
	}
}

// Iterates through all trapcontrollers, generating a list of waveforms to
//  to return
std::vector<Waveform> TrapControllerHandler::generateWaveform(){

  std::vector<Waveform> wfList;

  for ( int i =0; i < tchLen; i++)
    wfList.push_back(staticHandler[i].generateWaveform());

  return wfList;
}

bool TrapControllerHandler::loadPrecomputedWaveforms(double moveDuration, string starting_configuration, string ending_configuration) {

  // if (!staticStartingWaveform.initializeFromStaticWaveform(starting_configuration)) {
	// 	cout << "Unable to to find static waveform for: " << starting_configuration << endl;
	// 	cout << "Aborting!!!" << endl;
  //
	// 	return false;
	// }
  //
	// if (!staticEndingWaveform.initializeFromStaticWaveform(ending_configuration)) {
	// 	cout << "Unable to to find static waveform for: " << ending_configuration << endl;
	// 	cout << "Aborting!!!" << endl;
  //
	// 	return false;
	// }
  //
	// // Use configuration names (up to first open parens) to determine the number of traps before and after rearrangement.
	// numStartingTraps = numTrapsForConfigurationName(starting_configuration);
	// numEndingTraps = numTrapsForConfigurationName(ending_configuration);
  //
	// if (numStartingTraps <= 0 || numEndingTraps <= 0) {
	// 	cout << "Error: starting with " << numStartingTraps << " and ending with " << numEndingTraps << "!" << endl;
	// 	cout << "Aborting!!!" << endl;
  //
	// 	return false;
	// }
  //
  //
	// cout << "Loading precomputed waveforms from " << numStartingTraps << " traps to " << numEndingTraps << " traps." << endl;
	// cout << "\t" << fixed << setprecision(1) << moveDuration << " ms move time" << endl << endl;
  //
	// cout << "Loading precomputed waveforms..." << flush;
  //
	// chrono::high_resolution_clock::time_point start_timer = chrono::high_resolution_clock::now();
  //
  //
	// // Rearranging waveforms:
	// for (int start_index = 0; start_index < numStartingTraps; start_index++) {
	// 	cout << "\rLoading precomputed waveforms... moving waveforms, starting index " << start_index << "        " << flush;
  //
	// 	for (int dest_index = 0; dest_index < numEndingTraps; dest_index++) {
	// 		loadedTrapWaveforms[start_index][dest_index].initializeFromMovingWaveform(moveDuration,
	// 																				 starting_configuration, ending_configuration,
	// 																				 start_index, dest_index);
	// 	}
	// }
  //
	// rearrangeWaveform.dataVector.resize(loadedTrapWaveforms[0][0].dataVector.size());
  //
	// chrono::high_resolution_clock::time_point end_timer = chrono::high_resolution_clock::now();
	// double msElapsed = (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_timer)).count();
  //
	// lastLoadedWaveformProperties.duration = moveDuration;
	// lastLoadedWaveformProperties.starting_configuration = starting_configuration;
	// lastLoadedWaveformProperties.ending_configuration = ending_configuration;
  //
  //
	// cout << "\rLoading precomputed waveforms... done! (Time elapsed: " << (msElapsed / 1000.0) << " s)                      " << endl;
  //
	// if (loadedTrapWaveforms[0][0].dataVector.size() == 0) {
	// 	cout << "Unable to read waveforms in from disk!" << endl;
	// 	cout << "Aborting!!!" << endl;
	// 	return false;
	// }
	// cout << "Waveform size: " << loadedTrapWaveforms[0][0].dataVector.size() << " samples." << endl;
	return true;
}

bool TrapControllerHandler::mostRecentlyLoadedCorrectWaveforms(double duration, string starting_configuration, string ending_configuration) {
	// if (lastLoadedWaveformProperties.starting_configuration.compare(starting_configuration) != 0) {
	// 	return false;
	// }
  //
	// if (lastLoadedWaveformProperties.ending_configuration.compare(ending_configuration) != 0) {
	// 	return false;
	// }
  //
	// if (lastLoadedWaveformProperties.duration != duration) {
	// 	return false;
	// }

	return true;
}

void TrapController::printAvailableDefaultTrapConfigurations() {
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

std::vector<RearrangementMove> TrapControllerHandler::generateRearrangementMoves(std::vector<std::vector<bool>> atomsPresent, enum rearrange_mode mode){

  std::vector<RearrangementMove> moveList;

  while(1)
    break;

  return moveList;
}

vector<Waveform *> TrapControllerHandler::rearrangeTraps(vector<vector<bool>> atomsPresent, enum rearrange_mode mode, int modeArgument) {
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

	// if (mode == REARRANGE_MODE_STACK_FROM_LEFT) {
	// 	// Stack all atoms to the left.
  //
	// 	int numPresentToLeft = 0;
	// 	for (int trap_index = 0; trap_index < atomsPresent.size(); trap_index++) {
	// 		if (atomsPresent[trap_index]) {
	// 			destinations.push_back(numPresentToLeft++);
	// 		} else {
	// 			destinations.push_back(-1);
	// 		}
	// 	}
  //

	// return combinePrecomputedWaveforms(destinations);
  vector <Waveform *> blank;
  return blank;
}
