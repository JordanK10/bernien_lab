#include "AWGController.h"

bool bDoCardSetup (ST_SPCM_CARDINFO *pstCard)
    {
    int     i;

    int lFilter = 0;

    // we try to set the samplerate to 1 MHz (M2i) or 50 MHz (M4i) on internal PLL, no clock output
    bSpcMSetupClockPLL (pstCard, MEGA(50), false);
    lFilter = 1; // the only available filter
    printf ("Sampling rate set to %.1lf MHz\n", (double) pstCard->llSetSamplerate / MEGA(1));

    // external trigger, rising edge
    bSpcMSetupTrigExternal (pstCard, SPC_TM_POS, false);


		// program all output channels to +/-1 V, zero offset and filter
		bSpcMSetupAnalogOutputChannel (pstCard, 0, 1000, 0, lFilter);
		bSpcMSetupAnalogOutputChannel (pstCard, 1, 1000, 0, lFilter);

    // FIFO multi mode setup, we run continuously
    // only one chanel is activated for analog output to keep example simple
    bSpcMSetupModeRepFIFOMulti (pstCard, 1, 1024);
		bSpcMSetupModeRepFIFOMulti (pstCard, 0, 1024);

    return pstCard->bSetError;
    }


AWGController::AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain) {
	sampleRate = sample_rate;
	centerFreq = center_freq;
	gain = tx_gain;

	// ------------------------------------------------------------------------
	// init card number 0 (the first card in the system), get some information and print it
	if (bSpcMInitCardByIdx(&stCard, 0))
	{
		printf(pszSpcMPrintDocumentationLink(&stCard, szBuffer, sizeof(szBuffer)));
		printf(pszSpcMPrintCardInfo(&stCard, szBuffer, sizeof(szBuffer)));

	}
	else{
		nSpcMErrorMessageStdOut(&stCard, "Error: Could not open card\n", true);
}


	// ------------------------------------------------------------------------
	// do the card setup, error is routed in the structure so we don't care for the return values
	if (!stCard.bSetError)
	{
		bool bError = bDoCardSetup(&stCard);
		if (bError)
		{
			printf("Error: An error occured in card setup\n");
		}
	}

	connected = true;


	// we program the hardware buffer size to reduce the latency
	spcm_dwSetParam_i64(stCard.hDrv, SPC_DATA_OUTBUFSIZE, llHWBufSize);
	spcm_dwSetParam_i32(stCard.hDrv, SPC_M2CMD, M2CMD_CARD_WRITESETUP);



}

	void AWGController::disconnect() {
		shouldDisconnect = true;

		// spcm_vClose(hDrv);

		connected = false;
	}

/*
**************************************************************************
loadDataBlock: loads the data from the computed waveform arrays into buffers
**************************************************************************
*/

	static int64 g_llOffset = 0;
	static int64 g_llXDiv = KILO_B(100);

	bool AWGController::loadDataBlock(ST_SPCM_CARDINFO *pstCard, int64 llBytesToCalculate, std::vector<int> dataArr){

		// ------------------------------------------------------------------------
		// allocate and setup the fifo buffer and fill it once with data
		pvBuffer = pvAllocMemPageAligned((uint32)llSWBufSize);
		if (!pvBuffer)
			return nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
		
		int64 i;
		int16* pnData = (int16*) pvBuffer;

		for (i = 0; i < llBytesToCalculate / 2; i++)
			pnData[i] = (int16)dataArr[i]; //PUT DATA HURRRRRRR

		g_llOffset += (llBytesToCalculate / pstCard->lBytesPerSample);

		return true;
	}

	void AWGController::pushWaveform(Waveform waveform) {
		// if (waveform.shouldNotifyAfterSending) {
		// 	notificationHasBeenSent = false;
		// }
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// localWaveforms.push(waveform);
		// waveformQueue.push(&localWaveforms.back());
		// queueLock.unlock();
	}

	void AWGController::pushWaveform(Waveform *waveform) {
		// if (waveform->shouldNotifyAfterSending) {
		// 	notificationHasBeenSent = false;
		// }
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// waveformQueue.push(waveform);
		// queueLock.unlock();
	}

	void AWGController::pushWaveform(vector<Waveform> waveform){
		return;
	}

	void AWGController::pushWaveforms(vector<Waveform> waveforms) {
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// for (int i = 0; i < waveforms.size(); i++) {
		// 	if (waveforms[i].shouldNotifyAfterSending) {
		// 		notificationHasBeenSent = false;
		// 	}
		//
		// 	localWaveforms.push(waveforms[i]);
		//
		// 	waveformQueue.push(&localWaveforms.back());
		// }
		// queueLock.unlock();
	}

	void AWGController::pushWaveforms(vector<Waveform *> waveforms) {
		// if (hasStartedStreaming) {
		// 	queueLock.lock();
		// }
		// for (int i = 0; i < waveforms.size(); i++) {
		// 	if (waveforms[i]->shouldNotifyAfterSending) {
		// 		notificationHasBeenSent = false;
		// 	}
		//
		// 	waveformQueue.push(waveforms[i]);
		// }
		// queueLock.unlock();
	}


	bool AWGController::isConnected() {
		return connected;
	}
