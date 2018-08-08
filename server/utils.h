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