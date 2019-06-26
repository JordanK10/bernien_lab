// Created by H.Levine with Atom Array team, Lukin group (2016).


#ifndef BITMAP_H
#define BITMAP_H

#include <map>
#include <vector>
#include <string>


class Bitmap
{
public:
	Bitmap();
	Bitmap(std::vector<std::string> bitLines);

	void initializeWithLines(std::vector<std::string> bitLines);

	std::vector<std::vector<bool> > bitmap;

};

#endif