
#include "TrapControllerHandler.h"
#include <fstream>

/* Generate width+length trap controllers. The length primary TCs are centered
on the width axis, and create a trap per x-increment starting at the
lowest frequency (x-y axis). Same for width, but along y-x axis */
TrapControllerHandler::TrapControllerHandler( double sampleRate, double gain, int wt_freq){

statHandler.y = new TrapController(sampleRate, gain, true,wt_freq);
statHandler.x = new TrapController(sampleRate, gain, true,wt_freq);

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


	dir = opendir("C:\\Users\\bernien_lab\\Documents\\bernien_lab\\2d_Trap_Controller\\New_Code\\bin\\DefaultTrapConfigurations");

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

//void TrapControllerHanlder::setModes(vector<Waveform> modes, bool x){
//
//    if(x)
//      xmodes = modes;
//    if(!x)
//      ymodes = modes;
//}

// Loads a trap configuration from a file
bool TrapControllerHandler::loadDefaultTrapConfiguration(std::string filename){

  //Opens filestream from a given file. First two inputs are L/wW
  ifstream config_file("C:\\Users\\Bernien_Lab\\Documents\\bernien_lab\\2d_Trap_Controller\\New_Code\\bin\\DefaultTrapConfigurations\\" + filename);

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
  xmodes = statHandler.x->generateModes();
  ymodes = statHandler.y->generateModes();


  size_t size = xmodes[0].size()*sizeof(short);
  short* tempMode = NULL;
  short* tempMode2 = NULL;
  short* tempMode3 = NULL;
  for(int i = 0;i<xmodes.size();i++){//load the xmodes into xmodesCuda, which is
                                    //located on the GPU. If using 2 devices, Also
                                    //load xmodesCuda2 onto the second device
      tempMode = NULL;
      tempMode2 = NULL;
      tempMode3 = NULL;
      tempMode2 = static_cast<short*>(malloc(size));
      for(int j = 0;j<xmodes[0].size();j++){
        tempMode2[j] = xmodes[i][j];
      }
      if(numDevices == 1){
        cudaSetDevice(defaultDevice);
        err =  cudaMalloc((void **)&tempMode, size); if(err != cudaSuccess){cout << "Memory Allocation Error (modes)"<<endl;}
        err = cudaMemcpy(tempMode,tempMode2,size,cudaMemcpyHostToDevice); if(err != cudaSuccess){cout << "Memory Transfer Error (modes)" << endl;}
        xmodesCuda.push_back(tempMode);
      }
      if(numDevices == 2){
        cudaSetDevice(0);
        err = cudaMalloc((void **)&tempMode, size); if(err != cudaSuccess){cout << "Memory Allocation Error (modes)"<<endl;}
        err = cudaMemcpy(tempMode,tempMode2,size,cudaMemcpyHostToDevice); if(err != cudaSuccess){cout << "Memory Transfer Error (modes)" << endl;}
        xmodesCuda.push_back(tempMode);
        cudaSetDevice(1);
        err = cudaMalloc((void **)&tempMode3, size); if(err != cudaSuccess){cout << "Memory Allocation Error (modes)"<<endl;}
        err = cudaMemcpy(tempMode3,tempMode2,size,cudaMemcpyHostToDevice);if(err != cudaSuccess){cout << "Memory Transfer Error (modes)" << endl;}else{free(tempMode2);}
        xmodesCuda2.push_back(tempMode3);
      }
  }
  // for(int i = 0;i<ymodes.size();i++){
  //     tempMode = NULL;
  //     tempMode2 = NULL;
  //     tempMode2 = static_cast<short*>(malloc(size));
  //     for(int j = 0;j<ymodes[0].size();j++){
  //       tempMode2[j] = ymodes[i][j];
  //     }
  //     err =  cudaMalloc((void **)&tempMode, size);
  //     if(err != cudaSuccess){cout << "Memory Allocation Error"<<endl;}
  //     err = cudaMemcpy(tempMode,tempMode2,size,cudaMemcpyHostToDevice);
  //     if(err != cudaSuccess){cout << "Memory Transfer Error" << endl;}else{free(tempMode2);}
  //     ymodesCuda.push_back(tempMode);
  // }
  mode_len = ymodes[0].size();

  return true;

}

void TrapControllerHandler::printTraps(){

  cout << "x Axis Traps: \n";
  statHandler.x->printTraps();
  cout << "\ny Axis Traps: \n";
  statHandler.y->printTraps();

}

/* Assumes filenames are of the form N(A).txt, where N is the number of traps
and A is the spacing in MHz.*/
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
  wfList.push_back(statHandler.x->generateWaveform());

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

  // if(!statHandler.y->loadPrecomputedWaveforms(moveDuration,dimensionFormat(startConfig,"Y"), dimensionFormat(startConfig,"Y")))
  //   return false;

  if(!statHandler.x->loadPrecomputedWaveforms(moveDuration,dimensionFormat(startConfig,"X"), dimensionFormat(startConfig,"X")))
    return false;

  return true;
}

void TrapControllerHandler::cleanCudaModes(){//free modes from the GPU memory
  for(int i = 0;i<tchLen;i++){
    if(numDevices == 1){
      cudaSetDevice(defaultDevice);
      cudaFree(xmodesCuda[i]);
    }
    if(numDevices == 2){
      cudaSetDevice(0);
      cudaFree(xmodesCuda[i]);
      cudaSetDevice(1);
      cudaFree(xmodesCuda2[i]);
    }
  }
}

int TrapControllerHandler::rearrangeWaveforms(vector <RearrangementMove> moves, rearrange_mode mode, short* pvBuffer, short* cudaBuffer,short* cudaBuffer1) {
    const size_t movingWaveformSize = statHandler.x->getWFSize();
    int num_moves = moves.size();
    size_t size;
    size_t size1;
    cudaError_t err = cudaSuccess;
    auto start = chrono::high_resolution_clock::now();

    if(numDevices == 2){ //divide the moves in half if using 2 GPUs
      int n = num_moves/2 + num_moves%2;
      size1 = movingWaveformSize*n*sizeof(short)*2;
      size = movingWaveformSize*(num_moves - n)*sizeof(short)*2;
    }else{
      size = movingWaveformSize*num_moves*sizeof(short)*2;
    }

    if(numDevices == 2){
      int k = 0;
      for(int i=0; i<num_moves;i++){    //all data is now in statHandler.x
                                        //since statHandler.y is a duplicate
        if(i>=num_moves/2){
          statHandler.x->combinePrecomputedWaveform(moves[i].endingConfig, (xmodesCuda2[moves[i].dim]),k,cudaBuffer1,moves[i].row,mode_len, movingWaveformSize, num_moves,1);
          k++;
        }else{
          statHandler.x->combinePrecomputedWaveform(moves[i].endingConfig, (xmodesCuda[moves[i].dim]),i,cudaBuffer,moves[i].row,mode_len, movingWaveformSize, num_moves,0);
        }
      }
      err = cudaSetDevice(0); if(err != cudaSuccess){cout << "Device Set Error" << endl;}
      err = cudaMemcpyAsync(pvBuffer,cudaBuffer,size,cudaMemcpyDeviceToHost); if(err != cudaSuccess){cout << "Mem transfer error: " << cudaGetErrorString(err) << endl;}
      err = cudaSetDevice(1); if(err != cudaSuccess){cout << "Device Set Error: " << cudaGetErrorString(err) << endl;}
      err = cudaMemcpyAsync(&pvBuffer[size/sizeof(short)],cudaBuffer1,size1,cudaMemcpyDeviceToHost); if(err != cudaSuccess){cout << "Mem transfer error: " << cudaGetErrorString(err) << endl;}
    }

    if(numDevices == 1){
      for(int i=0; i<num_moves;i++){    //all data is now in statHandler.x
                                        //since statHandler.y is a duplicate
        statHandler.x->combinePrecomputedWaveform(moves[i].endingConfig, (xmodesCuda[moves[i].dim]),i,cudaBuffer,moves[i].row,mode_len, movingWaveformSize, num_moves,defaultDevice);
      }
      err = cudaSetDevice(defaultDevice); if(err != cudaSuccess){cout << "Device Set Error" << endl;}
      err = cudaMemcpyAsync(pvBuffer,cudaBuffer,size,cudaMemcpyDeviceToHost); if(err != cudaSuccess){cout << "Mem transfer error: " << cudaGetErrorString(err) << endl;}
    }
    cudaDeviceSynchronize();

    double dur = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();
    cout << "Data Calculation Rate: " << movingWaveformSize*num_moves*2*sizeof(short)/dur/1e6 << " GB/s" << endl;

    // Transfer pvBuffer To Text File:

    // cout << "\n\nTransferring data to text file\n" << endl;
    // ofstream myFile;
    // ofstream myFile2;
    // myFile.open("c:/users/bernien_lab/desktop/sample_move_ch1.txt");
    // myFile2.open("c:/users/bernien_lab/desktop/sample_move_ch2.txt");
    // for(int k = 0;k<movingWaveformSize*num_moves;k++){
    //   myFile << pvBuffer[2*k] << endl;
    //   myFile2 << pvBuffer[2*k+1] << endl;
    // }
    // myFile.close();
    // myFile2.close();
    // cout << "\nTransfer Complete\n\n" << endl;

    // Print Samples from the pvBuffer:

    // for(int k = 0;k<movingWaveformSize*num_moves*2;k+=2000000){
    //   cout << k << " " << pvBuffer[k] << endl;
    // }

	return statHandler.x->getWFSize();
}
