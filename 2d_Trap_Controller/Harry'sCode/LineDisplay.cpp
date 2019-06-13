// Created by H.Levine with Atom Array team, Lukin group (2016).


#include "LineDisplay.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


LineDisplay::LineDisplay() {
	ifstream bitmap_file("/home/control/Desktop/TrapController/character_bitmaps.txt", ios::in);

	string line;

	for (int c = 0; ; c++) {
		vector<string> bitLines;

		if (!getline(bitmap_file, line)) {
			break;
		}

		char character = line.c_str()[0];
		for (int i = 0; i < 6; i++) {
			getline(bitmap_file, line);
			bitLines.push_back(line);
		}

		characterBitmap[character] = Bitmap();
		characterBitmap[character].initializeWithLines(bitLines);
	}

	bitmap_file.close();
}

void LineDisplay::clearDisplay() {
	displayImage.clear();

	int WIDTH = 100;
	int HEIGHT = 6;

	for (int y = 0; y < HEIGHT; y++) {
		vector<bool> line;

		for (int x = 0; x < WIDTH; x++) {
			line.push_back(false);
		}

		displayImage.push_back(line);
	}
}

void LineDisplay::setText(string text) {
	clearDisplay();

	int numChars = text.size();
	int HORIZONTAL_CHAR_SPACING = 7;
	int WIDTH_PER_CHAR = 5;
	int HEIGHT_PER_CHAR = 6;

	if (WIDTH_PER_CHAR * numChars >= 100) {
		cout << "Too many characters to print!" << endl;
		cout << "Just printing:" << endl;

		numChars = 100 / HORIZONTAL_CHAR_SPACING;
		cout << "\t" << text.substr(0, numChars) << endl;
	}

	const char *textChars = text.c_str();

	for (int i = 0; i < numChars; i++) {
		char character = toupper(textChars[i]);

		if (characterBitmap.find(character) == characterBitmap.end()) {
			continue;
		}

		int startX = HORIZONTAL_CHAR_SPACING * i;
		int startY = 0;

		for (int x = startX; x < startX + WIDTH_PER_CHAR; x++) {
			for (int y = startY; y < startY + HEIGHT_PER_CHAR; y++) {
				displayImage[y][x] = characterBitmap[character].bitmap[y - startY][x - startX];
			}
		}
	}
}

void LineDisplay::printDisplayToConsole() {
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 100; x++) {
			cout << displayImage[y][x];
		}
		cout << endl;
	}
}

vector<bool> LineDisplay::getLine(int line) {
	if (line >= 6 || line < 0) {
		vector<bool> v;
		for (int i = 0; i < 100; i++) {
			v.push_back(true);
		}
		return v;
	}

	return displayImage[line];
}