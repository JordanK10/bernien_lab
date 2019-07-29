
#ifndef AWG_CONTROLLER_H
#define AWG_CONTROLLER_H

#define MAX_SEGMENTS 2

#include "Waveform.h"
#include "TrapControllerHandler.h"
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

enum signal_type{
	STATIC,
	TRANS,
	TRANS_EMPTY,
};

using namespace std;

extern double AWG_GAIN;

class AWGController{

public:

	AWGController(double sample_rate, output_mode mode, int sw_buf);

	void disconnect();
	void startStreaming();

	bool loadDataBlock(int segSize, signal_type data, vector<Waveform>* waveforms, vector<RearrangementMove>* moves);

	void pushWaveTable(std::vector<std::complex<float>> waveform);

	bool changeMode(output_mode mode);



	void pushStaticWaveforms(vector<Waveform> waveforms, bool first_push);
	void pushRearrangeWaveforms(int num_moves, int move_size);
	void triggerSequence();

	void allocateDynamicWFBuffer(float duration, int x_dim, int y_dim);
	short* getDynamicBuffer();

	bool isConnected();

	bool run(int timeout,int channel);
	void stop();

private:

    char                szBuffer[1024];     // a character buffer for any messages
  	ST_SPCM_CARDINFO    stCard;             // info structure of my card
  	short*              pvBuffer = NULL;
		short*							pvBufferDynamic = NULL;
  	uint32              dwErr;
  	int                 nKeyCheck = 0;      // key check counter to avoid to much key polling

    // setup for the FIFO mode (HW buffer size can be programmed starting with firmware V9)
		int64        llHWBufSize = MEGA_B(4);
   	int64        llSWBufSize;
   	int64        llNotifySize = KILO_B(2); // the data transfer speed to the card increases with the notify size
		int32 			 lBytesPerSample;

		bool twoChen = true;

    double sample_rate;

    float gain;

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
		void vWriteStepEntry (ST_SPCM_CARDINFO *pstCard, uint32 dwStepIndex,
		                      uint32 dwStepNextIndex, uint32 dwSegmentIndex, uint32 dwLoops, uint32 dwFlags);

};

#endif
