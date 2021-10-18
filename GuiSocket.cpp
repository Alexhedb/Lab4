#include "Header.h"
#include <string> 
GuiSocket::GuiSocket() {


	// Assign socket
	sockGUI = socket(AF_INET, SOCK_DGRAM, 0);

	// Startup Winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wSok = WSAStartup(version, &data);

	// Investigate Winsock
	if (wSok != 0) {
		std::cout << "Can't start Winsock! " << std::endl;
		return;
	}

	// Set hint structure for the server
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(5300);

	inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);

}

void GuiSocket::redraw(int id, Coordinate c) {
	int x = c.x + 100;
	int y = (c.y * -1) + 100;
    std::string datapackage = "PAINT;"+std::to_string(id) + ";" + std::to_string(y) + ";" + std::to_string(x) + ";";
 
    int sendPkg = sendto(sockGUI, datapackage.c_str(), datapackage.size(), 0, (sockaddr*)&myaddr, sizeof(myaddr));
 

    if (sendPkg == SOCKET_ERROR) {
        std::cout << "Error: " << WSAGetLastError() << "\n";
    }
}
