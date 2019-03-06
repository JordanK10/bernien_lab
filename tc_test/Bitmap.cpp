// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "Bitmap.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

Bitmap::Bitmap() {
}

Bitmap::Bitmap(vector<string> bitLines) {
	initializeWithLines(bitLines);
}


void Bitmap::initializeWithLines(vector<string> bitLines) {
	bitmap.clear();

	for (int i = 0; i < bitLines.size(); i++) {
		vector<bool> lineOfBits;
		stringstream stream(bitLines[i]);

		bool pixOn;
		for (int b = 0; b < 5; b++) {
			stream >> pixOn;

			lineOfBits.push_back(pixOn);
		}

		bitmap.push_back(lineOfBits);
	}
}
