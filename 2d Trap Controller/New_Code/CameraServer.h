// Created by H.Levine with Atom Array team, Lukin group (2016).

#ifndef CAMERA_SERVER_H
#define CAMERA_SERVER_H
//
// #include <iostream>
// #include <string>
#include <vector>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <iomanip>
#include <iostream>
#include <string>
// #include <strings.h>
#include <sstream>
// #include <netdb.h>
// #include <sys/time.h>
// #include <sys/types.h>
// #include <sys/ioctl.h>
// #include <unistd.h>
#include <vector>
#include <cstdlib>
#include <signal.h>
#include <cstring>

class CameraServer
{
public:
	CameraServer();
	~CameraServer();

	bool startServer();
	bool acceptConnection();


	std::vector<std::vector<bool>> receiveIdentifiedAtomList(int numTraps, int rowLen);

private:
	int serverSocket;
	int cameraSocket;

};



#endif
