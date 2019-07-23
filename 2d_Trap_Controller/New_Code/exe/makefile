#To make from scratch...
# nmake clean; nmake create_dirs; nmake;



EXE_FILE = AODctrl.exe
DIR_INCLUDE = ..\include
DIR_INT = ..\int
DIR_INT_X86 = $(DIR_INT)\x86
DIR_LIB = ../../c_header_win
DIR_SRC = ..\src
DIR_BIN = ..\bin
DIR_BIN_X86 = $(DIR_BIN)\x86
DIR_SB5 = ..\..\sb5_file

SRC_FILES = $(DIR_SRC)\main.cpp \
	$(DIR_SRC)\Trap.cpp \
	$(DIR_SRC)\TrapController.cpp \
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
 cl /c /EHsc /Fo$(DIR_INT_X86)\ /Ispcm_win32_msvcpp.lib  /I$(DIR_INCLUDE)  $<

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

# Linux makefile
# CC = g++
# LIBS = -O2 -pthread -lspcm_linux -fpermissive
#
# OBJECTS=  Console.o AtomSim.o CameraServer.o AWGController.o Trap.o TrapController.o  TrapControllerHandler.o Waveform.o WaveTable.o 	../common/spcm_lib_card.o ../common/spcm_lib_data.o ../common/ostools/spcm_ostools_linux.o ../sb5_file/sb5_file.o
#
# all: main.cpp $(OBJECTS)
# 	$(CC) $(LIBS) -o AODctrl main.cpp $(OBJECTS) $(LIBS)
# Trap.o: Trap.cpp Trap.h
# 	$(CC) $(CFLAGS) -c Trap.cpp
#
# TrapController.o: TrapController.cpp TrapController.h
# 	$(CC) $(CFLAGS) -c TrapController.cpp
#
# TrapControllerHandler.o: TrapControllerHandler.cpp TrapControllerHandler.h
# 		$(CC) $(CFLAGS) -c TrapControllerHandler.cpp
#
# Waveform.o: Waveform.cpp Waveform.h
# 	$(CC) $(CFLAGS) -c Waveform.cpp
#
# WaveTable.o: WaveTable.cpp WaveTable.h
# 	$(CC) $(CFLAGS) -c WaveTable.cpp
#
# spcm_lib_card.o: ../common/spcm-lib_card.cpp ../common/spcm_lib_card.h
# 	$(CC) $(CFLAGS) -c ../common/spcm_lib_card.cpp
#
# spcm_lib_data.o: ../common/spcm-lib_data.cpp ../common/spcm_lib_data.h
# 	$(CC) $(CFLAGS) -c ../common/spcm_lib_data.cpp
#
# spcm_ostools_linux.o: ../common/ostools/spcm_ostools_linux.cpp ../common/ostools/spcm_ostools.h
# 	$(CC) $(CFLAGS) -c ../common/ostools/spcm_ostools_linux.cpp
#
# sb5_file.o: ../sb5_file/sb5_file.cpp ../sb5_file/sb5_file.h
# 	$(CC) $(CFLAGS) -c ../sb5_file/sb5_file.cpp
#
# AWGController.o: AWGController.cpp AWGController.h ../common/spcm_lib_card.h ../common/spcm_lib_data.h ../common/ostools/spcm_ostools.h ../sb5_file/sb5_file.h
# 	$(CC) $(CFLAGS) -c AWGController.cpp -lspcm_linux
#
# Console.o: Console.cpp Console.h TrapController.h AWGController.h
# 	$(CC) $(CFLAGS) -c Console.cpp
#
# CameraServer.o: CameraServer.cpp CameraServer.h AtomSim.h
# 		$(CC) $(CFLAGS) -c CameraServer.cpp
#
# AtomSim.o: AtomSim.cpp AtomSim.h
# 		$(CC) $(CFLAGS) -c AtomSim.cpp
#
# clean:
# 	$(RM) TrapController *.o Console *.o
