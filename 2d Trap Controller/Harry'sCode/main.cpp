// Created by H.Levine with Atom Array team, Lukin group (2016).

#include <iostream>
#include "SDRController.h"
#include "TrapController.h"
#include "control_interface.h"
#include "Console.h"
#include <fstream>
#include <limits>


using namespace std;

static bool const CONNECT_TO_CAMERA_SERVER = false;
static bool connectToSDR = false;



void recordMetadata(int seqNumber, int numRepetitions, double *durations, bool *underflowRecords) {
	string filename = "C:\\Users\\Control\\Desktop\\Data\\seq_" + to_string(seqNumber) + "_records.txt";
	fstream f(filename, ios::out);

	for (int i = 0; i < numRepetitions; i++) {
		f << i << " ";
		if (!underflowRecords[i]) {
			f << "success ";
		}
		else {
			f << "underflow ";
		}
		f << durations[i] << endl;
	}
}

void test_handler(int s) {
	cout << "Caught signal!" << endl;
}


int main() {
	// Try to set up CPUs to make best use possible.

	/*
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);

	// Use cores 8, 9, 10, 11, 12, 13, 14 for main program.
	// Use core 15 for sending data (see SDRController)
	for (int core = 0; core < 15; core++) {
		CPU_SET(core, &cpuset);
	}
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
	*/


	//initializeAdwinConnection(); // Adwin sends trigger to start rearrangement.


	double sampleRate = 100E6;
	double centerFreq_MHz = 74;
	double centerFrequency;

	cout << "Enter center frequency (in MHz): ";
	cin >> centerFreq_MHz;

	centerFrequency = centerFreq_MHz * 1.0E6;

	double gain = 1;

	SDRController sdrController(connectToSDR, sampleRate, centerFrequency, gain);

	TrapController trapControllerX(centerFrequency, sampleRate, gain);
	TrapController trapControllerY(centerFrequency, sampleRate, gain);



	// We can here load preset trap configurations if desired. These need
	// to be generated separately.

	// Examples:
	// trapController.loadDefaultTrapConfiguration("seventy_2016_03_31_optimized_3.txt");
	// trapController.loadDefaultTrapConfiguration("100(0.49)_12.txt");


	runConsole(trapControllerX, trapControllerY, sdrController);



	return 0;
}
