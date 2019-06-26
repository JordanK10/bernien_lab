ECHO OFF
cls

ECHO Making....
ECHO _________________________
cd ..\exe
nmake clean
nmake create_dirs
nmake aodctrl
cd ..\scripts
ECHO _________________________
ECHO.
ECHO Finished!
