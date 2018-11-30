// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "CameraClient.h"
#include <iomanip>
#include <string>
#include <strings.h>
#include <sstream>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

static const char SERVER_ADDR[] = "192.168.10.2";
static const int SERVER_PORT = 1234;

static const size_t buffer_size = 1024;

static const int EXIT_CODE = -1;
static const int CLOSE_CODE = -2;


string CLOSE_STRING("close");				// Closes connection.
string EXIT_STRING("exit");					// Ask server to shut down.
string START_ACQUISITION_STRING("start");	// Starts acquisiton of images.
string FIND_ATOMS_STRING("find");			// Find atoms in image.
string SAVE_SEQUENCE_STRING("save");		// Save images from sequence.


CameraClient::CameraClient(bool connect_to_camera)
{
	hasConnectedToServer = false;

	if (connect_to_camera) {
		connectToServer();
	}
}

CameraClient::~CameraClient()
{
	string str("close");
	send(cameraSocket, str.c_str(), str.size(), 0);

	close(cameraSocket);
}

void CameraClient::disconnectFromServer() {
	if (hasConnectedToServer) {
		close(cameraSocket);
	}
}

bool CameraClient::connectToServer() {
	disconnectFromServer();

	struct sockaddr_in serv_addr;
	cameraSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (cameraSocket < 0) {
		cout << "Unable to open socket!" << endl;
		return false;
	}

	struct hostent *server = gethostbyname(SERVER_ADDR);
	if (server == NULL) {
		cout << "Unable to find host at address " << SERVER_ADDR << endl;
		return false;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(SERVER_PORT);

	if (connect(cameraSocket, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		cout << "Unable to connect to host!" << endl;
		return false;
	}

	hasConnectedToServer = true;
	return true;
	
}

void CameraClient::startAcquisition(int numImages) {
	string str = START_ACQUISITION_STRING + " " + to_string(numImages) + "\n";
	send(cameraSocket, str.c_str(), str.size(), 0);
	cout << "Started acquisition: " << numImages << " images." << endl;
}

void CameraClient::saveImages(int seqNumber) {
	string str = SAVE_SEQUENCE_STRING + " " + to_string(seqNumber) + "\n";
	send(cameraSocket, str.c_str(), str.size(), 0);
	cout << "Saving image... ";

	char buf[2];
	bzero(buf, 2);
	recv(cameraSocket, buf, 2, 0);
	if (buf[0] == '1') {
		cout << "Success!" << endl;
	}
	else {
		cout << "Error. See camera server." << endl;
	}
}

vector<bool> CameraClient::findAtoms(vector<double> frequencies, double centerFreq) {
	stringstream stream;
	stream << FIND_ATOMS_STRING;
	for (int i = 0; i < frequencies.size(); i++) {
		stream << " " << (frequencies[i] + centerFreq);
	}
	stream << endl;

	string str = stream.str();
	send(cameraSocket, str.c_str(), str.size(), 0);

	char buf[1024];
	bzero(buf, 1024);
	//cout << "Looking for atoms..." << endl;
	recv(cameraSocket, buf, 1024, 0);
	stringstream ret(buf);
	//cout << "Found atoms: " << ret.str() << endl;

	vector<bool> atomsPresent;
	for (int i = 0; i < frequencies.size(); i++) {
		int pres;
		ret >> pres;

		if (pres == 1) {
			atomsPresent.push_back(true);
		}
		else {
			atomsPresent.push_back(false);
		}
	}

	return atomsPresent;
}
