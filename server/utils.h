//============================================================================
// Name        : utils.h
// Author      : Daniel Palomino
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
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
	INSERT, RANGE, NEAREST,  MBRS, OBJECTS, DELETE, SUCCESS, FAIL, EXIT
};

struct message {
	string command;
	int k;
	vector<pair<int, int>> points;
};

static command str2cmd(string cmd) {
	if (cmd == "INSERT") {
		return INSERT;
	}else if (cmd == "RANGE") {
		return RANGE;
	}
	else if (cmd == "NEAREST") {
		return NEAREST;
	}
	else if (cmd == "MBRS") {
		return MBRS;
	}
	else if (cmd == "OBJECTS") {
		return OBJECTS;
	}
	else if (cmd == "DELETE") {
		return DELETE;
	}
	else if (cmd == "SUCCESS") {
		return SUCCESS;
	}
	else if (cmd == "FAIL") {
		return FAIL;
	}
	else if (cmd == "EXIT") {
		return EXIT;
	}
	else {
		return FAIL;
	}
}

#endif