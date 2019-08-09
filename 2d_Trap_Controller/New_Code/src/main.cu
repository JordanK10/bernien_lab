/** Created by J.Kemp with the atom Array team, Bernien group (2018). Based on
  protocol developed by H.Levin with the Lukin Group (2016) --
**/

#include <iostream>
#include "Console.h"

using namespace std;

// static bool const CONNECT_TO_CAMERA_SERVER = false;
static bool connectToAWG = false;

int main(){

    int sw_buf_size = pow(4,6);
    int wt_freq = 100;
    int cycles = 400;
    int clock_rate = MEGA(sw_buf_size*wt_freq/cycles); //Use mathematica to find parameter combination that is integer
    // double bandwidth = 15;
    int gain = 32761;

      TrapControllerHandler trapControllerHandler(clock_rate, gain, KILO(wt_freq));
      AWGController awgController(clock_rate,SEQUENCE,KILO(sw_buf_size));

      // if (awgsController.isConnected())
        run2DConsole(trapControllerHandler, awgController);
      return 0;
}
