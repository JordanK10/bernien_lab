#include "AWGController.h"
#include <fstream>

AWGController::AWGController(bool shouldConnect, double sample_rate, double center_freq, double tx_gain, output_mode mode, int clock_rate)
{
  sampleRate = sample_rate;
	centerFreq = center_freq;
	gain = tx_gain;

	if (bSpcMInitCardByIdx(&stCard, 0))
	{
		printf(pszSpcMPrintDocumentationLink(&stCard, szBuffer, sizeof(szBuffer)));
		printf(pszSpcMPrintCardInfo(&stCard, szBuffer, sizeof(szBuffer)));

    // // we program the hardware buffer size to reduce the latency
  	spcm_dwSetParam_i64(stCard.hDrv, SPC_DATA_OUTBUFSIZE, llHWBufSize);
  	spcm_dwSetParam_i32(stCard.hDrv, SPC_M2CMD, M2CMD_CARD_WRITESETUP);

    if(mode == SINGLE)
      setupSingleWave (&stCard, SPC_REP_STD_SINGLE, 0, clock_rate/*forever*/);
    else if(mode == FIFO)
      setupFIFO(&stCard);
    connected = true;
	}else{
		nSpcMErrorMessageStdOut(&stCard, "Error: Could not open card\n", true);
  }
}

void AWGController::disconnect() {
	shouldDisconnect = true;

	vSpcMCloseCard(&stCard);

	connected = false;
}

static int64 g_llOffset = 0;
static int64 g_llXDiv = KILO_B(100);

bool AWGController::loadDataBlock(Waveform dataArr, int channel, int64 llBytesToCalculate){

  int OUTPUT_GAIN = 800000;
  cout << "hello \n";

  // Generate array of pointers to buffer memory
  int16* pnData = (int16*) pvBuffer;

  // Populate buffer data
  int mult = (llSWBufSize/2)/dataArr.dataVector.size();
  int64 k;
  for (int64 i = 0; i < dataArr.dataVector.size()*mult-1; i++){

    k = i%dataArr.dataVector.size();
    pnData[i] = (int16)(real(dataArr.dataVector[k])*OUTPUT_GAIN);

  }
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
          spcm_dwSetParam_i32 (stCard.hDrv, SPC_TIMEOUT, 0);

          printf ("... data has been transferred to board memory\n");
          cout << "\nStarting the card and waiting for ready interrupt\n(continuous and single restart will have timeout)\n";
          spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER);
          cout << "Type \"s\" to timeout\n";
          while (cin >> input){
            if(input == 's'){
              spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
              break;
            }
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


void AWGController::errorPrint(bool dwErr, string error){
  if (dwErr)
    cout << endl << error  << endl << dwErr << endl;
}

// Sets up the card for outputting a single wave at a time
void AWGController::setupSingleWave (ST_SPCM_CARDINFO *pstCard, int32 lReplayMode, int64 llLoops, int clockRate)
  {
    int64 llChannelMask;

    llChannelMask = ((int64) 1 << pstCard->lMaxChannels) - 1;

    //Set clockrate
    bSpcMSetupClockPLL (pstCard, MEGA(clockRate), false);
    printf ("Sampling rate set to %.1lf MHz\n", (double) pstCard->llSetSamplerate / 1000000);

    //Continuous output
    bSpcMSetupModeRepStdLoops  (pstCard, llChannelMask, KILO_B(64), llLoops);
    bSpcMSetupTrigSoftware (pstCard, true);

    // all newer replay cards support multi-purpose lines
    spcm_dwSetParam_i32 (pstCard->hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK); // output continuous marker

    // program all output channels to +/- 1 V with no offset
    for (int i=0; i < pstCard->lMaxChannels; i++)
        bSpcMSetupAnalogOutputChannel (pstCard, i, 1000, 0, 0);

  }

bool AWGController::setupFIFO (ST_SPCM_CARDINFO *pstCard)
    {
    int lFilter = 1;

    // we try to set the samplerate to 1 MHz (M2i) or 50 MHz (M4i) on internal PLL, no clock output
    bSpcMSetupClockPLL (pstCard, MEGA(500), false);

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
