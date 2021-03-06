#ifndef TRAP_CONTROLLER_HANDLER_H
#define TRAP_CONTROLLER_HANDLER_H

#include "TrapController.h"
#include "Rearrange2d.h"
#include <vector>
#include <thread>
#include <iostream>
using namespace std;

struct StaticHandler{
  TrapController* x;
  TrapController* y;
};

string dimensionFormat(string str, string ins);

class TrapControllerHandler {

public:


  TrapControllerHandler(double cloack_rate, double gain, int wt_freq);
  void cleanCudaModes();
  bool loadDefaultTrapConfiguration(std::string filename);
  void printTraps();

  void printAvailableDefaultTrapConfigurations();

  bool loadPrecomputedWaveforms(double moveDuration,string startConfig, string endConfig);
  bool mostRecentlyLoadedCorrectWaveforms(double duration, std::vector<RearrangementMove> moves);
  void initializeFromBinaryFile(std::string filename);
	bool initializeFromStaticWaveform(std::string trap_configuration);

  bool sanitizeTraps(double new_gain = -1,bool shouldPrintTotalPower=true);

  int rearrangeWaveforms(std::vector<RearrangementMove> moves,  rearrange_mode mode=CENTER_COM,short* pvBuffer=NULL, short* cudaBuffer = NULL, short* cudaBuffer1 = NULL);
  std::vector<Waveform> generateStaticWaveform();

  void resetForRearrangement();

  vector<vector<double>> trapFrequencies();

  void saveTraps();

  void combineRearrangeWaveform();

  StaticHandler statHandler;

  static const int numWorkers = 1;

  std::string lastLoadedConfiguration;

  double awg_gain;

  int reservoirSeparation = 20;
  int tchLen;
  int tchWid;
  int numTokens = 4;

  int mode_len;

  Waveform staticYWaveform;
  Waveform staticXWaveform;

  vector<vector<short>> xmodes;
  vector<vector<short>> ymodes;

  vector<short*> xmodesCuda;
  vector<short*> xmodesCuda2;
  vector<short*> ymodesCuda;

  /* DETERMINE LENGTH OF VECTORS */
  int size = 0;

private:
  bool yes;

  std::vector<Waveform>* rearrangedWaveforms;

  std::vector<bool> periodicClusterPattern;
  int clusterSeparation;
  int clusterSize;
  int atomsPerCluster;
  int clusterPeriodicity;
  std::vector<bool> clusterTargetIndices;
  int numClustersToBuild;

};

#endif

//
