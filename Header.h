#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "Protocol.h"

#define SCK_version 2 0x0202

//! The client class communicates with the server and performs all necessary actions to play the game according to the provided game protocol
class Client {
private:
	//! The default client header included in all sent messages
	MsgHead MsgToServer;
	//! The TCP client socket
	SOCKET sock;
	//! Variable used to store a clients latest coordinate 
	Coordinate pos;

	//Booleans to check for stop
	bool InputKeys =false;
	bool stop;
	bool wheld=false;
	bool aheld = false;
	bool sheld = false;
	bool dheld = false;


	void start();
	void leave();
	void move(Coordinate MovetoPos);
public:
	void connectToServer(std::string IP, unsigned int port);
};
class GuiSocket {
private:
	sockaddr_in myaddr;
	SOCKET sockGUI;
public:
	void redraw(int id, Coordinate c);
	GuiSocket();
};

