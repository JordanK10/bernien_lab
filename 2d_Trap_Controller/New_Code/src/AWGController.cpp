#include "AWGController.h"
#include <fstream>

// AWG Controller constructor
AWGController::AWGController(bool shouldConnect, double sample_rate, output_mode mode, int sw_buf){
  //Set card parameters
  llSWBufSize = sw_buf;
  sampleRate = MEGA(sample_rate/(MEGA(1)));

  // Read out used bytes per sample
  // spcm_dwGetParam_i32 (stCard.hDrv, SPC_MIINST_BYTESPERSAMPLE, &lBytesPerSample);

  //Initialize card and setup card if successful
  if (bSpcMInitCardByIdx(&stCard, 0)){
	  setupCard();
    changeMode(mode);
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
    if(twoChen)
      spcm_dwSetParam_i32(stCard.hDrv, SPC_CHENABLE,CHANNEL0 | CHANNEL1 );
    else
      spcm_dwSetParam_i32(stCard.hDrv, SPC_CHENABLE,CHANNEL0 );

    spcm_dwSetParam_i32(stCard.hDrv, SPC_AMP0,2000);
    spcm_dwSetParam_i32(stCard.hDrv, SPC_AMP1,2000);

    spcm_dwSetParam_i32(stCard.hDrv, SPC_FILTER0,0);
    spcm_dwSetParam_i32(stCard.hDrv, SPC_FILTER1,0);

    spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_STARTSTEP, 0);

    connected = true;

    return true;
}

bool AWGController::changeMode(output_mode mode){
  if( mode == SINGLE){
    bSpcMSetupModeRepStdLoops  (&stCard, 1, KILO_B(64), 0);
    return spcm_dwSetParam_i32 (stCard.hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK);
  }else if (mode == FIFO){
    return bSpcMSetupModeRepFIFOSingle (&stCard, 1, 1024);
  }else if (mode == SEQUENCE){
    return  bSpcMSetupModeRepSequence(&stCard,1,2);
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

bool AWGController::loadStaticDataBlock(vector<Waveform> waveforms, int channel, int64 llBytesToCalculate){

  // Generate array of pointers to buffer memory
  int16* pnData = (int16*) pvBuffer;
  cout << "\n" << gain << "\n";

  vector<complex<float>> dataVecX = waveforms[0].dataVector;
  vector<complex<float>> dataVecY = waveforms[1].dataVector;
  cout << endl;
  ofstream myfile;
  myfile.open ("example.txt");
  if(twoChen){
    for (int64 i = 0; i <llSWBufSize/4; i++){
      pnData[i*2] = (int16)(real(dataVecX[i%dataVecX.size()])*gain);
      pnData[i*2+1] = (int16)(real(dataVecY[i%dataVecY.size()])*gain);
      myfile << pnData[i*2] << endl;
    }
  }
  else{
    for (int64 i = 0; i <llSWBufSize/2; i++){
      pnData[i] = (int16)(real(dataVecX[i%dataVecX.size()])*gain);
    }
  }
  myfile.close();

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

void AWGController::pushStaticWaveforms(vector<Waveform> waveforms) {
  // if (stCard.bSetError){
  //   return;
  // }
  // ------------------------------------------------------------------------
	// allocate and setup the fifo buffer and fill it once with data
	pvBuffer = pvAllocMemPageAligned((uint32)llSWBufSize);
	if (!pvBuffer){
		nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
    return;
  }

  // for (i = 0; i < waveforms.size(); i++){
  loadStaticDataBlock(waveforms,1,llSWBufSize);

  if (!stCard.bSetError)
      {
      // we define the buffer for transfer and start the DMA transfer
      printf ("Starting the DMA transfer and waiting until data is in board memory\n");

      spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_WRITESEGMENT, 0);
      spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_SEGMENTSIZE, llSWBufSize/lBytesPerSample);

      spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, pvBuffer, 0, llSWBufSize);
      spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA);

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

  int64 lStep = 0; // current step is Step#0
  int64 llSegment = 0; // associated with data memory segment 0
  int64 llLoop = 10; // Pattern will be repeated 10 times
  int64 llNext = 1; // Next step is Step#1
  int64 llCondition = SPCSEQ_ENDLOOPALWAYS; // Unconditionally leave current step
  // combine all the parameters to one int64 bit value
  int64 llValue = (llCondition << 32) | (llLoop << 32) | (llNext << 16) | (llSegment);
  cout << spcm_dwSetParam_i64 (stCard.hDrv, SPC_SEQMODE_STEPMEM0 + lStep, llValue) << endl ;
  // combine all the parameters to one int64 bit value
  spcm_dwSetParam_i64 (stCard.hDrv, SPC_SEQMODE_STEPMEM0 + lStep, llValue);
  lStep = 1; // current step is Step#1
  llSegment = 1; // associated with data memory segment 1
  llLoop = 1; // Pattern will be repeated once before condition is checked
  llNext = 0; // Next step is Step#0
  llCondition = SPCSEQ_ENDLOOPONTRIG; // Repeat current step until a trigger has occurred
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
  tempBuffer = pvAllocMemPageAligned((uint32)llSWBufSize);
  int16* pnDataTemp = (int16*)tempBuffer;
  int16* pnData = (int16*)pvBuffer;
  for(int i=0; i<llSWBufSize/2;i++){
    pnDataTemp[i] = pnData[i]*0.5;
  }
   spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, tempBuffer, llSWBufSize+1, llSWBufSize);
  cout << endl <<spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA)<<endl;
  //spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
  //cout << endl << spcm_dwSetParam_i64 (stCard.hDrv, SPC_LOOPS,llLoops);
  //spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER);

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
