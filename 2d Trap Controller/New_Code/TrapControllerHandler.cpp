
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
  cout << size << "\n";
  size = tcxList.size();
  cout << size << "\n";

}

bool TrapControllerHandler::loadDefaultTrapConfiguration(std::string filename){

  bool success = false;

  for (int i = 0; i < size; i++){

    if (tcxList[i].loadDefaultTrapConfiguration(filename,i)){
      bool success = true;
    }
  }

  cout << "\n" << success << "\n";

  return success;
}

void TrapControllerHandler::printTraps(){

  for(int i = 0; i < size; i ++){
    tcxList[i].printTraps();
  }

}
