

#ifndef STATIC_CONTROLLER_H
#define STATIC_CONTROLLER_H

#include "WaveTable.h"
#include "Waveform.h"
#include "Trap.h"

#include <vector>

class StaticController {

public:

    StaticController(double centerFreq, double sampleRate, double gain);

    void addTrap(double freq, double amp, double phase=0);

    void printTraps();

    double xCenterFrequency;
    double yCenterFrequency;
    double sdr_gain;
  private:
  	//void combineRearrangeWaveform(std::complex<float> *movingWaveform, int worker, std::vector<int> *destinations, const size_t movingWaveformSize);


  	//LineDisplay lineDisplay;
  	int lineDisplayCounter;


  //	WaveTable *waveTable;
  	//Waveform rearrangeWaveform;

  	//struct loadedWaveformProperties lastLoadedWaveformProperties;
  	//Waveform loadedTrapWaveforms[MAX_NUM_TRAPS][MAX_NUM_TRAPS];
  	int numStartingTraps;
  	int numEndingTraps;


  	std::vector<int> periodicClusterPattern;
  	int clusterSeparation;
  	int clusterSize;
  	int atomsPerCluster;
  	int clusterPeriodicity;
  	std::vector<int> clusterTargetIndices;
  	int numClustersToBuild;


  };


#endif
