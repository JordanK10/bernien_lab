
#ifndef AWG_CONTROLLER_H
#define AWG_CONTROLLER_H

#define MAX_SEGMENTS 2
#define USING_EXTERNAL_TRIGGER  0

#include "Waveform.h"
#include <queue>
#include <mutex>
#include <thread>
#include <iostream>

// ----- include of common example librarys -----
// ----- include of common example libraries -----
#include "../../common/spcm_lib_card.h"
#include "../../common/spcm_lib_data.h"
#include "../../common/ostools/spcm_oswrap.h"
#include "../../common/ostools/spcm_ostools.h"
#include "../../sb5_file/sb5_file.h"

enum output_mode{
	FIFO,
	SINGLE,
	SEQUENCE
};

using namespace std;

extern double AWG_GAIN;


class AWGController{

public:

	AWGController(bool shouldConnect, double sample_rate, output_mode mode, int sw_buf);

	void disconnect();
	void startStreaming();

	bool loadStaticDataBlock(vector<Waveform> waveforms, int channel, int64 llBytesToCalculate);

	void pushWaveform(Waveform waveform);
	void pushWaveform(Waveform *waveform);

	void pushWaveform(std::vector<Waveform> waveform);

	void pushWaveTable(std::vector<std::complex<float>> waveform);

	bool changeMode(output_mode mode);

	int getGain();
	bool changeGain(int gain);

	void pushStaticWaveforms(vector<Waveform> waveforms);
	void pushWaveforms(vector<Waveform *> waveforms);

	bool isConnected();

	bool run(int timeout,int channel);
	void fifoLoop(int loops);
	void stop();

	// drv_handle hDrv;


private:

    char                szBuffer[1024];     // a character buffer for any messages
  	ST_SPCM_CARDINFO    stCard;             // info structure of my card
  	void*              pvBuffer = NULL;
		void* 						 tempBuffer = NULL;
  	uint32              dwErr;
  	int                 nKeyCheck = 0;      // key check counter to avoid to much key polling

    // setup for the FIFO mode (HW buffer size can be programmed starting with firmware V9)
		int64        llHWBufSize = MEGA_B(4);
   	int64        llSWBufSize;
   	int64        llNotifySize = KILO_B(2); // the data transfer speed to the card increases with the notify size
		int32 			 lBytesPerSample;

		bool twoChen = true;

    double sampleRate;

    int gain = 32761;

    bool connected = false;
    bool shouldDisconnect;

		char szErrorText[500];

    bool hasStartedStreaming = false;

    bool hasInitialized = false;


    bool notificationHasBeenSent;

    size_t samples_per_packet = 0;

		char input;

		bool setupSuccess;

		bool setupCard();
		void errorPrint(bool dwErr, string error);
};

#endif
