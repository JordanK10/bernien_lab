/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016) --
**/

#include <iostream>
#include "Console.h"

using namespace std;

static bool const CONNECT_TO_CAMERA_SERVER = false;
static bool connectToAWG = false;

int main(){

    int len = 5;
    // cin >> len;
    // cout << "Width? ";
    int wid = 5;

    int sw_buf_size = pow(4,6);
    int wt_freq = 1000;
    int cycles = 4000;
    int clock_rate = MEGA(sw_buf_size*wt_freq/cycles); //Use mathematica to find parameter combination that is integer
    double bandwidth = 15;
    double gain = 30;

    // if(input=='N'){
    //
    //   //Run 1D Console
    //   TrapController trapController(centerFrequency, sampleRate, gain);
    //   AWGController awgController(0,sampleRate,centerFrequency,gain);
    //   run1DConsole(trapController, awgController);
    //
    // }else if(input=='Y'){

      //Run 2D Console
      TrapControllerHandler trapControllerHandler(len, wid, clock_rate, gain, KILO(wt_freq));
      AWGController awgController(0,clock_rate,1,gain,SINGLE,KILO(sw_buf_size*40));

      run2DConsole(trapControllerHandler, awgController);



      return 0;
}
