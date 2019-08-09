/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016)
**/

#ifndef TRAP_CLASS_H
#define TRAP_CLASS_H

#include "WaveTable.h"
#include <iostream>
#include "math.h"
#include <ostream>
#include <string>


class Trap {
    public:
        Trap(WaveTable *, double freq, double amplit, double phase=0);
        double frequency;
      	double amplitude;
        double phase;

        void setPhase(double);

        void changeToFrequency(double newFrequency, double duration);
        void changeToAmplitude(double newAmplitude, double duration);

      	void saveState();
      	void restoreState();

        short nextSample();


        double getPhase();


    private:

    //  bool orientation; //True for x, false for y
      long int phaseIndex;


      bool isChangingFrequency;
      double newFrequency;
      size_t samplesSinceStartingFrequencyTransition;
      double durationOfFrequencyTransition;

    	bool isChangingAmplitude;
    	double newAmplitude;
    	size_t samplesSinceStartingAmplitudeTransition;
    	double durationOfAmplitudeTransition;

      float fpi = 3.1415926535897;

    	double savedFrequency;
    	double savedAmplitude;
    	long int savedPhaseIndex;

      WaveTable *waveTable;
    };

#endif
