#To make from scratch...
# nmake clean; nmake create_dirs; nmake;



EXE_FILE = AODctrl.exe
DIR_INCLUDE = ..\include
DIR_INT = ..\int
DIR_INT_X86 = $(DIR_INT)\x86
DIR_LIB = ..\..\c_header_win
DIR_LIB_CUDA = ..\..\cuda_lib\lib\x64
DIR_INCLUDE_CUDA = ..\..\cuda_lib\include
DIR_SRC = ..\src
DIR_SRC_CUDA = ..\..\cuda_lib\src
DIR_BIN = ..\bin
DIR_BIN_X86 = $(DIR_BIN)\x86
DIR_SB5 = ..\..\sb5_file

SRC_FILES = $(DIR_SRC)\main.cpp \
	$(DIR_SRC)\Trap.cpp \
	$(DIR_SRC)\TrapControllerHandler.cpp \
	$(DIR_SRC)\Waveform.cpp \
	$(DIR_SRC)\WaveTable.cpp \
	$(DIR_SRC)\AWGController.cpp \
	$(DIR_SRC)\Console.cpp \
	$(DIR_SRC)\CameraServer.cpp \
	$(DIR_SRC)\AtomSim.cpp \
	$(DIR_SRC)\spcm_lib_card.cpp \
	$(DIR_SRC)\spcm_lib_data.cpp \
	$(DIR_SRC)\spcm_ostools_win.cpp \
	$(DIR_SRC)\sb5_file.cpp


{$(DIR_SRC)}.cpp{$(DIR_INT_X86)}.obj ::
        @echo Compiling...
 cl /c /EHsc /O2 /Fo$(DIR_INT_X86)\ /Ispcm_win32_msvcpp.lib  /I$(DIR_LIB_CUDA) /I$(DIR_INCLUDE) /I$(DIR_INCLUDE_CUDA)   $<

$(EXE_FILE) : $(DIR_INT_X86)\*.obj
   @echo Linking $(EXE_FILE)...
   link  ..\..\c_header_win\spcm_win64_msvcpp.lib  /out:$(DIR_BIN_X86)\$(EXE_FILE) $(DIR_INT_X86)\*.obj

# build application
aodctrl: $(EXE_FILE)

# create output directories
create_dirs:
 @if not exist $(DIR_BIN_X86) mkdir $(DIR_BIN_X86)
 @if not exist $(DIR_INT_X86) mkdir $(DIR_INT_X86)

# delete output directories
clean:
 @if exist $(DIR_BIN_X86) rmdir /S /Q $(DIR_BIN_X86)
 @if exist $(DIR_INT_X86) rmdir /S /Q $(DIR_INT_X86)

# create directories and build application
all: aodctrl
