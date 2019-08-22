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


/*
#include "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v10.1\\include\\cuda_runtime.h"
*/
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include <vector>

#define DEFAULT_WAVEFORM_DURATION 0.00001
// #define DEFAULT_WAVEFORM_DURATION 0.0000003
#define MAX_NUM_TRAPS 50

using namespace std;


static const int numDevices = 2; //This is the number of graphics cards being used.
																//The software currently supports either 1 or 2.
																//If no card is used, one needs to use rearrangedWaveforms
																//instead of rearrangeWaveformsCuda.
static const int defaultDevice = 0; //if only using a single graphics card, and
																		//multiple cards are available, it will run on the
																		//default device. If only one card is available,
																		//set the default device to 0. Also, recall that
																		//graphics cards are 0 indexed if setting the default
																		//to a value other than 0.
static const int numWorkers = 20;


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

		void combinePrecomputedWaveform(vector<int> &destinations,short* mode, int move_ind, short* pvBuffer, bool row, int mode_len,const size_t movingWaveformSize,int num_moves,int device);

		void combineRearrangeWaveform(int worker, std::vector<int> *destinations, const size_t movingWaveformSize, std::vector<short> *mode, int move_ind, short* pvBuffer, bool row, int mode_len, int bufferSize);

		void combineRearrangeWaveformCuda(vector<int> *destinations, const size_t movingWaveformSize, short* mode, short* pvBuffer, bool row, int mode_len, int num_moves, int move_index,int device);

    bool sanitizeTraps(double new_gain = -1, bool shouldPrintTotalPower=true);

		void printTraps();

		std::vector<double> trapFrequencies();

		void resetForRearrangement();

		void printAvailableDefaultTrapConfigurations();

    double gain;
    double xAxisCenterFreq;
    double yAxisCenterFreq;


		Waveform staticStartingWaveform;
		Waveform staticEndingWaveform;

		void setAxisFile(string filename, string axis);

		string staticWaveform;

		int getWFSize();
		vector<vector<short*>> loadedCudaWaveforms;
		vector<vector<short*>> loadedCudaWaveforms2;
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

		int rearrangeDataSize;
  };


	__global__ void addWaveformsCuda(short* wave1, short* wave2, int row, int col,bool addMode, size_t movingWaveformSize, int startIndex, int endIndex,short* mode,int mode_len);
#endif
