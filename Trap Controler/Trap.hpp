// Created by H.Levine with Atom Array team, Lukin group (2016).


#ifndef TRAP_CLASS_H
#define TRAP_CLASS_H

#include "WaveTable.hpp"
#include <iostream>
#include <complex.h>
#include <ostream>
#include <string>


class Trap {
public:
    Trap(WaveTable *, double freq, double amplit, double phase=0);
    
	friend std::ostream& operator<< (std::ostream&, Trap&);

	std::complex<float> nextSample();
    void changeToFrequency(double newFrequency, double duration);
	void changeToAmplitude(double newAmplitude, double duration);

	void saveState();
	void restoreState();

	void setPhase(double);
	double getPhase();

	double frequency;
	double amplitude;
    
private:
    long int phaseIndex;

    
    bool isChangingFrequency;
    double newFrequency;
    size_t samplesSinceStartingFrequencyTransition;
    double durationOfFrequencyTransition;

	bool isChangingAmplitude;
	double newAmplitude;
	size_t samplesSinceStartingAmplitudeTransition;
	double durationOfAmplitudeTransition;


	double savedFrequency;
	double savedAmplitude;
	long int savedPhaseIndex;
    
    WaveTable *waveTable;
};


#endif