#include "Header.h"
#include <string> 
void GuiSocket::ToGUI() {

    sockGUI = socket(AF_INET, SOCK_STREAM, 0);


    // Initialize winsock
    WSADATA wsaData;
    WORD ver = MAKEWORD(2, 2);
    int wsok = WSAStartup(ver, &wsaData);

    // Investigate WSAStartup
    if (wsok != 0) {
        std::cout << "Error! could not start WSA" << std::endl;
        return;
    }

    // Configure address
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(9876);
    inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);
}
void GuiSocket::redraw(int id, Coordinate c) {
    auto colorID = std::to_string(id);
    auto xcord = std::to_string(c.x);
    auto ycord = std::to_string(c.y);
    std::string datapackage("PAINT;" + colorID + ";" + xcord + ";" + ycord + ";");
    std::cout << datapackage << std::endl;


    int sendPkg = sendto(sockGUI, (const char*)&datapackage, sizeof(datapackage), 0, (sockaddr*)&myaddr, sizeof(myaddr));

    if (sendPkg == SOCKET_ERROR) {
        std::cout << "Could not send string to GUI!" << std::endl;
    }
}
