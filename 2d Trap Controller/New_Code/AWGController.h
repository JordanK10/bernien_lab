
#ifndef AWG_CONTROLLER_H
#define AWG_CONTROLLER_H

#include "Waveform.h"
#include <queue>
#include <mutex>
#include <thread>
#include <iostream>

// ----- include of common example librarys -----
//#include "../common/spcm_lib_card.h"
#include "spcm_lib_card.h"

#include "../common/spcm_lib_data.h"
#include "../common/ostools/spcm_oswrap.h"
#include "../common/ostools/spcm_ostools.h"
#include "../sb5_file/sb5_file.h"


using namespace std;

extern double AWG_GAIN;


class AWGController{

public:

	AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain);

	void disconnect();
	void startStreaming();

	bool loadDataBlock(Waveform dataArr, int channel);

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
   	int64        llHWBufSize = MEGA_B(64);
   	int64        llSWBufSize = MEGA_B(16);
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

};

#endif
