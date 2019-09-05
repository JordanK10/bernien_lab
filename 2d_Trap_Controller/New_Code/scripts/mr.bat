
ECHO OFF
cls

ECHO Making...
ECHO _________________________
cd "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\bin"
nvcc.exe -ccbin "C:\Program Files (x86)\Microsoft Visual Studio\2019\community\VC\Tools\MSVC\14.22.27905\bin\Hostx64\x64" -m 64 -L  "..\..\..\..\..\users\bernien_lab\documents\bernien_lab\2d_trap_controller\c_header_win" -lspcm_win64_msvcpp -I..\include\ -I..\..\cuda_lib\include -L..\..\cuda_lib\lib\x64\ -I"..\..\..\..\..\users\bernien_lab\documents\bernien_lab\2d_trap_controller\new_code\include" "..\..\..\..\..\users\bernien_lab\documents\bernien_lab\2d_trap_controller\new_code\src\TrapController.cu" -c -o  "C:\Users\bernien_lab\Documents\bernien_lab\2d_Trap_Controller\New_Code\int\x86\TrapController.obj"
cd "c:\users\bernien_lab\documents\bernien_lab\2d_Trap_Controller\new_code\scripts
cd ..\exe
nmake aodctrl
cd ..\scripts
ECHO _________________________
ECHO.
ECHO Finished! Running AODctrl
ECHO _________________________
..\bin\x86\AODctrl.exe echo
