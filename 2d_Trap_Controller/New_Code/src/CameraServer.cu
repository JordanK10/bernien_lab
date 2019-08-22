// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "CameraServer.h"


using namespace std;

// static const char SERVER_ADDR[] = "192.168.10.2";
// static const int PORT = 1234;
//
// static const size_t buffer_size = 1024;
//
// static const int EXIT_CODE = -1;
// static const int CLOSE_CODE = -2;
//
// static int waitingSocket;



string CLOSE_STRING("close");				// Closes connection.
string EXIT_STRING("exit");					// Ask server to shut down.
string START_ACQUISITION_STRING("start");	// Starts acquisiton of images.
string FIND_ATOMS_STRING("find");			// Find atoms in image.
string SAVE_SEQUENCE_STRING("save");		// Save images from sequence.

CameraServer::CameraServer()
{

}

CameraServer::~CameraServer()
{
	// close(serverSocket);
	// close(cameraSocket);
}


//
bool CameraServer::startServer() {
	cout << "Starting server..." << endl;
	// serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	// if (serverSocket < 0) {
	// 	cout << "Unable to open socket!" << endl;
	// 	return false;
	// }
	return true;
}

vector<vector<bool>> CameraServer::receiveIdentifiedAtomList(int numTraps, int rowLen) {
		return MakeBoolArray(rowLen,.7);
}
