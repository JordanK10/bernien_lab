// Created by H.Levine with Atom Array team, Lukin group (2016).
// Adapted by J.Kemp with Atom Array team, Bernien Group (2019).

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <vector>
#include <string>
#include <windows.h>
#include <sstream>

static const std::string dir = "C:\\Users\\Bernien Lab\\Documents\\bernien_lab\\2d_Trap_Controller\\New_Code\\bin\\TrapWaveforms\\";
static const std::string static_waveforms = "C:\\Users\\Bernien Lab\\Documents\\bernien_lab\\2d_Trap_Controller\\New_Code\\bin\\TrapWaveforms\\";


class Waveform
{
public:
	Waveform();
	Waveform(std::string binaryFilename);
	Waveform(short* data);
	Waveform(std::vector<short> data);

	std::vector<short> dataVectorShort;
	short* dataShort;
	bool shouldNotifyAfterSending = false;

	static bool fileExists(std::string filename);

	int initializeFromBinaryFile(std::string binaryFilename);
	bool initializeFromStaticWaveform(std::string trap_configuration_file);

	void writeToFile(std::string filename);
	void writeToBinaryFile(std::string filename);

	int initializeFromMovingWaveform(double duration,
									  std::string starting_configuration, std::string ending_configuration,
									  int starting_index, int ending_index);

	bool row;
	int dim;
};
#endif
