// Created by H.Levine with Atom Array team, Lukin group (2016).


#ifndef SDR_CONTROLLER_H
#define SDR_CONTROLLER_H

// #include "uhd/usrp/multi_usrp.hpp"
#include "Waveform.h"
#include <queue>
#include <mutex>
#include <thread>

extern bool has_underflow;
extern double SDR_GAIN;

class SDRController
{
public:
	SDRController(bool shouldConnect, double sampleRate, double centerFreq, double gain=1);
	void initializeUHD();
	void disconnect();
	void startStreaming();

	void pushWaveform(Waveform waveform);
	void pushWaveform(Waveform *waveform);

	void pushWaveforms(std::vector<Waveform *> waveforms);
	void pushWaveforms(std::vector<Waveform> waveforms);

	void waitOnWaveformNotification();

	void joinSendThread();

	bool isConnected();

	bool changeGain(double newGain);
	double getGain();

	std::queue<Waveform *> waveformQueue;

private:
	double sampleRate;
	double centerFreq;
	double gain;

	void sendData();

	bool connected;
	bool shouldDisconnect;

	std::queue<Waveform> localWaveforms; // Waveforms stored locally by this object.

	std::mutex queueLock;
	bool hasStartedStreaming = false;

	bool hasInitialized = false;

	std::thread *sendThread;

	uhd::usrp::multi_usrp::sptr usrp;
	uhd::tx_streamer::sptr tx_stream;

	bool notificationHasBeenSent;

	size_t samples_per_packet = 0;
};

#endif
