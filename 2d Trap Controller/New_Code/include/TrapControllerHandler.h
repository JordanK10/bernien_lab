#ifndef TRAP_CONTROLLER_HANDLER_H
#define TRAP_CONTROLLER_HANDLER_H

#include "TrapController.h"
#include "Rearrange2d.h"
#include <vector>
#include <thread>

using namespace std;

struct RearrangementMove;

class TrapControllerHandler {

public:


  TrapControllerHandler(int len, double cloack_rate, double gain, int wt_freq);

  bool loadDefaultTrapConfiguration(std::string filename);
  void printTraps();

  void printAvailableDefaultTrapConfigurations();


  bool loadPrecomputedWaveforms(double moveDuration, string starting_configuration, string ending_configuration);
  bool mostRecentlyLoadedCorrectWaveforms(double duration, string starting_configuration, string ending_configuration);
  void initializeFromBinaryFile(std::string filename);
	bool initializeFromStaticWaveform(std::string trap_configuration);

  bool sanitizeTraps(double new_gain = -1,bool shouldPrintTotalPower=true);

  std::vector<Waveform *> rearrangeTraps(std::vector<std::vector<bool>> atomsPresent,  rearrange_mode mode, int modeArgument=0);
  std::vector<std::vector<Waveform *>> TrapControllerHandler::combinePrecomputedWaveforms(vector<int> &destinations);
  std::vector<Waveform> generateWaveform();

  void resetForRearrangement();

  vector<vector<double>> trapFrequencies();

  void saveTraps();


  std::vector<TrapController> staticHandler;


  std::string lastLoadedConfiguration;

  double awg_gain;

  int reservoirSeparation = 20;
  int tchLen;
  int tchWid;
  int numTokens = 4;

  Waveform staticYWaveform;
  Waveform staticXWaveform;

  /* DETERMINE LENGTH OF VECTORS */
  int size = 0;

private:
  bool yes;

  std::vector<int> periodicClusterPattern;
  int clusterSeparation;
  int clusterSize;
  int atomsPerCluster;
  int clusterPeriodicity;
  std::vector<int> clusterTargetIndices;
  int numClustersToBuild;

};

#endif

//
