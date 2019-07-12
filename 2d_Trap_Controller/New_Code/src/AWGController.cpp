#include "AWGController.h"
#include <fstream>

#define USING_EXTERNAL_TRIGGER  0   // wait for keystroke to switch to next sequence group
//#define USING_EXTERNAL_TRIGGER  1   // use external trigger to switch to next sequence group

static int BYTES_PER_DATA = 8;
static int BIG_NUMBER = pow(2,31)-1;
static int SMALL_NUMBER = 1.024*pow(10,7);

// AWG Controller constructor
AWGController::AWGController(double sample_rate, output_mode mode, int sw_buf){
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

void AWGController::setModes(vector<Waveform> modes, bool x){

    if(x)
      xmodes = modes;
    if(!x)
      ymodes = modes;
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

    spcm_dwSetParam_i32(stCard.hDrv, SPC_AMP0,2000);
    spcm_dwSetParam_i32(stCard.hDrv, SPC_AMP1,2000);

    spcm_dwSetParam_i32(stCard.hDrv, SPC_FILTER0,0);
    spcm_dwSetParam_i32(stCard.hDrv, SPC_FILTER1,0);

    spcm_dwSetParam_i32 (stCard.hDrv, SPC_TRIG_ORMASK, SPC_TMASK_NONE);


    connected = true;

    return true;
}

bool AWGController::changeMode(output_mode mode){
  if( mode == SINGLE){
    bSpcMSetupModeRepStdLoops  (&stCard, 1, KILO_B(64), 0);
    return spcm_dwSetParam_i32 (stCard.hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK);
  }else if (mode == SEQUENCE){
    bSpcMSetupModeRepSequence(&stCard,2,3);
    return true;
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

bool AWGController::loadDataBlock(int segSize, signal_type data, vector<Waveform>* waveforms, vector<RearrangementMove>* moves){

  // Generate array of pointers to buffer memory
  int16* pnData = (int16*) pvBuffer;

  int seg;
  int show;
  if(data == STATIC){
    seg = 0;
    show = 1;
  }else if(data == TRANS_EMPTY || data == TRANS){
    seg = 1;
    show = 1;
  }

  if(waveforms != NULL){
    vector<complex<float>> dataVecX = (*waveforms)[0].dataVector;
    vector<complex<float>> dataVecY = (*waveforms)[1].dataVector;

    for (int64 i = 0; i <segSize; i++){
      pnData[i*2] = (int16)(real(dataVecX[i%dataVecX.size()])*gain*show);
      pnData[i*2+1] = (int16)(real(dataVecY[i%dataVecY.size()])*gain*show);
    }
  }
  if(moves != NULL){
    vector<complex<float>> dataVec;
    int move_len = (*moves)[0].wf->dataVector.size();
    vector<complex<float>> mode;
    int move_num = moves->size();
    int mode_len;
    int64 seg_ind = 0;
    int64 mem_ind = segSize;
    cout << "here1\n";
    for(int64 move=0; move<move_num;move++){
      cout << "here " << move << endl;
      cout << mem_ind << "  " << seg_ind << endl;
      dataVec = (*moves)[move].wf->dataVector;
      if((*moves)[move].row){
        cout << mem_ind << endl;
        mode = xmodes[(*moves)[move].dim].dataVector;
        mode_len = mode.size();
        for(;seg_ind<mem_ind;seg_ind++){
          pnData[seg_ind*2] = (int16)(real(dataVec[seg_ind%move_len])*gain/5);
          pnData[seg_ind*2+1] = (int16)(real(mode[seg_ind%mode_len])*gain);
        }
      }else{
        mode = ymodes[(*moves)[move].dim].dataVector;
        mode_len = mode.size();
        for(;seg_ind<mem_ind;seg_ind++){
          pnData[seg_ind*2+1] = (int16)(real(dataVec[seg_ind%move_len])*gain/5);
          pnData[seg_ind*2] = (int16)(real(mode[seg_ind%mode_len])*gain);

        }
      }
      mem_ind += move_len;
    }
    segSize = seg_ind;
  }

    // write data to board (main) sample memory
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_WRITESEGMENT, seg);
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_SEGMENTSIZE,  segSize);
  spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, pvBuffer, 0, segSize*stCard.lBytesPerSample*2);
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA);

	return true;
}

float AWGController::getGain(){
  return float(gain/base_gain);
}

bool AWGController::changeGain(float g){
  gain = g*base_gain;
  //Insert check for valid variable change
  return true;
}

void AWGController::vWriteStepEntry (ST_SPCM_CARDINFO *pstCard, uint32 dwStepIndex,
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

void AWGController::pushStaticWaveforms(vector<Waveform> waveforms, bool first_push) {

  int dataSize = waveforms[0].dataVector.size()*2;
	pvBuffer = pvAllocMemPageAligned(dataSize*BYTES_PER_DATA);
	if (!pvBuffer){
		nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
    return;
  }

  if (!stCard.bSetError)
      {
      spcm_dwSetParam_i32(stCard.hDrv, SPC_CHENABLE,CHANNEL0 | CHANNEL1 );
      if(first_push){
        loadDataBlock(dataSize,STATIC,&waveforms,NULL);
        loadDataBlock(dataSize,TRANS_EMPTY,&waveforms,NULL);
        vWriteStepEntry (&stCard,  0,  0,  STATIC, 1,  SPCSEQ_ENDLOOPONTRIG);
        vWriteStepEntry (&stCard,  1,  0,  TRANS,  1,  SPCSEQ_ENDLOOPALWAYS);
      }else
        loadDataBlock(dataSize,TRANS,&waveforms,NULL);
        vWriteStepEntry (&stCard,  0,  1,  STATIC, 1,  SPCSEQ_ENDLOOPALWAYS);

      spcm_dwSetParam_i32 (stCard.hDrv, SPC_SEQMODE_STARTSTEP, 0);

      // check for error code
      if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer)){
          vFreeMemPageAligned (pvBuffer, llSWBufSize);
          nSpcMErrorMessageStdOut (&stCard, szBuffer, false);
          printf( "Data transfer failed. Freeing memory\n");
      }
    }else
      cout << "Error detected in card... Abortinig data transfer \n";
  vFreeMemPageAligned (pvBuffer, (int32)llSWBufSize);

  return;

}

void AWGController::pushRearrangeWaveforms(vector<RearrangementMove> moves){

  int num_moves = moves.size();
  int dataSize = moves[0].wf->dataVector.size();
  pvBuffer = pvAllocMemPageAligned(num_moves*2*dataSize*BYTES_PER_DATA);
  cout << dataSize << "  " << num_moves << "  " << num_moves*dataSize*2 << endl;
  if (!pvBuffer){
    nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
    return;
  }

  if (!stCard.bSetError){
      loadDataBlock(dataSize,STATIC,NULL,&moves);
      vWriteStepEntry (&stCard,  0,  1,  STATIC, 1,  SPCSEQ_ENDLOOPONTRIG);
      vWriteStepEntry (&stCard,  1,  0,  TRANS,  1,  SPCSEQ_ENDLOOPONTRIG);

      // check for error code
      if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer)){
          vFreeMemPageAligned (pvBuffer, llSWBufSize);
          nSpcMErrorMessageStdOut (&stCard, szBuffer, false);
          printf( "Data transfer failed. Freeing memory\n");
      }
  }else
    cout << "Error detected in card... Abortinig data transfer \n";
  vFreeMemPageAligned (pvBuffer, (int32)llSWBufSize);
}

void AWGController::triggerSequence(){
  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_FORCETRIGGER);

}

bool AWGController::isConnected() {
	return connected;
}

bool AWGController::run(int timeout, int channel){

  spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START |  M2CMD_CARD_FORCETRIGGER);

  if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szErrorText) != ERR_OK) {
    cout << (szErrorText); // print the error text
    return false;
  }
  // spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_FORCETRIGGER);

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
