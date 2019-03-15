CC = g++
CFLAGS = -std=c++11 -O2 -pthread -g
#LIBS = -lspcm_linux

all: main.cpp Console.o CameraServer.o AWGController.o Trap.o TrapController.o StaticController.o TrapControllerHandler.o Waveform.o WaveTable.o

	$(CC) $(CFLAGS) -o TrapController main.cpp Console.o AWGController.o Trap.o TrapController.o StaticController.o TrapControllerHandler.o Waveform.o WaveTable.o -lreadline

Trap.o: Trap.cpp Trap.h
	$(CC) $(CFLAGS) -c Trap.cpp

TrapController.o: TrapController.cpp TrapController.h
	$(CC) $(CFLAGS) -c TrapController.cpp

StaticController.o: StaticController.cpp StaticController.h
	$(CC) $(CFLAGS) -c StaticController.cpp

TrapControllerHandler.o: TrapControllerHandler.cpp TrapControllerHandler.h
		$(CC) $(CFLAGS) -c TrapControllerHandler.cpp

Waveform.o: Waveform.cpp Waveform.h
	$(CC) $(CFLAGS) -c Waveform.cpp

WaveTable.o: WaveTable.cpp WaveTable.h
	$(CC) $(CFLAGS) -c WaveTable.cpp

AWGController.o: AWGController.cpp AWGController.h
	$(CC) $(CFLAGS) -c AWGController.cpp

Console.o: Console.cpp Console.h TrapController.h StaticController.h AWGController.h
	$(CC) $(CFLAGS) -c Console.cpp

CameraServer.o: CameraServer.cpp CameraServer.h
		$(CC) $(CFLAGS) -c CameraServer.cpp

clean:
	$(RM) TrapController *.o Console *.o
