//============================================================================
// Name        : TcpServer.cpp
// Author      : Daniel Palomino
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
// Description : RTree Backend
// Created on  : 31 jul. 2018
//============================================================================

#include "TcpServer.h"

TcpServer::TcpServer(string host, uint16_t port)
{
	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		cout << "Could not create socket" << endl;
	}
	cout << "Socket created in host: " << host << ", port: " << port << endl;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
}

void TcpServer::split(const string& s, char delimiter, message& msg) {
	string token;
	istringstream tokenStream(s);
	int x, y;
	msg.command.resize(0);
	msg.points.clear();
	msg.k = -1;

	getline(tokenStream, token, delimiter);
	msg.command = token;

	if (msg.command == "NEAREST") {
		getline(tokenStream, token, delimiter);
		msg.k = stoi(token);
	}

	getline(tokenStream, token, delimiter);
	int num_points = stoi(token);
	for (int i = 0; i < num_points; i++) {
		getline(tokenStream, token, delimiter);
		x = stoi(token);
		getline(tokenStream, token, delimiter);
		y = stoi(token);
		msg.points.push_back(make_pair(x, y));
	}

	return;
}

void TcpServer::join(const string& command, vector<vector<pair<int, int>>>& objects, string& output) {
	output.resize(0);
	output = command;

	switch (str2cmd(command))
	{
	case OBJECTS:
	case MBRS:

		output += "|" + to_string(objects.size());
		for (auto& polygon : objects) {
			output += "|" + to_string(polygon.size());

			for (auto& point : polygon) {
				output += "|" + to_string(point.first) + "|" + to_string(point.second);
			}
		}
		break;

	case DELETE:
	case SUCCESS:
	case FAIL:
	case EXIT:
	default:
		output += "|0";
		break;
	}

	output += "|END";
}

bool TcpServer::bindSocket()
{
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		cerr << "bind failed. Error" << endl;
		return false;
	}

	cout << "bind done" << endl;
	return true;
}

void TcpServer::listenToClient()
{
	listen(socket_desc, 3);

	cout << "Waiting for incoming connections..." << endl;
}

bool TcpServer::acceptConnection()
{
	c = sizeof(struct sockaddr_in);

	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		cerr << "accept failed" << endl;
		return false;
	}

	cout << "Connection accepted" << endl;
	return true;
}

bool TcpServer::sendData(string data)
{
	fflush(stdout);
	if (write(client_sock, data.c_str(), data.length()+1) != 0) {
		return false;
	}

	return true;
}

bool TcpServer::receiveData(int size = 1024)
{
	char buffer[size];

	read_size = read(client_sock, buffer, sizeof(buffer));

	if (read_size == 0)
	{
		cout << "Client disconnected" << endl;
		return false;
	}
	else if (read_size == -1)
	{
		cerr << "recv failed" << endl;
		return false;
	}

	split(buffer, '|', client_message);

	return true;
}