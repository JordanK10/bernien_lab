
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
..\bin\x86\AODctrl.exe
