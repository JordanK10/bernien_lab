/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#ifndef TRAP_CONTROLLER_H
#define TRAP_CONTROLLER_H

#include "Trap.h"
#include "Waveform.h"

#include <vector>

#define DEFAULT_WAVEFORM_DURATION 0.001
#define MAX_NUM_TRAPS 101

using namespace std;

enum rearrange_mode {
	REARRANGE_MODE_COMPACT_BL,
	REARRANGE_MODE_COMPACT_TL,
	REARRANGE_MODE_COMPACT_BR,
	REARRANGE_MODE_COMPACT_TR,
	REARRANGE_MODE_FIXED_ARRAY_WITHOUT_RESERVOIR,
	REARRANGE_MODE_FIXED_ARRAY_WITH_RESERVOIR,
	REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR,
	REARRANGE_MODE_CLUSTERS,
	REARRANGE_MODE_OPTIMIZED_CLUSTERS,
	REARRANGE_MODE_SLOW_VIDEO
};

static const int numWorkers = 6;


struct loadedWaveformProperties {
	std::string starting_configuration;
	std::string ending_configuration;
	double duration;
};


class TrapController {

public:

    TrapController(double centerFx, double centerFy, double sampleRate, double gain, bool axis);

    void addTrap(double freq, double amp, double phase=0);

    void setCenter(double xaxis, double freq);

		bool loadDefaultTrapConfiguration(std::vector<std::vector<std::string>> tokenList, int groupSize);

    std::vector<Trap> traps;

		bool sanitizeTraps(double new_gain = -1, bool shouldPrintTotalPower=true);

		void printTraps();

		std::vector<double> trapFrequencies();

		void resetForRearrangement();

		vector<Waveform *> rearrangeTraps(std::vector<bool> atomsPresent, enum rearrange_mode mode, int modeArgument);

		void printAvailableDefaultTrapConfigurations();

    double awg_gain;
    double xAxisCenterFreq;
    double yAxisCenterFreq;

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
