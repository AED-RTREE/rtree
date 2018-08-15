//============================================================================
// Name        : main.cpp
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
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
	return true;
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
			vector<vector<vector<pair<int, int>>>> objects_n;
			vector<vector<pair<int, int>>> objects;

			if (server.receiveData(1024)) {
				server.join("SUCCESS", objects_n, output);
				server.sendData(output);

				switch (str2cmd(server.client_message.command))
				{
				case INSERT:
				{
					Rect rect = MBR(server.client_message.points);
					rtree.Insert(rect.m_min, rect.m_max, server.client_message.points);
					rtree.getMBRs(objects_n);
					server.join("MBRS", objects_n, output);
					break;
				}
				case RANGE:
				{
					rtree.Search(server.client_message.points[0], server.client_message.points[1], objects);
					objects_n.push_back(objects);
					server.join("OBJECTS", objects_n, output);
					break;
				}
				case NEAREST:
					rtree.nearest(server.client_message.k, server.client_message.points, objects);
					objects_n.push_back(objects);
					server.join("OBJECTS", objects_n, output);
					break;
				case DELETE:
				case EXIT:
				{
					rtree.RemoveAll();
					server.join(server.client_message.command, objects_n, output);
					break;
				}
				default:
				{
					server.join("FAIL", objects_n, output);
					break;
				}
				}

				server.sendData(output);
				
			}
			else {
				server.join("FAIL", objects_n, output);
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
