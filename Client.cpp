#include "Header.h"

//! Method for connecting to the server
void Client::connectToServer(std::string ipAddress, unsigned int port) {

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
    sockaddr_in address;
    std::string strAddress = ipAddress;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, strAddress.c_str(), &address.sin_addr);

    // Initialize socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Investigate connection
    int connected = SOCKET_ERROR;
    connected = connect(sock, (sockaddr*)&address, sizeof(address));
    if (connected != SOCKET_ERROR) {
        std::cout << "Connection successful" << std::endl;
    }
    else {
        std::cout << "Could not connect to server" << std::endl;
        return;
    }

    // Send join message
    JoinMsg jMsg{
        MsgHead { sizeof jMsg, 0, 0, Join },
        Human,
        Cube,
        "Chris"
    };
    send(sock, (char*)&jMsg, sizeof(jMsg), 0);

    // Receive server response
    char buffer[256] = { '\0*' };
    recv(sock, buffer, sizeof(buffer), 0);
    MsgHead response;
    memcpy(&response, buffer, buffer[0]);
    myHeader.id = response.id;

    std::cout << "Your player ID is: " << myHeader.id << std::endl;
    stopFlag = false;
    start();
}

void Client::start() {

    fd_set readset;
    timeval timeout;
    GuiSocket s;

    while (true) {
        if (stopFlag) {
            return;
        }
        char recvBuff[512] = { '\0' };

        FD_ZERO(&readset);
        FD_SET(sock, &readset);

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int input = select(NULL, &readset, NULL, NULL, &timeout);

        if (input == SOCKET_ERROR) {
            std::cout << "Socket error on select" << std::endl;
            return;
        }

        if (FD_ISSET(sock, &readset)) {
            recv(sock, recvBuff, sizeof(recvBuff), 0);
            MsgHead response;
            memcpy(&response, recvBuff, sizeof(response));
            switch (response.type) {
            case Change:
                ChangeMsg msg;
                memcpy(&msg, recvBuff, sizeof(msg));
                switch (msg.type) {
                case NewPlayer: {
                    NewPlayerMsg nmsg;
                    memcpy(&nmsg, recvBuff, sizeof(nmsg));
                    std::cout << "New player has joined the game with ID: " << response.id << std::endl;
                    s.redraw(response.id, pos);
                    break;
                }
                case PlayerLeave: {
                    PlayerLeaveMsg left;
                    memcpy(&left, recvBuff, sizeof(left));
                    std::cout << "Player with ID " << response.id << " has left the game" << std::endl;
                    break;
                }
                case NewPlayerPosition: {
                    NewPlayerPositionMsg newpos;
                    memcpy(&newpos, recvBuff, sizeof(newpos));
                    if (response.id == myHeader.id) {
                        pos.x = newpos.pos.x;
                        pos.y = newpos.pos.y;
                    }
                    break;
                }
                }
            }
        }
    }
}

int main() {
    Client c;
    c.connectToServer("130.240.40.25", 49154);
}