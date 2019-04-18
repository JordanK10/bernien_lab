/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016) --
**/

#include <iostream>
#include <math.h>
#include <vector>
#include<cmath>
#include "../../common/spcm_lib_card.h"
#include "../../common/spcm_lib_data.h"
#include "../../common/ostools/spcm_oswrap.h"
#include "../../common/ostools/spcm_ostools.h"
#include "../../sb5_file/sb5_file.h"
using namespace std;

static bool const CONNECT_TO_CAMERA_SERVER = false;
static bool connectToAWG = false;

char                szBuffer[1024];     // a character buffer for any messages
ST_SPCM_CARDINFO    stCard;             // info structure of my card
void*               pvBuffer = NULL;
uint32              dwErr;
int                 nKeyCheck = 0;      // key check counter to avoid to much key polling

// setup for the FIFO mode (HW buffer size can be programmed starting with firmware V9)
int64        llHWBufSize = KILO_B(200);
int64        llSWBufSize = KILO_B(200);
int64        llNotifySize = KILO_B(200); // the data transfer speed to the card increases with the notify size

std::vector<double> genSin(){
  std::vector<double> vec;
  int OG = 16000000;

  for ( int i=0; i<KILO_B(100); i++){
    vec.push_back(OG*sin(2*i*3.14/(KILO_B(100))));
  //  cout << vec(i) << "\n";
  }
    cout  << "SIZE OF THE VECTOR " << int(vec.size()) << "\n";
  return vec;
}

void vDoCardSetup (ST_SPCM_CARDINFO *pstCard, int32 lReplayMode, int64 llLoops = 0){
    int i;
    int64 llChannelMask;

    llChannelMask = 1;


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

            spcm_dwSetParam_i32 (pstCard->hDrv, SPCM_X0_MODE, SPCM_XMODE_CONTOUTMARK); // output continuous marker

            break;

        // single restart (one signal on every trigger edge) with ext trigger positive edge
        case SPC_REP_STD_SINGLERESTART:
            bSpcMSetupModeRepStdSingleRestart (pstCard, llChannelMask, KILO_B(64), 0);
            bSpcMSetupTrigExternal (pstCard, SPC_TM_POS, false, 0);
            break;
          }




    // type dependent card setup
    switch (pstCard->eCardFunction)
        {

        // analog generator card setup
        case AnalogOut:

            // program all output channels to +/- 1 V with no offset
            for (i=0; i < pstCard->lMaxChannels; i++)
                bSpcMSetupAnalogOutputChannel (pstCard, i, 1000, 0, 0);
            break;

        // digital generator card setup
        case DigitalOut:
        case DigitalIO:
            for (i=0; i < pstCard->uCfg.stDIO.lGroups; i++)
                bSpcMSetupDigitalOutput (pstCard, i, SPCM_STOPLVL_LOW, 0, 3300);
            break;
        }
    }



    bool loadDataBlock(std::vector<double> dataArr, int channel, int64 llBytesToCalculate){
      cout << "hoot\n";

      // Generate array of pointers to buffer memory
      int16* pnData = (int16*) pvBuffer;

      // Populate buffer data
      int64 k;
    	for (int64 i = 0; i < llSWBufSize; i++){
        pnData[i] = (int16)(dataArr[i]); //PUT DATA HURRRRRRR
      }


      return true;
    }


int main(){
  cout << "yooooo \n";

  uint64              qwMemInBytes;


  // ------------------------------------------------------------------------
  // init card number 0 (the first card in the system), get some information and print it
  // uncomment the second line and replace the IP address to use remote
  // cards like in a generatorNETBOX
  if (bSpcMInitCardByIdx (&stCard, 0))
  //if (bSpcMInitCardByIdx (&stCard, "192.168.1.10", 0))
      {
      printf (pszSpcMPrintDocumentationLink (&stCard, szBuffer, sizeof (szBuffer)));
      printf (pszSpcMPrintCardInfo (&stCard, szBuffer, sizeof (szBuffer)));
      }
  else
      return nSpcMErrorMessageStdOut (&stCard, "Error: Could not open card\n", true);


  // check whether we support this card type in the example
  if ((stCard.eCardFunction != AnalogOut) && (stCard.eCardFunction != DigitalOut) && (stCard.eCardFunction != DigitalIO))
      return nSpcMErrorMessageStdOut (&stCard, "Error: Card function not supported by this example\n", false);


  // ------------------------------------------------------------------------
  // do the card setup, error is routed in the structure so we don't care for the return values
  vDoCardSetup (&stCard, SPC_REP_STD_SINGLE, 0/*forever*/);

  // ------------------------------------------------------------------------
  // calculate the amount of data we need and allocate memory buffer
  if (!stCard.bSetError)
      {

      // buffer for data transfer, containing multiplexed data later on
      qwMemInBytes = stCard.llSetMemsize * stCard.lBytesPerSample * stCard.lSetChannels;

      pvBuffer = pvAllocMemPageAligned (qwMemInBytes);
      cout << "haer5555e3 \n";

      if (!pvBuffer){
          return nSpcMErrorMessageStdOut (&stCard, "Memory allocation error\n", false);
        }
      // calculate the data
      if (!loadDataBlock(genSin(),1,llSWBufSize)){
         cout << "haere3 \n";

        return nSpcMErrorMessageStdOut (&stCard, "Data calculation failed\n", false);
      cout << "hae00000e \n";

      }
    }
cout << "haer3412e \n";

  // ------------------------------------------------------------------------
  // start the generation
  if (!stCard.bSetError)
      {

      // we define the buffer for transfer and start the DMA transfer
      printf ("Starting the DMA transfer and waiting until data is in board memory\n");
      spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, pvBuffer, 0, qwMemInBytes);
      spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA);

      // check for error code
      if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer))
          {
          vFreeMemPageAligned (pvBuffer, qwMemInBytes);
          return nSpcMErrorMessageStdOut (&stCard, szBuffer, false);
          }
      printf ("... data has been transferred to board memory\n");

      // We'll start and wait untill the card has finished or until a timeout occurs
      spcm_dwSetParam_i32 (stCard.hDrv, SPC_TIMEOUT, 50000);
      printf ("\nStarting the card and waiting for ready interrupt\n(continuous and single restart will have timeout)\n");
      if (spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER | M2CMD_CARD_WAITREADY) == ERR_TIMEOUT)
          {
          vFreeMemPageAligned (pvBuffer, qwMemInBytes);
          spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
          return nSpcMErrorMessageStdOut (&stCard, "... Timeout", false);
          }
      }

      cout << "fgweg 2 2 \n";

  // ------------------------------------------------------------------------
  // print error information if an error occured
  if (stCard.bSetError)
      return nSpcMErrorMessageStdOut (&stCard, "An error occured while programming the card:\n", true);
      cout << "here3453 \n";

  // clean up and close the driver
  vSpcMCloseCard (&stCard);

  vFreeMemPageAligned (pvBuffer, qwMemInBytes);

  return EXIT_SUCCESS;
  }


//   // ------------------------------------------------------------------------
// 	// init card number 0 (the first card in the system), get some information and print it
// 	if (bSpcMInitCardByIdx(&stCard, 0))
// 	{
// 		printf(pszSpcMPrintDocumentationLink(&stCard, szBuffer, sizeof(szBuffer)));
// 		printf(pszSpcMPrintCardInfo(&stCard, szBuffer, sizeof(szBuffer)));
//
// 	}
// 	else{
// 		nSpcMErrorMessageStdOut(&stCard, "Error: Could not open card\n", true);
// }
//
//
// 	// ------------------------------------------------------------------------
// 	// do the card setup, error is routed in the structure so we don't care for the return values
// 	if (!stCard.bSetError)
// 	{
//      vDoCardSetup (&stCard, SPC_REP_STD_SINGLE, 0/*forever*/);
// 		//if (bDoCardSetup(&stCard))
// 		//	printf("Error: An error occured in card setup\n");
// 	}
//
//
// 	// we program the hardware buffer size to reduce the latency
// 	spcm_dwSetParam_i64(stCard.hDrv, SPC_DATA_OUTBUFSIZE, llHWBufSize);
// 	spcm_dwSetParam_i32(stCard.hDrv, SPC_M2CMD, M2CMD_CARD_WRITESETUP);
//
//   if (stCard.bSetError){
//     cout << "Crikey";
//     return 0;
//   }
//   // ------------------------------------------------------------------------
// 	// allocate and setup the fifo buffer and fill it once with data
// 	pvBuffer = pvAllocMemPageAligned((uint32)llSWBufSize);
// 	if (!pvBuffer){
//     cout << "fail";
// 		nSpcMErrorMessageStdOut(&stCard, "Memory allocation error\n", false);
//     return 0;
//   }
//
//   loadDataBlock(genSin(),1,llSWBufSize);
//
//   if (!stCard.bSetError)
//       {
//       // we define the buffer for transfer and start the DMA transfer
//       printf ("Starting the DMA transfer and waiting until data is in board memory\n");
//       spcm_dwDefTransfer_i64 (stCard.hDrv, SPCM_BUF_DATA, SPCM_DIR_PCTOCARD, 0, pvBuffer, 0, llSWBufSize);
//       spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_DATA_STARTDMA | M2CMD_DATA_WAITDMA);
//
//       // check for error code
//       if (spcm_dwGetErrorInfo_i32 (stCard.hDrv, NULL, NULL, szBuffer))
//           {
//           vFreeMemPageAligned (pvBuffer, llSWBufSize);
//           nSpcMErrorMessageStdOut (&stCard, szBuffer, false);
//           }
//       else{
//           spcm_dwSetParam_i32 (stCard.hDrv, SPC_TIMEOUT, 30000);
//
//           printf ("... data has been transferred to board memory\n");
//           cout << "\nStarting the card and waiting for ready interrupt\n(continuous and single restart will have timeout)\n";
//           if (spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER | M2CMD_CARD_WAITREADY) == ERR_TIMEOUT)
//               {
//               spcm_dwSetParam_i32 (stCard.hDrv, SPC_M2CMD, M2CMD_CARD_STOP);
//               cout << "timeout\n";
//               }
//           }
//       }
//     else
//       cout << "Error \n";
//     cout << "End\n";
//     vFreeMemPageAligned (pvBuffer, (int32)llSWBufSize);
//
//     return 1;

// }
