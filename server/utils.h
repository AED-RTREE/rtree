//============================================================================
// Name        : utils.h
// Author      : Daniel Palomino
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : RTree Backend
// Created on  : 31 jul. 2018
//============================================================================

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

using namespace std;

enum command
{
	DELETE, INSERT, RANGE, NEAREST, SUCCESS, OBJECTS, MBR
};

struct message {
	string command;
	vector<pair<int, int>> points;
};

#endif