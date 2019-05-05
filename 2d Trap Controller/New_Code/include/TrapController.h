/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#ifndef TRAP_CONTROLLER_H
#define TRAP_CONTROLLER_H

#include "Trap.h"
#include "Waveform.h"

#include <iomanip>
#include <thread>
#include <stdlib.h>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <dirent.h>
#include <fstream>

#include <vector>

#define DEFAULT_WAVEFORM_DURATION 0.00001
#define MAX_NUM_TRAPS 101

using namespace std;



static const int numWorkers = 6;


struct loadedWaveformProperties {
	std::string starting_configuration;
	std::string ending_configuration;
	double duration;
};


class TrapController {

public:

    TrapController(double centerFx, double centerFy, double sampleRate, double gain, bool axis, int wt_freq);

    void addTrap(double freq, double amp, double phase=0);

    void setCenter(double xaxis, double freq);

		Waveform generateWaveform(double duration = DEFAULT_WAVEFORM_DURATION);

		bool loadDefaultTrapConfiguration(std::vector<std::vector<std::string>> tokenList, int groupSize);

    std::vector<Trap> traps;
		std::vector<Trap> previousTraps;


		bool sanitizeTraps(double new_gain = -1, bool shouldPrintTotalPower=true);

		void printTraps();

		std::vector<double> trapFrequencies();

		void resetForRearrangement();

		void printAvailableDefaultTrapConfigurations();

		vector<Waveform *> rearrangeTraps(std::vector<bool> atomsPresent, enum rearrange_mode mode, int modeArgument);

    double awg_gain;
    double xAxisCenterFreq;
    double yAxisCenterFreq;

		std::vector<std::complex<float>> getWaveTable();


		Waveform staticStartingWaveform;
		Waveform staticEndingWaveform;

private:
  	void combineRearrangeWaveform(std::complex<float> *movingWaveform, int worker, std::vector<int> *destinations, const size_t movingWaveformSize);

    bool majorAxisx;

  	//LineDisplay lineDisplay;
  	int lineDisplayCounter;

  	WaveTable *waveTable;
  	Waveform rearrangeWaveform;

  	struct loadedWaveformProperties lastLoadedWaveformProperties;
  	// Waveform loadedTrapWaveforms[MAX_NUM_TRAPS][MAX_NUM_TRAPS];
  	int numStartingTraps;
  	int numEndingTraps;

		Waveform loadedTrapWaveforms[MAX_NUM_TRAPS][MAX_NUM_TRAPS];



  	std::vector<int> periodicClusterPattern;
  	int clusterSeparation;
  	int clusterSize;
  	int atomsPerCluster;
  	int clusterPeriodicity;
  	std::vector<int> clusterTargetIndices;
  	int numClustersToBuild;
  };

#endif
