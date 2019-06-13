// Created by H.Levine with Atom Array team, Lukin group (2016).
// Adapted by J.Kemp with Atom Array team, Bernien Group (2019).



#include "Waveform.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>

using namespace std;

Waveform::Waveform() {
}

Waveform::Waveform(string binaryFilename) {
	initializeFromBinaryFile(binaryFilename);
}

bool Waveform::fileExists(string filename) {
	string path(filename);

	ifstream file(path, ios::in | ios::binary);
	return file.good();
}

Waveform::Waveform(std::vector<std::complex<float>> data)
{
	dataVector = data;
}


void Waveform::importFromBinaryFile(vector<complex<float>> tempVector) {
	dataVector = tempVector;
}

void Waveform::initializeFromBinaryFile(string binaryFilename) {
	string path(dir);
	path.append(binaryFilename);

	ifstream binaryFile(path, ios::in | ios::binary);
	unsigned int numElements;
	binaryFile.read(reinterpret_cast<char*>(&numElements), sizeof(unsigned int));

	dataVector.resize(numElements);
	binaryFile.read(reinterpret_cast<char*>(&dataVector[0]), numElements * sizeof(dataVector[0]));
}

bool Waveform::initializeFromStaticWaveform(string trap_configuration_file) {
	int length = trap_configuration_file.length();
	string waveform_filename = trap_configuration_file.substr(0, length - 4) + "_static"; // Remove .txt, replace with _static.
	string waveform_path = static_waveforms + waveform_filename;
	if (fileExists(waveform_path)) {
		initializeFromBinaryFile(waveform_path);
		return true;
	} else {
		return false;
	}
}

void Waveform::initializeFromMovingWaveform(double duration,
											string starting_configuration, string ending_configuration,
											int start_index, int end_index) {
	int starting_length = starting_configuration.length();
	int ending_length = ending_configuration.length();

	stringstream dirStream;
	dirStream << starting_configuration.substr(0, starting_length - 4) << "_to_";
	dirStream << ending_configuration.substr(0, ending_length - 4) << "_in_";
	dirStream << fixed << setprecision(1) << duration << "ms/";

	stringstream fileStream;
	fileStream << "rearrange_" << start_index << "_to_" << end_index;


	string path = dirStream.str() + fileStream.str();
	initializeFromBinaryFile(path);
}

void Waveform::writeToFile(string filename) {
	string path(dir);
	path.append(filename);

	ofstream file(path);
	std::ostream_iterator<complex<float>> output_iterator(file, "\n");

	copy(std::begin(dataVector), std::end(dataVector), output_iterator);
}


void Waveform::writeToBinaryFile(string filename) {
	string path(dir);
	path.append(filename);

	ofstream file(path, ios::out | ios::binary);
	const char *dataPointer = reinterpret_cast<const char*>(&dataVector[0]);
	unsigned int numElements = dataVector.size();

	file.write(reinterpret_cast<const char*>(&numElements), sizeof(unsigned int));
	file.write(dataPointer, numElements * sizeof(dataVector[0]));
	file.close();
}
