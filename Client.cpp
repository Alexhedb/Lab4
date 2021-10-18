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
        "Vampire"
    };
    send(sock, (char*)&jMsg, sizeof(jMsg), 0);

    // Receive server response
    char buffer[256] = { '\0*' };
    recv(sock, buffer, sizeof(buffer), 0);
    MsgHead JoinMsgResponse;
    memcpy(&JoinMsgResponse, buffer, buffer[0]);
    response.id=JoinMsgResponse.id;

    std::cout << "Your player ID is: " << response.id << std::endl;
    stopFlag = false;
    start();
}

void Client::start(){

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
            std::cout << "select() failed with error:" << WSAGetLastError() << std::endl;
            return;
        }
        
        if (FD_ISSET(sock, &readset)) {
            recv(sock, recvBuff, sizeof(recvBuff), 0);
            MsgHead receivedMsgHead;
            memcpy(&receivedMsgHead, recvBuff, sizeof(receivedMsgHead));
            switch (receivedMsgHead.type) {
            case Change:
                ChangeMsg msg;
                memcpy(&msg, recvBuff, sizeof(msg));
                switch (msg.type) {
                case NewPlayer: {
                    NewPlayerMsg nmsg;
                    memcpy(&nmsg, recvBuff, sizeof(nmsg));
                    std::cout << "New player has joined the game with ID: " << receivedMsgHead.id << std::endl;
                    break;
                }
                case PlayerLeave: {
                    PlayerLeaveMsg left;
                    memcpy(&left, recvBuff, sizeof(left));
                    std::cout << "Player with ID " << receivedMsgHead.id << " has left the game" << std::endl;
                    break;
                }
                case NewPlayerPosition: {
                    NewPlayerPositionMsg newpos;
                    memcpy(&newpos, recvBuff, sizeof(newpos));
                    if (receivedMsgHead.id == response.id) {
                        pos.x = newpos.pos.x;
                        pos.y = newpos.pos.y;
                        std::cout << "Your new position is X: " << pos.x
                            << " Y: " << pos.y << std::endl;
                    }
                    else {
                        std::cout << "Client " << receivedMsgHead.id << ":s position is X: " <<
                            newpos.pos.x << " Y: " << newpos.pos.y << std::endl;
                    }
                    s.redraw(receivedMsgHead.id, newpos.pos);
                    positionSet = true;
                    break;
                  }
                }
            }
        }
        // Check for client input if the game has started
        if (positionSet == true) {
            if (GetAsyncKeyState('W') & 1) {
                pos.y++;
                move(pos);
            }
            if (GetAsyncKeyState('A') & 1) {
                pos.x--;
                move(pos);
            }
            if (GetAsyncKeyState('S') & 1) {
                pos.y--;
                move(pos);
            }
            if (GetAsyncKeyState('D') & 1) {
                pos.x++;
                move(pos);
            }
            if (GetAsyncKeyState('L') & 1) {
                leave();
            }
        }
    }
}

    
void Client::move(Coordinate MovetoPos) {

    // Set client head
    response.type = Event;

    // Create EventMsg
    EventMsg eMsg;
    eMsg.head = response;
    eMsg.type = Move;

    // Create MoveEvent
    MoveEvent mEve;
    mEve.event = eMsg;
    mEve.pos = MovetoPos;

    // Send to server
    send(sock, (char*)&mEve, sizeof(mEve), 0);

}

void Client::leave() {
    // Set client head
    response.type = Leave;

    LeaveMsg bye;
    bye.head = response;

    // Send to server
    send(sock, (char*)&bye, sizeof(bye), 0);

    Client::~Client();

    stopFlag = true;

}

Client::~Client() {
    WSACleanup();
    closesocket(sock);
    return;
}
int main() {
    Client c;
    c.connectToServer("192.168.1.126", 5400);
}