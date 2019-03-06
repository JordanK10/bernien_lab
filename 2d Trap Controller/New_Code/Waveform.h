// Created by H.Levine with Atom Array team, Lukin group (2016).
// Adapted by J.Kemp with Atom Array team, Bernien Group (2019).

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <vector>
#include <complex>
#include <string>

class Waveform
{
public:
	Waveform();
	Waveform(std::string binaryFilename);
	Waveform(std::vector<std::complex<float>> data);

	std::vector<std::complex<float>> dataVector;

	bool shouldNotifyAfterSending = false;

	static bool fileExists(std::string filename);

	void initializeFromBinaryFile(std::string filename);
	void writeToFile(std::string filename);
	void writeToBinaryFile(std::string filename);

	bool initializeFromStaticWaveform(std::string trap_configuration);
	void initializeFromMovingWaveform(double duration,
									  std::string starting_configuration, std::string ending_configuration,
									  int starting_index, int ending_index);

};

#endif
