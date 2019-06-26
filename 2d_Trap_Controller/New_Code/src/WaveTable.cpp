// Created by H.Levine with Atom Array team, Lukin group (2016).
// Adapted by J.Kemp with Atom Array team, Bernien Group (2019).


#include "WaveTable.h"
#include <complex.h>

#include <iostream>

using namespace std;

WaveTable::WaveTable(long int sampRate, long int tableFreq) {
    // std::cout << "Initializing wave table..." << std::endl;
    sampleRate = sampRate;
    timeStep = 1.0 / sampRate;
    tableLength = sampRate / tableFreq;
    tableFrequency = tableFreq;

	// Wave Table
	waveTable.resize(tableLength);


	std::complex<float> imag(0, 1.0);
	double coeff = 2 * 3.1415926535 / tableLength;
    for (int i = 0; i < tableLength; i++) {
        waveTable[i] = std::exp(imag * (float)(coeff * i));
    }


	// Transition Table
    transitionTableLength = 10000;
    adiabaticTransitionTable = new double[transitionTableLength];
    for (int i = 0; i < transitionTableLength; i++) {
        double fractionalElapsedTime = ((double)i) / transitionTableLength;

        double fractionalChange;

        if (fractionalElapsedTime <= 0.5) {
            fractionalChange = 2 * fractionalElapsedTime * fractionalElapsedTime;
        } else if (fractionalElapsedTime < 1.0) {
            double remainingTime = 1 - fractionalElapsedTime;
            fractionalChange = 1 - 2 * remainingTime * remainingTime;
        }

        adiabaticTransitionTable[i] = fractionalChange;
    }

	// std::cout << "Done initializing wave table." << std::endl << std::endl;
}

WaveTable::~WaveTable() {
    delete[] adiabaticTransitionTable;
}

//Helper method:
double WaveTable::adiabaticTransition(double initFreq, double newFreq, double fractionalElapsedTime) {
    double fractionalChange = adiabaticTransitionTable[(int)(fractionalElapsedTime * transitionTableLength)];

	return initFreq + fractionalChange * (newFreq - initFreq);
}
