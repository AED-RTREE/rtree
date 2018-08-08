//============================================================================
// Name        : TcpServer.cpp
// Author      : Daniel Palomino
// Version     : 0.1
// Copyright   : Your copyright notice
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

void TcpServer::split(const string& s, char delimiter, string& command, vector<pair<int, int>>& tokens) {
	string token;
	istringstream tokenStream(s);
	int x, y;

	getline(tokenStream, token, delimiter);
	command.resize(0);
	command = token;

	getline(tokenStream, token, delimiter);
	int last = stoi(token);
	for (int i = 0; i < last; i++) {
		getline(tokenStream, token, delimiter);
		x = stoi(token);
		getline(tokenStream, token, delimiter);
		y = stoi(token);
		tokens.push_back(make_pair(x, y));
	}

	return;
}

void TcpServer::join(const string& command, vector<vector<pair<int, int>>>& objects, string& output) {
	output = command;

	switch (str2cmd[command])
	{
	case SUCCESS:

		output += "|0";
		break;

	case OBJECTS:
	case MBR:

		output += "|" + to_string(objects.size());
		for (auto& polygon : objects) {
			output += "|" + to_string(polygon.size());

			for (auto& point : polygon) {
				output += "|" + to_string(point.first) + "|" + to_string(point.second);
			}
		}
		break;

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
	if (write(client_sock, data.c_str(), data.length()) != 0) {
		return false;
	}

	return true;
}

bool TcpServer::receiveData(int size = 512)
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

	split(buffer, '|', client_message.command, client_message.points);

	return true;
}