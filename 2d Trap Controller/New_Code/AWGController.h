

#ifndef AWG_CONTROLLER_H
#define AWG_CONTROLLER_H

#include "Waveform.h"
#include <queue>
#include <mutex>
#include <thread>

class AWGController{

public:

  AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain);

  void disconnect();
	void startStreaming();


	void pushWaveform(Waveform waveform);
	void pushWaveform(Waveform *waveform);

  bool isConnected();


private:

    double sampleRate;
    double centerFreq;
    double gain;

    bool connected;
    bool shouldDisconnect;


    bool hasStartedStreaming = false;

    bool hasInitialized = false;


    bool notificationHasBeenSent;

    size_t samples_per_packet = 0;

};

#endif
