/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016) --
**/

#include <iostream>
#include "Console.h"

using namespace std;

static bool const CONNECT_TO_CAMERA_SERVER = false;
static bool connectToAWG = false;

int main(){

    //Determine if we need to run protocol for 1D or 2D
    char input = 'Y';
    // cout << "2D operation? (Y/N)";
    // cin >> input;
    // cout << "Length? ";
    int len = 5;
    // cin >> len;
    // cout << "Width? ";
    int wid = 5;
    // cin >> wid;
    /////////////////////////////

    //Input Center frequency

    double sampleRate = 100E6;
    double centerFreq_MHz = 74;
    double bandwidth = 15;
    double centerFrequency = 77;
    double gain = 1;

    cout << "Enter center frequency (in MHz): ";
    // cin >> centerFreq_MHz;
    centerFrequency = centerFreq_MHz * 1.0E6;
    //////////////////////////////

    // if(input=='N'){
    //
    //   //Run 1D Console
    //   TrapController trapController(centerFrequency, sampleRate, gain);
    //   AWGController awgController(0,sampleRate,centerFrequency,gain);
    //   run1DConsole(trapController, awgController);
    //
    // }else if(input=='Y'){

      //Run 2D Console
      TrapControllerHandler trapControllerHandler(len, wid, centerFrequency-.5*bandwidth, len/bandwidth, centerFrequency-.5*bandwidth, len/bandwidth,  sampleRate, gain);
      AWGController awgController(0,sampleRate,centerFrequency,gain);

      run2DConsole(trapControllerHandler, awgController);



      return 0;
}
