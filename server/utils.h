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
	DELETE, INSERT, RANGE, NEAREST, SUCCESS, MBR, OBJECTS
};

struct message {
	string command;
	vector<pair<int, int>> points;
};

static map<string, command> str2cmd{
	{ "DELETE", DELETE },
	{ "INSERT", INSERT },
	{ "RANGE", RANGE },
	{ "NEAREST", NEAREST },
	{ "SUCCESS", SUCCESS },
	{ "MBR", MBR },
	{ "OBJECTS", OBJECTS }
};

static string cmd2str(command cmd) {

	switch (cmd) {
	case DELETE:
		return "DELETE";

	case INSERT:
		return "INSERT";

	case RANGE:
		return "RANGE";

	case NEAREST:
		return "NEAREST";

	case SUCCESS:
		return "SUCCESS";

	case MBR:
		return "MBR";

	case OBJECTS:
		return "OBJECTS";

	default:
		return "ERROR";
	}
}

#endif