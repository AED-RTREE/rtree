//============================================================================
// Name        : TcpServer.h
// Author      : Daniel Palomino
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
// Description : RTree Backend
// Created on  : 31 jul. 2018
//============================================================================

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include "utils.h"

/*
TRAMA:

QUERIES:
* INSERT|1|100|230|END
* RANGE|2|120|360|520|450|END
* NEAREST|K|1|120|320|END
* DELETE|0|END

RESPONSES:
* DELETE|0|END
* SUCCESS|0|END
* FAIL|0|END
* EXIT|0|END

FORMAT:

COMMAND|N|X1|Y1|X2|Y2|END

COMMAND: Comando a utilizar en el rtree.
N: Numero de Coordenadas siguientes.
(X1,Y1): Primera coordenada.
END: Flag finalizador.

------------------------------------------
* OBJECTS|2|1|12|32|2|65|256|23|65|END

FORMAT:

COMMAND|K|N1|X1|Y1|...|N2|X1|Y1|...|END

COMMAND: Comando a utilizar en el rtree.
K: Numero de objetos siguientes
N1: Numero de Coordenadas del objeto siguiente.
(X1,Y1): Primera coordenada.

------------------------------------------

* MBRS|2|2|2|120|360|520|450|2|120|360|520|450|1|2|12|32|14|22|END

FORMAT:

COMMAND|K|M1|N1|X1|Y1|...|N2|X1|Y1|...|END

COMMAND: Comando a utilizar en el rtree.
K: Numero de nivels.
M1: Numero de MBRs siguientes del primer nivel.
N1=2: Numero de Coordenadas del MBR siguiente.
((X1,Y1),(X2,Y2)): Primer MBR.

*/

using namespace std;

class TcpServer
{
private:
	int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;

public:
	struct message client_message;

public:
	TcpServer(string host, uint16_t port);
	bool bindSocket();
	void listenToClient();
	bool acceptConnection();
	bool sendData(string data);
	bool receiveData(int size);
	void split(const string& s, char delimiter, message& msg);
	void join(const string& command, vector<vector<vector<pair<int, int>>>>& objects_n, string& output);
};

#endif