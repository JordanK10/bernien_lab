// Created by H.Levine with Atom Array team, Lukin group (2016).


#ifndef CAMERA_CLIENT_H
#define CAMERA_CLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



class CameraClient
{
public:
	CameraClient(bool connect_to_camera);
	~CameraClient();

	void startAcquisition(int numImages);
	void saveImages(int seqNumber);
	std::vector<bool> findAtoms(std::vector<double> frequencies, double centerFreq);

	bool connectToServer();
	void disconnectFromServer();
private:
	int cameraSocket;

	bool hasConnectedToServer;
};



#endif