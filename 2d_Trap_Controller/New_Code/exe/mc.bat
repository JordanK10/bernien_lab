ECHO OFF
cls

ECHO Making....
ECHO _________________________
del *.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\TrapController.cu -c -o  TrapController.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\TrapControllerHandler.cpp -c -o  TrapControllerHandler.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\AWGController.cpp -c -o  AWGController.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\CameraServer.cpp -c -o  CameraServer.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\Console.cpp -c -o  Console.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\main.cpp -c -o  main.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\rearrange2d.cpp -c -o  rearrange2d.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\sb5_file.cpp -c -o  sb5_file.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\spcm_lib_card.cpp -c -o  spcm_lib_card.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\spcm_lib_data.cpp -c -o  spcm_lib_data.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\spcm_ostools_win.cpp -c -o  spcm_ostools_win.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\Trap.cpp -c -o  Trap.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\Waveform.cpp -c -o  Waveform.o
nvcc.exe --dont-use-profile -m 64 -ldir ..\..\cuda_lib\nvvm\libdevice -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64 ..\src\WaveTable.cpp -c -o  WaveTable.o
ECHO _________________________
nvcc.exe -link TrapController.o TrapControllerHandler.o AWGController.o CameraServer.o Console.o main.o rearrange2d.o sb5_file.o spcm_lib_card.o spcm_lib_data.o spcm_ostools_win.o Trap.o Waveform.o WaveTable.o
ECHO _________________________
ECHO.
ECHO Finished!
