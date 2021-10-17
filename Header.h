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
	MsgHead myHeader;
	//! The TCP client socket
	SOCKET sock;
	//! Tracks if the client has been given a position
	bool ready;
	//! Variable used to store a clients latest coordinate 
	Coordinate pos;
	//! Boolean stop flag used to terminate the play loop
	bool stopFlag;
	void start();
public:
	void connectToServer(std::string ipAddress, unsigned int port);
	class GuiSocket {
	private:
		sockaddr_in myaddr;
		SOCKET sockGUI;
	public:
		void redraw(int id, Coordinate c);
		void ToGUI();
	};

