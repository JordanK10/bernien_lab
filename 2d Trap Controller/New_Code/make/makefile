#Windows makefile
CC = cl.exe

EXE_FILE = AODctrl.exe
DIR_INCLUDE = ..\include
DIR_INT = ..\int
DIR_SRC = ..\src
DIR_COMMON = ..\..\common
DIR_OSTOOLS =..\ ..\common\ostools
DIR_BIN = ..\bin
DIR_SB5 = ..\..\sb5_file

SRC_FILES = $(DIR_SRC)\main.cpp \
						$(DIR_SRC)\Trap.cpp \
						$(DIR_SRC)\TrapController.cpp \
						$(DIR_SRC)\TrapControllerHandler.cpp \
						$(DIR_SRC)\Waveform.cpp \
						$(DIR_SRC)\WaveTable.cpp \
						$(DIR_COMMON)\spcm_lib_card.cpp \
						$(DIR_COMMON)\spcm_lib_data.cpp \
						$(DIR_OSTOOLS)\spcm_ostools_linux.cpp \
						$(DIR_SB5)\sb5_file.cpp \
						$(DIR_SRC)\AWGController.cpp \
						$(DIR_SRC)\Console.cpp \
						$(DIR_SRC)\CameraServer.cpp \
						$(DIR_SRC)\AtomSim.cpp \

{$(DIR_SRC)}.cpp{$(DIR_INT)}\*obj::
			@echo Compiling...
cl /c /EHsc /Fo$(DIR_INT)\ /I$(DIR_INCLUDE) /I$(DIR_COMMON) \
													 /I$(DIR_OSTOOLS) /I$(DIR_SB5) $<

$(EXECUTABLE_NAME) : $(DIR_INT)\*.obj
  		@echo Linking $(EXECUTABLE_NAME)...
  		link /out:$(DIR_BIN)\$(EXECUTABLE_NAME) $(DIR_INTERMEDIATE)\*.obj

AODcrtl :$(EXE_FILE)

# create output directories
create_dirs:
 @if not exist $(DIR_BIN) mkdir $(DIR_BIN)
 @if not exist $(DIR_INTERMEDIATE) mkdir $(DIR_INTERMEDIATE)

# delete output directories
clean:
 @if exist $(DIR_BIN) rmdir /S /Q $(DIR_BIN)
 @if exist $(DIR_INTERMEDIATE) rmdir /S /Q $(DIR_INTERMEDIATE)

 # create directories and build application
 all: clean create_dirs AODctrl


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
