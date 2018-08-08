#include <string>
#include <vector>
#include <iostream>
#include "tcp_server.h"

using namespace std;

int main(int argc, char *argv[])
{
	TcpServer server("localhost", 8080);
	if (server.bindSocket()) {
		server.listenToClient();
	}
	else {
		cout << "Server not created" << endl;
		return 1;
	}

	if (server.acceptConnection()) {
		do {
			server.receiveData(512);

			string output;
			vector<vector<pair<int, int>>> objects;
			objects.push_back(server.client_message.points);
			server.join(server.client_message.command, objects, output);
			cout << output << endl;
			server.sendData(output);
		} while (server.client_message.command != "EXIT");

		cout << "Client disconnected" << endl;
	}
	else {
		cout << "Client not connected" << endl;
		return 1;
	}

	return 0;
}
