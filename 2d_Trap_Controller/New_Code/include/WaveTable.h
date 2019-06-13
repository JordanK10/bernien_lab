// Created by H.Levine with Atom Array team, Lukin group (2016).
// Adapted by J.Kemp with Atom Array team, Bernien Group (2019).


#ifndef WAVE_TABLE_H
#define WAVE_TABLE_H

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <complex>

class WaveTable {
public:
    WaveTable(long int, long int);
    ~WaveTable();

    double sampleRate = 0;
    double timeStep = 0;

	  std::vector<std::complex<float>> waveTable;
    size_t tableLength = 0;
    double tableFrequency = 0;

    double adiabaticTransition(double initFreq, double newFreq, double fractionalElapsedTime);

private:
    size_t transitionTableLength;
    double *adiabaticTransitionTable;
};

#endif
