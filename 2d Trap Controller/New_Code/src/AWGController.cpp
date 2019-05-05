#include "AWGController.h"
#include <fstream>

// AWG Controller constructor
AWGController::AWGController(bool shouldConnect, double sample_rate, output_mode mode, int sw_buf){
  //Set card parameters
  llSWBufSize = sw_buf;
  sampleRate = MEGA(sample_rate/(MEGA(1)));

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

    connected = true;

    return true;
}

bool AWGController::changeMode(output_mode mode){
  if( mode == SINGLE){
    bSpcMSetupModeRepStdLoops  (&stCard, 1, KILO_B(64), 0);
    return spcm_dwSetParam_i32 (stCard.hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK);
  }else if (mode == FIFO){
    return bSpcMSetupModeRepFIFOSingle (&stCard, 1, 1024);
  }
  else{
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

bool AWGController::loadDataBlock(vector<complex<float>> dataArr, int channel, int64 llBytesToCalculate){

  // Generate array of pointers to buffer memory
  int16* pnData = (int16*) pvBuffer;

  for (int64 i = 0; i <llSWBufSize/2; i++)
    pnData[i] = (int16)(real(dataArr[i%dataArr.size()])*gain);

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

void AWGController::pushWaveforms(vector<Waveform> waveforms) {
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
  for (int i = 0; i < 1; i++){
      loadDataBlock(waveforms[i].dataVector,1,llSWBufSize);
  }

  if (!stCard.bSetError)
      {
      // we define the buffer for transfer and start the DMA transfer
      printf ("Starting the DMA transfer and waiting until data is in board memory\n");
      spcm_dwSetParam_i32(stCard.hDrv, SPC_CHENABLE,CHANNEL0);
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
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER);
  if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szErrorText) != ERR_OK) {
    cout << (szErrorText); // print the error text
    return false;
  }
  cout << "Running...";
  return true;
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
