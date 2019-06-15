/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#include "Trap.h"
#include <iomanip>
#include <iostream>
using namespace std;
//Declaration for trap. major axis freq,amp handled by Trap/Static controller
Trap::Trap(WaveTable *wTable, double freq, double amp, double phas) {
    waveTable = wTable;

    frequency = freq;
    amplitude = amp;
    phase = phas;
    setPhase(phas);


    isChangingFrequency = false;
    isChangingAmplitude = false;

}

//Sets phase
void Trap::setPhase(double newPhase) {
	phaseIndex = newPhase * waveTable->tableLength;
}

std::complex<float> Trap::nextSample() {
	double amplitudeForSample = amplitude;
	if (isChangingAmplitude) {
		samplesSinceStartingAmplitudeTransition += 1;

		double fractionalElapsedTime = (double)(samplesSinceStartingAmplitudeTransition) / (waveTable->sampleRate * durationOfAmplitudeTransition);

		if (fractionalElapsedTime >= 1.0) {
			fractionalElapsedTime = 1.0;
			isChangingAmplitude = false;
			amplitude = newAmplitude;
		}

		amplitudeForSample = waveTable->adiabaticTransition(amplitude, newAmplitude, fractionalElapsedTime);
	}
  std::complex<float> sample = (float)amplitudeForSample * waveTable->waveTable[(size_t)phaseIndex];

  if (isChangingFrequency) {
	samplesSinceStartingFrequencyTransition += 1;

      double fractionalElapsedTime = (double)(samplesSinceStartingFrequencyTransition) / (waveTable->sampleRate * durationOfFrequencyTransition);

      if (fractionalElapsedTime >= 1.0) {
          fractionalElapsedTime = 1.0;
          isChangingFrequency = false;
          frequency = newFrequency;
      }

      double intermediateFrequency = waveTable->adiabaticTransition(frequency, newFrequency, fractionalElapsedTime);
	phaseIndex += (long int)(intermediateFrequency / waveTable->tableFrequency);
  } else {
      phaseIndex += (long int)(frequency / waveTable->tableFrequency);
  }

  if (phaseIndex >= (long int)(waveTable->tableLength)) {
      phaseIndex -= waveTable->tableLength;
  } else if (phaseIndex < 0) {
	phaseIndex += waveTable->tableLength;
  }
  return sample;
}

//Gets phase
double Trap::getPhase() {
	return ((double)phaseIndex) / waveTable->tableLength;
}

// Changes trap frequency to new over given duration
void Trap::changeToFrequency(double newFreq, double duration) {
    samplesSinceStartingFrequencyTransition = 0;
    isChangingFrequency = true;
    newFrequency = newFreq;
    durationOfFrequencyTransition = duration;
}

// Changes trap amplitude to new over given duration
void Trap::changeToAmplitude(double newAmp, double duration) {
	samplesSinceStartingAmplitudeTransition = 0;
	isChangingAmplitude = true;
	newAmplitude = newAmp;
	durationOfAmplitudeTransition = duration;
}

// Save information of a trap state
void Trap::saveState() {
	savedFrequency = frequency;
	savedAmplitude = amplitude;
	savedPhaseIndex = phaseIndex;
}

//Restores state from memory
void Trap::restoreState() {
	frequency = savedFrequency;
	amplitude = savedAmplitude;
	phaseIndex = savedPhaseIndex;
}
