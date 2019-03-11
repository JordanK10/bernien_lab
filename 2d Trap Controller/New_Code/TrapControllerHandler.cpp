
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
