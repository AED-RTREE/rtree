//============================================================================
// Name        : main.cpp
// Author      : Daniel Palomino
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : RTree Backend
// Created on  : 31 jul. 2018
//============================================================================

#include <string>
#include <vector>
#include <iostream>
#include "TcpServer.h"
#include "RTree.h"

using namespace std;

int main(int argc, char *argv[])
{
	TcpServer server("localhost", 8080);
	RTree rtree(2);

	string output;
	vector<vector<pair<int, int>>> objects;

	if (server.bindSocket()) {
		server.listenToClient();
	}
	else {
		cout << "Server not created" << endl;
		return 1;
	}

	if (server.acceptConnection()) {
		do {
			vector<vector<pair<int, int>>> objects;

			if (server.receiveData(512)) {
				server.join("SUCCESS", objects, output);
				server.sendData(output);

				switch (str2cmd(server.client_message.command))
				{
				case INSERT:
					/*rtree.insert(server.client_message.points);
					rtree.getMBRs(objects);
					server.join("MBRS", objects, output);*/
					objects.push_back(server.client_message.points);
					server.join("MBRS", objects, output);
					cout << output << endl;
					break;
				case RANGE:
					rtree.range(server.client_message.points, objects);
					server.join("OBJECTS", objects, output);
					break;
				case NEAREST:
					rtree.nearest(server.client_message.k, server.client_message.points, objects);
					server.join("OBJECTS", objects, output);
					break;
				case DELETE:
				case EXIT:
					rtree.deleteAll();
					server.join(server.client_message.command, objects, output);
					break;
				default:
					server.join("FAIL", objects, output);
					break;
				}

				server.sendData(output);
				
			}
			else {
				server.join("FAIL", objects, output);
				server.sendData(output);
			}

		} while (server.client_message.command != "EXIT");

		cout << "Client disconnected" << endl;
	}
	else {
		cout << "Client not connected" << endl;
		return 1;
	}

	return 0;
}
