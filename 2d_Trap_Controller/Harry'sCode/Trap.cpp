// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "Trap.hpp"
#include <iomanip>

Trap::Trap(WaveTable *wTable, double freq, double amp, double phase) {
    frequency = freq;
    amplitude = amp;

    waveTable = wTable;

	setPhase(phase);

    isChangingFrequency = false;
	isChangingAmplitude = false;
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

void Trap::changeToFrequency(double newFreq, double duration) {
    samplesSinceStartingFrequencyTransition = 0;
    isChangingFrequency = true;
    newFrequency = newFreq;
    durationOfFrequencyTransition = duration;
}

void Trap::changeToAmplitude(double newAmp, double duration) {
	samplesSinceStartingAmplitudeTransition = 0;
	isChangingAmplitude = true;
	newAmplitude = newAmp;
	durationOfAmplitudeTransition = duration;
}

void Trap::saveState() {
	savedFrequency = frequency;
	savedAmplitude = amplitude;
	savedPhaseIndex = phaseIndex;
}

void Trap::restoreState() {
	frequency = savedFrequency;
	amplitude = savedAmplitude;
	phaseIndex = savedPhaseIndex;
}

void Trap::setPhase(double newPhase) {
	phaseIndex = newPhase * waveTable->tableLength;
}

double Trap::getPhase() {
	return ((double)phaseIndex) / waveTable->tableLength;
}


std::ostream& operator<<(std::ostream &strm, Trap &trap) {
	return strm << "f = " << std::fixed << std::setprecision(2) << (trap.frequency / 1.0E6) << " MHz, amp = " << std::setprecision(4) << trap.amplitude << ", phase = " << std::setprecision(3) << trap.getPhase();
}
