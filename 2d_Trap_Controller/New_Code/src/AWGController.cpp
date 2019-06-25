#include "AWGController.h"
#include <fstream>

#define USING_EXTERNAL_TRIGGER  0   // wait for keystroke to switch to next sequence group
//#define USING_EXTERNAL_TRIGGER  1   // use external trigger to switch to next sequence group

// AWG Controller constructor
AWGController::AWGController(bool shouldConnect, double sample_rate, output_mode mode, int sw_buf){
  //Set card parameters
  llSWBufSize = sw_buf;
  sampleRate = MEGA(sample_rate/(MEGA(1)));

  //Initialize card and setup card if successful
  if (bSpcMInitCardByIdx(&stCard, 0)){
	  setupCard();
    changeMode(SEQUENCE);
    setupSuccess = true;
  }else{
    cout << "Could not connect";
    setupSuccess = false;
  }
}

//Sets up parameters such as buffer information, clockrate, output channels, and mode
bool AWGController::setupCard(){
    //Print card info
    printf(pszSpcMPrintCardInfo(&stCard, szBuffer, sizeof(szBuffer)));

    //The hardware buffer is the length of the memory we write the wave to
    spcm_dwSetParam_i64(stCard.hDrv, SPC_DATA_OUTBUFSIZE, llHWBufSize);
    //Sets up the card for writing to memory
    spcm_dwSetParam_i32(stCard.hDrv, SPC_M2CMD, M2CMD_CARD_WRITESETUP);
    //Sets clockrate
    bSpcMSetupClockPLL (&stCard,  sampleRate, false);
    printf ("Sampling rate set to %.1lf MHz\n", (double) stCard.llSetSamplerate / MEGA(1));

    //Enables all channels
    for (int i=0; i < stCard.lMaxChannels; i++)
            bSpcMSetupAnalogOutputChannel (&stCard, i, 1000, 0, 0);

    connected = true;

    return true;
}

bool AWGController::changeMode(output_mode mode){
  if( mode == SINGLE){
    bSpcMSetupModeRepStdLoops  (&stCard, 1, KILO_B(64), 0);
    return spcm_dwSetParam_i32 (stCard.hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK);
  }else if (mode == SEQUENCE){
    return bSpcMSetupModeRepSequence(&stCard,2,2);
  }else{
    printf("Mode selection invalid\n");
    return false;
  }
}

void AWGController::disconnect() {

	shouldDisconnect = true;
	vSpcMCloseCard(&stCard);
	connected = false;
}

static int64 g_llOffset = 0;
static int64 g_llXDiv = KILO_B(100);

bool AWGController::loadStaticDataBlock(vector<Waveform> waveforms, int channel, int segSize, sin_mode wave){

  // Generate array of pointers to buffer memory
  int16* pnData = (int16*) pvBuffer;

  int scale; int seg;
  if(wave == SIN2){
    seg = 1;
    scale = 1;
  }else{
    seg = 0;
    scale = 2;
  }
  vector<complex<float>> dataVecX = waveforms[0].dataVector;
  vector<complex<float>> dataVecY = waveforms[1].dataVector;

  if(twoChen){
    cout << segSize << endl;
    for (int64 i = 0; i <segSize; i++){
      pnData[i*2] = (int16)(real(dataVecX[i%dataVecX.size()])*gain/scale);
      pnData[i*2+1] = (int16)(real(dataVecY[i%dataVecY.size()])*gain/scale);
    }
  }
  else{
    for (int64 i = 0; i <segSize/2; i++){
      pnData[i] = (int16)(real(dataVecX[i%dataVecX.size()])*gain);
    }
  }
  cout << "Halfway. The Segsize is: " << segSize << endl;
  // setup
  cout << seg << endl;
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_WRITESEGMENT, seg);

  spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_SEGMENTSIZE,  segSize);
  cout << spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer) << endl;

  // write data to board (main) sample memory
  spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, pvBuffer, 0, segSize*stCard.lBytesPerSample);
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA);

	return true;
}

int AWGController::getGain(){
  return gain;
}

bool AWGController::changeGain(int g){
  gain = g;
  //Insert check for valid variable change
  return true;
}

void vWriteStepEntry (ST_SPCM_CARDINFO *pstCard, uint32 dwStepIndex,
                      uint32 dwStepNextIndex, uint32 dwSegmentIndex, uint32 dwLoops, uint32 dwFlags)
    {
    uint32 dwErr =         0;
    uint64 qwSequenceEntry = 0;

    // setup register value
    qwSequenceEntry = (dwFlags & ~SPCSEQ_LOOPMASK) | (dwLoops & SPCSEQ_LOOPMASK);
    qwSequenceEntry <<= 32;
    qwSequenceEntry |= ((dwStepNextIndex << 16)& SPCSEQ_NEXTSTEPMASK) | (dwSegmentIndex & SPCSEQ_SEGMENTMASK);

    if (!dwErr) dwErr = spcm_dwSetParam_i64 (pstCard->hDrv, SPC_SEQMODE_STEPMEM0 + dwStepIndex, qwSequenceEntry);
  }


void AWGController::pushStaticWaveforms(vector<Waveform> waveforms) {
  // if (stCard.bSetError){
  //   return;
  // }
  // ------------------------------------------------------------------------
	// allocate and setup the fifo buffer and fill it once with data
  int dataSize = waveforms[0].dataVector.size();
	pvBuffer = pvAllocMemPageAligned(dataSize*8);
	if (!pvBuffer){
		nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
    return;
  }
  cout << "before\n";
  // for (i = 0; i < waveforms.size(); i++){
  loadStaticDataBlock(waveforms,1,dataSize*2,SIN1);
  loadStaticDataBlock(waveforms,1,dataSize*2,SIN2);

  cout << "after\n";
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_STARTSTEP, 0);
  if (!stCard.bSetError)
      {
      // we define the buffer for transfer and start the DMA transfer
      printf ("Starting the DMA transfer and waiting until data is in board memory\n");

      if(twoChen)
        spcm_dwSetParam_i32(stCard.hDrv, SPC_CHENABLE,CHANNEL0 | CHANNEL1 );
      else
        spcm_dwSetParam_i32(stCard.hDrv, SPC_CHENABLE,CHANNEL0 );

      spcm_dwSetParam_i32(stCard.hDrv, SPC_AMP0,2000);
      spcm_dwSetParam_i32(stCard.hDrv, SPC_AMP1,2000);

      spcm_dwSetParam_i32(stCard.hDrv, SPC_FILTER0,0);
      spcm_dwSetParam_i32(stCard.hDrv, SPC_FILTER1,0);

      vWriteStepEntry (&stCard,  0,  1,  SIN1,      1,  SPCSEQ_ENDLOOPALWAYS);
      vWriteStepEntry (&stCard,  1,  0,  SIN2,      1,  SPCSEQ_ENDLOOPALWAYS);

      // check for error code
      if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer)){
          vFreeMemPageAligned (pvBuffer, llSWBufSize);
          nSpcMErrorMessageStdOut (&stCard, szBuffer, false);
          printf( "...data transfer failed. Freeing memory\n");
      }else{
          spcm_dwSetParam_i32 (stCard.hDrv, SPC_TIMEOUT, 0);
          printf ("... data has been transferred to board memory\n");
      }
    }else
      cout << "Error \n";
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

bool AWGController::run(int timeout, int channel){
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER);
  if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szErrorText) != ERR_OK) {
    cout << (szErrorText); // print the error text
    return false;
  }
  cout << "Running...";
  return true;
}

//An experimental function to test fifo looping
void AWGController::fifoLoop(int llLoops){
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
  cout << endl << spcm_dwSetParam_i64 (stCard.hDrv, SPC_LOOPS,llLoops);
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER);

}

void AWGController::stop(){

  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
  if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szErrorText) != ERR_OK){
    cout << (szErrorText);
  }else
    printf("Stopping card... ");

}

void AWGController::errorPrint(bool dwErr, string error){
  if (dwErr)
    cout << endl << error  << endl << dwErr << endl;
}
