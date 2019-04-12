/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#include "Trap.h"
#include <iomanip>

//Declaration for trap. major axis freq,amp handled by Trap/Static controller
Trap::Trap(WaveTable *wTable, double freq, double amp, double phase) {
    waveTable = wTable;

    frequency = freq;
    amplitude = amp;

    setPhase(phase);


    isChangingFrequency = false;
    isChangingAmplitude = false;

}

//Sets phase
void Trap::setPhase(double newPhase) {
	phaseIndex = newPhase * waveTable->tableLength;
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
