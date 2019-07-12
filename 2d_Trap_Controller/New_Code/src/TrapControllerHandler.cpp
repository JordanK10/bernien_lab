
#include "TrapControllerHandler.h"


/* Generate width+length trap controllers. The length primary TCs are centered
on the width axis, and create a trap per x-increment starting at the
lowest frequency (x-y axis). Same for width, but along y-x axis */
TrapControllerHandler::TrapControllerHandler( double sampleRate, double gain, int wt_freq){

  statHandler.x = &TrapController(sampleRate, gain, true,wt_freq);

  statHandler.y = &TrapController(sampleRate, gain, true,wt_freq);

  tchLen = 0;
  tchWid = 0;
}

vector<vector<double>> TrapControllerHandler::trapFrequencies() {

  vector<vector<double>> frequenciesList;

  frequenciesList.push_back(statHandler.x->trapFrequencies());
  frequenciesList.push_back(statHandler.y->trapFrequencies());

	return frequenciesList;
}

void TrapControllerHandler::resetForRearrangement() {

  statHandler.x->resetForRearrangement();
  statHandler.x->resetForRearrangement();
}

bool TrapControllerHandler::sanitizeTraps(double new_gain,
	bool shouldPrintTotalPower){

  if (!statHandler.x->sanitizeTraps(new_gain, shouldPrintTotalPower)){
    statHandler.x->traps = statHandler.x->previousTraps;
    return false;
  }

  if (!statHandler.y->sanitizeTraps(new_gain, shouldPrintTotalPower)){
    statHandler.y->traps = statHandler.y->previousTraps;
    return false;
  }
  return true;
}

void TrapControllerHandler::saveTraps(){

  statHandler.x->previousTraps = statHandler.x->traps;
  statHandler.y->previousTraps = statHandler.y->traps;


}

void TrapControllerHandler::printAvailableDefaultTrapConfigurations() {
	DIR *dir;
	struct dirent *epdf;


	dir = opendir("C:\\Users\\Bernien Lab\\Documents\\bernien_lab\\2d_Trap_Controller\\New_Code\\bin\\DefaultTrapConfigurations");

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
  ifstream config_file("C:\\Users\\Bernien Lab\\Documents\\bernien_lab\\2d_Trap_Controller\\New_Code\\bin\\DefaultTrapConfigurations\\" + filename);

  if (!config_file.is_open()) {
	  cout << "Unable to open file: " << filename << endl;
	  printAvailableDefaultTrapConfigurations();
	  return false;
  }

  saveTraps();
  statHandler.x->traps.clear();
  statHandler.y->traps.clear();

  // for(int )

  config_file >> tchLen;  config_file >> tchWid;
  int numTokensParsed=0; int numLinesParsed = 0; int numGroupsParsed = 0;

  //Generates list of token sets by group size, then sends them to corresponding trap
  vector<vector<string>> tokenList(tchLen);
  string temp;
  bool err = true;
  //Inputs trap information untill EOF
  while (config_file >> temp){
    tokenList[numLinesParsed].push_back(temp);
    numTokensParsed ++;
    if (numTokensParsed == 3) {
      numTokensParsed = 0;
      numLinesParsed ++;
      if (numLinesParsed ==tchLen) {
        if(numGroupsParsed == 0)
          err = statHandler.x->loadDefaultTrapConfiguration(tokenList,tchLen);
         if(numGroupsParsed == 1)
            err &= statHandler.y->loadDefaultTrapConfiguration(tokenList,tchWid);
        numLinesParsed = 0;
        numGroupsParsed ++;
      }
    }
    if(!err){
      cout << "bad";
      return false;

  }}

  lastLoadedConfiguration = filename;

  config_file.close();

  return true;

}

void TrapControllerHandler::printTraps(){

  cout << "x Axis Traps: \n";
  statHandler.x->printTraps();
  cout << "\ny Axis Traps: \n";
  statHandler.y->printTraps();

}

/* Assumes filenames are of the form N(A).txt, where N is the number of traps
and A is the pacing in MHz.*/
// int numTrapsForConfigurationName(string config_name) {
// 	int index_of_parens = config_name.find_first_of('(');
//
// 	if (index_of_parens < 0) {
// 		return -1;
// 	}
//
// 	string num_traps_substring = config_name.substr(0, index_of_parens);
//
// 	return stoi(num_traps_substring);
// }

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
std::vector<Waveform> TrapControllerHandler::generateStaticWaveform(){

  std::vector<Waveform> wfList;

  //Generate x,y component of static wave
  wfList.push_back(statHandler.x->generateWaveform());
  wfList.push_back(statHandler.y->generateWaveform());

  return wfList;
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
// bool TrapController::mostRecentlyLoadedCorrectWaveforms(double duration, vector<RearrangementMove> moves) {
//
//   for(int i=0;i<moves.size();i++){
//     if(mostRecentlyLoadedCorrectWaveforms(1000,moves[i].startingConfig,moves[i].endingConfig))
//       return false;
//   }
//   return true;
// }
string dimensionFormat(string str, string ins){
	int lind = str.find('x');
	str.erase(lind, str.find('t')-lind);
	str.insert(lind, ins);
  return str;
}

bool TrapControllerHandler::loadPrecomputedWaveforms(double moveDuration, string startConfig, string endConfig) {

  if(!statHandler.x->loadPrecomputedWaveforms(moveDuration,dimensionFormat(startConfig,"X"), dimensionFormat(startConfig,"X")))
    return false;

  if(!statHandler.y->loadPrecomputedWaveforms(moveDuration,dimensionFormat(startConfig,"Y"), dimensionFormat(startConfig,"Y")))
    return false;
  return true;
}

vector <RearrangementMove> TrapControllerHandler::rearrangeWaveforms(vector <RearrangementMove> moves, rearrange_mode mode) {

	vector<Waveform *> waveforms;

  for(int i=0; i<moves.size();i++){
    if(moves[i].row) //this is a row operation
      moves[i].wf = statHandler.x->combinePrecomputedWaveform(moves[i].startingConfig,moves[i].endingConfig);
    else //this is a column operation
      moves[i].wf = statHandler.y->combinePrecomputedWaveform(moves[i].startingConfig,moves[i].endingConfig);
  }

	return moves;
}

/* Moving traps: This will be the sum of the "loaded trap" waveforms for each
moving trap, designated by a start position and end position.
*/
// std::vector<std::vector<Waveform *>> TrapControllerHandler::combinePrecomputedWaveforms(
// 	vector<int> &destinations) {
//
// 	const size_t movingWaveformSize = loadedTrapWaveforms[0][0].dataVector.size();
//
// 	thread *workers[numWorkers];
//
//
// 	// Moving traps:
// 	complex<float> *movingWaveform = rearrangeWaveform.dataVector.data();
//
// 	// Add each moving waveform separately.
// 	for (int worker = 0; worker < numWorkers; worker++) {
// 		workers[worker] = new thread(&TrapController::combineRearrangeWaveform, this, movingWaveform, worker, &destinations, movingWaveformSize);
// 	}
//
// 	// Wait for all workers to finish combining waveforms.
// 	for (int worker = 0; worker < numWorkers; worker++) {
// 		workers[worker]->join();
// 	}
// 	// Done with rearrangement!
//
//
// 	vector<Waveform *> waveforms;
// 	waveforms.push_back(&rearrangeWaveform);
// 	waveforms.push_back(&staticEndingWaveform);
//
//
// 	return waveforms;
// }
