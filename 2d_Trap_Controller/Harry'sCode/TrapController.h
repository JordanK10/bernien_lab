// Created by H.Levine with Atom Array team, Lukin group (2016).

#ifndef TRAP_CONTROLLER_H
#define TRAP_CONTROLLER_H

#include "Trap.hpp"
#include "WaveTable.hpp"
#include "LineDisplay.h"
#include "Waveform.h"

#include <vector>

#define DEFAULT_WAVEFORM_DURATION 0.001
#define MAX_NUM_TRAPS 101

enum rearrange_mode {
	REARRANGE_MODE_STACK_FROM_LEFT,
	REARRANGE_MODE_STACK_FROM_RIGHT,
	REARRANGE_MODE_FIXED_ARRAY_WITHOUT_RESERVOIR,
	REARRANGE_MODE_FIXED_ARRAY_WITH_RESERVOIR,
	REARRANGE_MODE_FIXED_ARRAY_WITH_NEARBY_RESERVOIR,
	REARRANGE_MODE_LINE_DISPLAY,
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


class TrapController
{
public:
	TrapController(double centerFreq, double sampleRate, double gain);

	void addTrap(double frequency, double amplitude, double phase=0);
	Waveform generateWaveform(double duration = DEFAULT_WAVEFORM_DURATION);

	void printTraps();
	std::vector<double> trapFrequencies();

	bool loadDefaultTrapConfiguration(std::string filename);
	void printAvailableDefaultTrapConfigurations();

	Waveform takeConsoleInput();

	std::vector<Trap> traps;
	void saveTrapStates();
	void restoreTrapStates();
	bool sanitizeTraps(double new_gain = -1, bool shouldPrintTotalPower=true);

	std::vector<Waveform *> rearrangeTraps(std::vector<bool> atomsPresent, enum rearrange_mode mode, int modeArgument=0);
	std::vector<Waveform *> combinePrecomputedWaveforms(std::vector<int> &destinations);
	void resetForRearrangement();


	bool loadPrecomputedWaveforms(double moveDuration, std::string starting_configuration, std::string ending_configuration);

	bool mostRecentlyLoadedCorrectWaveforms(double duration, std::string starting_configuration, std::string ending_configuration);

	void testPrecomputedWaveforms();
	void testOnTheFlyComputedWaveforms();

	void prepareLineDisplay(std::string text);
	void printLineDisplay();


	double centerFrequency;
	double sdr_gain;

	int reservoirSeparation = 20;

	void calculateClusterProperties(std::vector<int> pattern, int separation);



	Waveform staticStartingWaveform;
	Waveform staticEndingWaveform;

	std::string lastLoadedConfiguration;


private:
	void combineRearrangeWaveform(std::complex<float> *movingWaveform, int worker, std::vector<int> *destinations, const size_t movingWaveformSize);


	LineDisplay lineDisplay;
	int lineDisplayCounter;


	WaveTable *waveTable;
	Waveform rearrangeWaveform;

	struct loadedWaveformProperties lastLoadedWaveformProperties;
	Waveform loadedTrapWaveforms[MAX_NUM_TRAPS][MAX_NUM_TRAPS];
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