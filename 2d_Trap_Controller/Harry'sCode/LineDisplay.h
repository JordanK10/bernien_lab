// Created by H.Levine with Atom Array team, Lukin group (2016).


#ifndef LINEDISPLAY_H
#define LINEDISPLAY_H

#include "Bitmap.h"
#include <map>
#include <string>


class LineDisplay
{
public:
	LineDisplay();

	void clearDisplay();
	void setText(std::string str);

	void printDisplayToConsole();
	std::vector<bool> getLine(int line);
private:
	std::map<char, Bitmap> characterBitmap;

	std::vector<std::vector<bool> >displayImage;
};

#endif