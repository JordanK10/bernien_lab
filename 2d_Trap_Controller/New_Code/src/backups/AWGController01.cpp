#include "AWGController.h"
#include <fstream>

int OUTPUT_GAIN = 400000;

vector<int> interpolate(Waveform dataArr, int grain){
  // vector<int> new_vec;
  // for(int i=0; i<dataArr.dataVector.size()-1; i++){
  //   double diff = real(dataArr.dataVector[i+1])-real(dataArr.dataVector[i]);
  //   for(int j=0; j<grain; j++){
  //     new_vec.push_back(OUTPUT_GAIN*real(dataArr.dataVector[i])+double(j)*diff);
  //   }
  // }
  // new_vec.push_back(real(dataArr.dataVector[-1]));
  //
  // return new_vec;
  //

  vector<int> new_vec;
  for(int i=0; i<dataArr.dataVector.size()-1; i++){
    new_vec.push_back(OUTPUT_GAIN*real(dataArr.dataVector[i]));
  }
  return new_vec;

}

void errorPrint(bool dwErr, string error){
  if (dwErr)
    cout << endl << error  << endl << dwErr << endl;
}

void vDoCardSetup (ST_SPCM_CARDINFO *pstCard, int32 lReplayMode, int64 llLoops = 0)
    {
    int i;
    int64 llChannelMask;


    // set mask for maximal channels
    if (pstCard->lMaxChannels >= 64)
        llChannelMask = -1; // -1 is all bits set to 1 = 0xffffffffffffffff
    else
        llChannelMask = ((int64) 1 << pstCard->lMaxChannels) - 1;


    // we try to set the samplerate to 1 MHz (M2i) or 50 MHz (M4i) on internal PLL, no clock output
    bSpcMSetupClockPLL (pstCard, MEGA(50), false);

    printf ("Sampling rate set to %.1lf MHz\n", (double) pstCard->llSetSamplerate / 1000000);


    // setup the replay mode and the trigger
    switch (lReplayMode)
        {

        // with loops == 1: singleshot replay with software trigger
        // with loops == 0: endless continuous mode with software trigger
        case SPC_REP_STD_SINGLE:
            bSpcMSetupModeRepStdLoops  (pstCard, llChannelMask, KILO_B(64), llLoops);
            bSpcMSetupTrigSoftware (pstCard, true);

            // on M2i starting with build 1604 we can use the trigger output as a marker for each loop start
            // be sure to have the trigger output enabled for this
            if (pstCard->bM2i)
                {
                if ((pstCard->lLibVersion & 0xFFFF) >= 1604)
                    spcm_dwSetParam_i32 (pstCard->hDrv, SPC_CONTOUTMARK, 1);
                }
            else
                {
                // all newer replay cards support multi-purpose lines
                spcm_dwSetParam_i32 (pstCard->hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK); // output continuous marker
                }
            break;

        // single restart (one signal on every trigger edge) with ext trigger positive edge
        case SPC_REP_STD_SINGLERESTART:
            bSpcMSetupModeRepStdSingleRestart (pstCard, llChannelMask, KILO_B(64), 0);
            bSpcMSetupTrigExternal (pstCard, SPC_TM_POS, false, 0);
            break;
        }

          // program all output channels to +/- 1 V with no offset
          for (i=0; i < pstCard->lMaxChannels; i++)
              bSpcMSetupAnalogOutputChannel (pstCard, i, 1000, 0, 0);
    }

bool bDoCardSetup (ST_SPCM_CARDINFO *pstCard)
    {
    int lFilter = 1;

    // we try to set the samplerate to 1 MHz (M2i) or 50 MHz (M4i) on internal PLL, no clock output
    bSpcMSetupClockPLL (pstCard, MEGA(50), false);

    printf ("Sampling rate set to %.1lf MHz\n", (double) pstCard->llSetSamplerate / MEGA(1));

    // external trigger, rising edge
    bSpcMSetupTrigExternal (pstCard, SPC_TM_POS, false);

		// program all output channels to +/-1 V, zero offset and filter
		bSpcMSetupAnalogOutputChannel (pstCard, 0, 1000, 0, lFilter);
		bSpcMSetupAnalogOutputChannel (pstCard, 1, 1000, 0, lFilter);

    cout << "  Channels: " << pstCard->lMaxChannels << "\n";

    // FIFO multi mode setup, we run continuously
    // only one chanel is activated for analog output to keep example simple
    bSpcMSetupModeRepFIFOMulti (pstCard, 1, 1024);
		bSpcMSetupModeRepFIFOMulti (pstCard, 2, 1024);

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
     vDoCardSetup (&stCard, SPC_REP_STD_SINGLE, 0/*forever*/);
		//if (bDoCardSetup(&stCard))
		//	printf("Error: An error occured in card setup\n");
	}

	connected = true;


	// we program the hardware buffer size to reduce the latency
	spcm_dwSetParam_i64(stCard.hDrv, SPC_DATA_OUTBUFSIZE, llHWBufSize);
	spcm_dwSetParam_i32(stCard.hDrv, SPC_M2CMD, M2CMD_CARD_WRITESETUP);

}

void AWGController::disconnect() {
	shouldDisconnect = true;

	vSpcMCloseCard(&stCard);

	connected = false;
}

/*
**************************************************************************
loadDataBlock: loads the data from the computed waveform arrays into buffers
**************************************************************************
*/

static int64 g_llOffset = 0;
static int64 g_llXDiv = KILO_B(100);

bool AWGController::loadDataBlock(Waveform dataArr, int channel, int64 llBytesToCalculate){

  // Generate array of pointers to buffer memory
  int16* pnData = (int16*) pvBuffer;

  // Populate buffer data
  int64 k;
  // vector<int> dVec = interpolate(dataArr,1);
  // cout << dVec.size() << " " << llSWBufSize/2 << "\n";
	for (int64 i = 0; i < llSWBufSize/2; i++){
    // k = i%dVec.size();
    // pnData[i] = (int16)(dVec[k]);
    k = i%dataArr.dataVector.size();
    pnData[i] = (int16)(real(dataArr.dataVector[k])*OUTPUT_GAIN);

  }


  // bSpcMCalcSignal (&stCard, pvBuffer, (uint32) stCard.llSetMemsize, 0, eSine);
	return true;
}

void AWGController::pushWaveforms(vector<Waveform> waveforms) {

  if (stCard.bSetError){
    cout << "Crikey";
    return;
  }
  // ------------------------------------------------------------------------
	// allocate and setup the fifo buffer and fill it once with data
	pvBuffer = pvAllocMemPageAligned((uint32)llSWBufSize);
	if (!pvBuffer){
    cout << "fail";
		nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
    return;
  }

  // for (int i = 0; i < waveforms.size(); i++){
  for (int i = 0; i < 1; i++){
      loadDataBlock(waveforms[i],1,llSWBufSize);
  }

  if (!stCard.bSetError)
      {
      // we define the buffer for transfer and start the DMA transfer
      printf ("Starting the DMA transfer and waiting until data is in board memory\n");
      spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, pvBuffer, 0, llSWBufSize);
      spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA);

      // check for error code
      if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer))
          {
          vFreeMemPageAligned (pvBuffer, llSWBufSize);
          nSpcMErrorMessageStdOut (&stCard, szBuffer, false);
          }
      else{
          spcm_dwSetParam_i32 (stCard.hDrv, SPC_TIMEOUT, 5000000000);

          printf ("... data has been transferred to board memory\n");
          cout << "\nStarting the card and waiting for ready interrupt\n(continuous and single restart will have timeout)\n";
          if (spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER | M2CMD_CARD_WAITREADY) == ERR_TIMEOUT)
              {
              spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
              cout << "timeout\n";
              }
          }
      }
    else
      cout << "Error \n";
    cout << "End\n";
    vFreeMemPageAligned (pvBuffer, (int32)llSWBufSize);

    return;

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


void AWGController::pushWaveforms(vector<Waveform *> waveform) {
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



bool AWGController::isConnected() {
	return connected;
}
