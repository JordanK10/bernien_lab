
#ifndef AWG_CONTROLLER_H
#define AWG_CONTROLLER_H

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
	SINGLE
};

using namespace std;

extern double AWG_GAIN;


class AWGController{

public:

	AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain, output_mode mode, int clock_rate);

	void disconnect();
	void startStreaming();

	bool loadDataBlock(Waveform dataArr, int channel, int64 llBytesToCalculate);

	void pushWaveform(Waveform waveform);
	void pushWaveform(Waveform *waveform);

	void pushWaveform(std::vector<Waveform> waveform);

	void pushWaveforms(vector<Waveform> waveforms);
	void pushWaveforms(vector<Waveform *> waveforms);

	bool isConnected();

	// drv_handle hDrv;


private:

    char                szBuffer[1024];     // a character buffer for any messages
  	ST_SPCM_CARDINFO    stCard;             // info structure of my card
  	void*               pvBuffer = NULL;
  	uint32              dwErr;
  	int                 nKeyCheck = 0;      // key check counter to avoid to much key polling

    // setup for the FIFO mode (HW buffer size can be programmed starting with firmware V9)
   	int64        llHWBufSize = MEGA_B(256);
   	int64        llSWBufSize = MEGA_B(128);
   	int64        llNotifySize = KILO_B(1024); // the data transfer speed to the card increases with the notify size


    double sampleRate;
    double centerFreq;
    double gain;

    bool connected = false;
    bool shouldDisconnect;


    bool hasStartedStreaming = false;

    bool hasInitialized = false;


    bool notificationHasBeenSent;

    size_t samples_per_packet = 0;

		char input;

		void setupSingleWave (ST_SPCM_CARDINFO *pstCard, int32 lReplayMode, int64 llLoops, int clockRate);
		bool setupFIFO (ST_SPCM_CARDINFO *pstCard);
		void errorPrint(bool dwErr, string error);
};

#endif
