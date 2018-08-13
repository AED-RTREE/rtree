//============================================================================
// Name        : main.cpp
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 1.0
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

typedef vector<pair<int, int>> ValueType;

struct Rect
{
	Rect() {}

	Rect(int a_minX, int a_minY, int a_maxX, int a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}


	int min[2];
	int max[2];
};

Rect MBR(ValueType pol)//el MBR que cubre al poligono
{
	int x1 = pol[0].first;
	int x2 = pol[0].first;
	int y1 = pol[0].second;
	int y2 = pol[0].second;
	if (pol.size() == 1) {//Si "poligono" es un punto
		x1 -= 5; x2 += 5; y1 -= 5; y2 += 5;
	}
	else {//si el poligono tiene mas puntos
		for (unsigned int i = 1; i<pol.size(); i++) {
			if (pol[i].first<x1) {
				x1 = pol[i].first;
			}
			if (x2<pol[i].first) {
				x2 = pol[i].first;
			}
			if (pol[i].second<y1) {
				y1 = pol[i].second;
			}
			if (y2<pol[i].second) {
				y2 = pol[i].second;
			}
		}
	}

	return Rect(x1, y1, x2, y2);
}

bool MySearchCallback(ValueType id)
{
	cout << "Hit data rect: " << "\n";
	for (auto point : id) {
		cout << "( " << point.first << ", " << point.second << "),";
	}
	cout << endl;
	return true; // keep going
}


int main(int argc, char *argv[])
{
	TcpServer server("localhost", 8080);
	RTree rtree;

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
				{
					Rect rect = MBR(server.client_message.points);
					rtree.Insert(rect.min, rect.max, server.client_message.points);
					rtree.getMBRs(objects);
					server.join("MBRS", objects, output);
					/*objects.push_back(server.client_message.points);
					server.join("MBRS", objects, output);
					cout << output << endl;*/
					break;
				}
				case RANGE:
				{
					rtree.Search(server.client_message.points[0], server.client_message.points[1], objects);
					server.join("OBJECTS", objects, output);
					break;
				}
				case NEAREST:
					rtree.nearest(server.client_message.k, server.client_message.points, objects);
					server.join("OBJECTS", objects, output);
					break;
				case DELETE:
				case EXIT:
				{
					rtree.RemoveAll();
					//rtree.deleteAll();
					server.join(server.client_message.command, objects, output);
					break;
				}
				default:
				{
					server.join("FAIL", objects, output);
					break;
				}
				}

				server.sendData(output);
				
			}
			else {
				server.join("FAIL", objects, output);
				server.sendData(output);
			}

		} while (server.client_message.command != "EXIT");
		rtree.RemoveAll();

		cout << "Client disconnected" << endl;
	}
	else {
		cout << "Client not connected" << endl;
		return 1;
	}

	return 0;
}
