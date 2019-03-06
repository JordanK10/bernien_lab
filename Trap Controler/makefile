CC = g++
CFLAGS = -std=c++11 -O2 -pthread

all: main.cpp control_interface.o Console.o CameraServer.o SDRController.o Bitmap.o LineDisplay.o Trap.o TrapController.o Waveform.o WaveTable.o
	$(CC) $(CFLAGS) -o TrapController main.cpp control_interface.o Console.o CameraServer.o SDRController.o Bitmap.o LineDisplay.o Trap.o TrapController.o Waveform.o WaveTable.o /usr/include/uhd.h /usr/lib/x86_64-linux-gnu/libuhd.so -lreadline -I/opt/adwin/include -L/opt/adwin/lib -ladwin -Wl,--rpath -Wl,/opt/adwin/lib

CameraServer.o: CameraServer.cpp CameraServer.h
	$(CC) $(CFLAGS) -c CameraServer.cpp

Bitmap.o: Bitmap.cpp Bitmap.h
	$(CC) $(CFLAGS) -c Bitmap.cpp

LineDisplay.o: LineDisplay.cpp LineDisplay.h Bitmap.cpp Bitmap.h
	$(CC) $(CFLAGS) -c LineDisplay.cpp

control_interface.o: control_interface.cpp control_interface.h
	$(CC) $(CFLAGS) -c control_interface.cpp -I/opt/adwin/include -L/opt/adwin/lib -ladwin -Wl,--rpath -Wl,/opt/adwin/lib

SDRController.o: SDRController.cpp SDRController.h Waveform.h
	$(CC) $(CFLAGS) -c SDRController.cpp

Trap.o: Trap.cpp Trap.hpp WaveTable.hpp
	$(CC) $(CFLAGS) -c Trap.cpp Trap.hpp WaveTable.hpp

TrapController.o: TrapController.cpp TrapController.h WaveTable.hpp Waveform.h LineD`isplay.h
	$(CC) $(CFLAGS) -c TrapController.cpp

Waveform.o: Waveform.cpp Waveform.h
	$(CC) $(CFLAGS) -c Waveform.cpp

WaveTable.o: WaveTable.cpp WaveTable.hpp
	$(CC) $(CFLAGS) -c WaveTable.cpp

Console.o: Console.cpp Console.h control_interface.h CameraServer.h TrapController.h SDRController.h
	$(CC) $(CFLAGS) -c Console.cpp


clean:
	$(RM) TrapController *.o
