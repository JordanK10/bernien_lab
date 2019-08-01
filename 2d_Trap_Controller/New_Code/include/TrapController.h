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
// #define DEFAULT_WAVEFORM_DURATION 0.0000003
#define MAX_NUM_TRAPS 50

using namespace std;



static const int numWorkers = 15;


struct loadedWaveformProperties {
	std::string starting_configuration;
	std::string ending_configuration;
	double duration;
};


class TrapController {

public:

    TrapController( double sampleRate, double gain, bool axis, int wt_freq);

    void addTrap(double freq, double amp, double phase=0);

    void setCenter(double xaxis, double freq);

		Waveform generateWaveform(double duration = DEFAULT_WAVEFORM_DURATION);
		vector<vector<short>> generateModes();
		bool loadDefaultTrapConfiguration(std::vector<std::vector<std::string>> tokenList, int groupSize);

    std::vector<Trap> traps;
		std::vector<Trap> previousTraps;

		bool mostRecentlyLoadedCorrectWaveforms(double duration, std::string starting_configuration, std::string ending_configuration);

		bool loadPrecomputedWaveforms(double moveDuration, std::string starting_configuration, std::string ending_configuration);

		void combinePrecomputedWaveform(std::vector<int> &destinations,std::vector<short> &mode, int move_ind, short* pvBuffer, bool row, int mode_len, const size_t movingWaveformSize);

		void combineRearrangeWaveform(short *movingWaveform, int worker, std::vector<int> *destinations, const size_t movingWaveformSize, std::vector<short> *mode, int move_ind, short* pvBuffer, bool row, int mode_len);


		bool sanitizeTraps(double new_gain = -1, bool shouldPrintTotalPower=true);

		void printTraps();

		std::vector<double> trapFrequencies();

		void resetForRearrangement();

		void printAvailableDefaultTrapConfigurations();

    double gain;
    double xAxisCenterFreq;
    double yAxisCenterFreq;

		std::vector<std::complex<float>> getWaveTable();

		Waveform staticStartingWaveform;
		Waveform staticEndingWaveform;

		void setAxisFile(string filename, string axis);

		string staticWaveform;

		int getWFSize();

private:


    bool majorAxisx;

  	//LineDisplay lineDisplay;
  	int lineDisplayCounter;

  	WaveTable *waveTable;
  	Waveform rearrangeWaveform;

  	struct loadedWaveformProperties lastLoadedWaveformProperties;

  	int numStartingTraps;
  	int numEndingTraps;


		Waveform loadedTrapWaveforms[MAX_NUM_TRAPS][MAX_NUM_TRAPS];
	  Waveform loadedTrapWaveformsShort[MAX_NUM_TRAPS][MAX_NUM_TRAPS];


  	std::vector<bool> periodicClusterPattern;
  	int clusterSeparation;
  	int clusterSize;
  	int atomsPerCluster;
  	int clusterPeriodicity;
  	std::vector<bool> clusterTargetIndices;
  	int numClustersToBuild;
  };

#endif
