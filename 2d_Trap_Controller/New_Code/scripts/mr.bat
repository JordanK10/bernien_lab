
ECHO OFF
cls

ECHO Making...
ECHO _________________________
cd ..\exe
nmake aodctrl
cd ..\scripts
ECHO _________________________
ECHO.
ECHO Finished! Running AODctrl
ECHO _________________________
ECHO.


 ( echo traps load_default 10(1^^^)x10(1^^^)tr.txt
   echo awg gain .7
   echo awg push_waveform
   echo awg run
   echo run rearrangement
   echo 0
   echo awg trigger
   pause
   echo awg trigger
   pause
   echo awg trigger
   pause
   echo awg stop
   echo exit)  | ..\bin\x86\AODctrl.exe echo
