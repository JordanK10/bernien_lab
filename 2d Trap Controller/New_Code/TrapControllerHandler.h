#ifndef TRAP_CONTROLLER_HANDLER_H
#define TRAP_CONTROLLER_HANDLER_H

#include "TrapController.h"
#include <vector>

using namespace std;

struct RearrangementMove{
  std::vector<bool> startingConfig;
  std::vector<bool> endingConfig;
  int row;
  int col;
};

class TrapControllerHandler {

public:


  TrapControllerHandler(int len, int wid, double startFx, double fxIncrement, double startFy, double fyIncrement, double sampleRate, double gain);

  bool loadDefaultTrapConfiguration(std::string filename);
  void printTraps();

  void printAvailableDefaultTrapConfigurations();


  bool loadPrecomputedWaveforms(double moveDuration, string starting_configuration, string ending_configuration);
  bool mostRecentlyLoadedCorrectWaveforms(double duration, string starting_configuration, string ending_configuration);
  void initializeFromBinaryFile(std::string filename);
	bool initializeFromStaticWaveform(std::string trap_configuration);

  std::vector<RearrangementMove> generateRearrangementMoves(std::vector<std::vector<bool>> atomsPresent, enum rearrange_mode mode);

  std::vector<Waveform *> rearrangeTraps(std::vector<std::vector<bool>> atomsPresent, enum rearrange_mode mode, int modeArgument=0);

  void resetForRearrangement();

  vector<vector<double>> trapFrequencies();


  std::vector<TrapController> tcxList;
  std::vector<TrapController> tcyList;

  std::string lastLoadedConfiguration;


  int reservoirSeparation = 20;
  int tchLen;
  int tchWid;
  int numTokens = 4;

  std::vector<Waveform> staticStartingWaveform;
	std::vector<Waveform> staticEndingWaveform;

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
