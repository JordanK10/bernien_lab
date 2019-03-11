#ifndef TRAP_CONTROLLER_HANDLER_H
#define TRAP_CONTROLLER_HANDLER_H

#include "TrapController.h"
#include <vector>

using namespace std;

class TrapControllerHandler {

public:

  TrapControllerHandler(int len, int wid, double startFx, double fxIncrement, double startFy, double fyIncrement, double sampleRate, double gain);

  bool loadDefaultTrapConfiguration(std::string filename);
  void printTraps();

  void printAvailableDefaultTrapConfigurations();


  std::vector<TrapController> tcxList;
  std::vector<TrapController> tcyList;

  std::string lastLoadedConfiguration;

  int tchLen;
  int tchWid;
  int numTokens = 4;


  /* DETERMINE LENGTH OF VECTORS */
  int size = 0;

private:
  bool yes;

};

#endif

//
