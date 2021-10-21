#include "Header.h"

int main() {
	Client c;
	c.connectToServer("192.168.1.82", 5300);
}
//! Method for connecting to the server
void Client::connectToServer(std::string IP, unsigned int port) {

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
	std::string strAddress = IP;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, strAddress.c_str(), &address.sin_addr);

	// Initialize socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// Investigate connection
	int connected = SOCKET_ERROR;
	connected = connect(sock, (sockaddr*)&address, sizeof(address));
	if (connected != SOCKET_ERROR) {
		std::cout << "Connection successful, if you do not get assigned an ID, game has already started!" << std::endl;
	}
	else {
		std::cout << "Could not connect to server" << std::endl;
		return;
	}

	// Create joinMsg
	JoinMsg jMsg{
		MsgHead { sizeof jMsg, 0, 0, Join },
		Human,
		Cube,
		"Ame"
	};
	send(sock, (char*)&jMsg, sizeof(jMsg), 0);

	// Receive server response from joinMsg
	char buffer[512] = { '\0' };
	recv(sock, buffer, sizeof(buffer), 0);
	MsgHead JoinMsgResponse;
	memcpy(&JoinMsgResponse, buffer, sizeof(JoinMsgResponse));
	MsgToServer.id = JoinMsgResponse.id;

	std::cout << "Your player ID is: " << MsgToServer.id << std::endl;
	stop = false;
	start();
}

void Client::start() {

	fd_set readset;
	timeval timeout;
	GuiSocket s;

	while (true) {
		if (stop) {
			return;
		}
		char recvBuff[512] = { '\0' };

		FD_ZERO(&readset);
		FD_SET(sock, &readset);

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		int input = select(NULL, &readset, NULL, NULL, &timeout);

		if (input == SOCKET_ERROR) {
			return;
		}

		if (FD_ISSET(sock, &readset)) {
			recv(sock, recvBuff, sizeof(recvBuff), 0);
			MsgHead receivedMsgHead;
			memcpy(&receivedMsgHead, recvBuff, sizeof(receivedMsgHead));
			if (receivedMsgHead.type == Change) {
				ChangeMsg msg;
				memcpy(&msg, recvBuff, sizeof(msg));
				if (msg.type == NewPlayer) {
					NewPlayerMsg nmsg;
					memcpy(&nmsg, recvBuff, sizeof(nmsg));
					std::cout << "New player has joined the game with ID: " << receivedMsgHead.id << std::endl;

				}
				if (msg.type == PlayerLeave) {
					PlayerLeaveMsg left;
					memcpy(&left, recvBuff, sizeof(left));
					std::cout << "The game has ended due to Player with ID " << receivedMsgHead.id << " has left the game" << std::endl;
					WSACleanup();
					closesocket(sock);
				}
				if (msg.type == NewPlayerPosition) {
					NewPlayerPositionMsg newpos;
					memcpy(&newpos, recvBuff, sizeof(newpos));
					if (receivedMsgHead.id == MsgToServer.id) {
						pos.x = newpos.pos.x;
						pos.y = newpos.pos.y;
						std::cout << "Your new position is X: " << pos.x
							<< " Y: " << pos.y << std::endl;
					}
					else {
						std::cout << "Client " << receivedMsgHead.id << ":s position is X: " <<
							newpos.pos.x << " Y: " << newpos.pos.y << std::endl;
					}
					std::cout << receivedMsgHead.seq_no << std::endl;
					s.redraw(receivedMsgHead.id, newpos.pos);
					InputKeys = true;
				}

			}
		}
		// Check for client input if the game has started
		if (InputKeys) {
			if (GetAsyncKeyState('W') < 0 && wheld == false)
			{
				pos.y++;
				move(pos);
				wheld = true;
			}
			if (GetAsyncKeyState('W') == 0 && wheld == true)
			{
				wheld = false;
			}
			if (GetAsyncKeyState('A') < 0 && aheld == false)
			{
				pos.x--;
				move(pos);
				aheld = true;
			}
			if (GetAsyncKeyState('A') == 0 && aheld == true)
			{
				aheld = false;
			}
			if (GetAsyncKeyState('S') < 0 && sheld == false)
			{
				pos.y--;
				move(pos);
				sheld = true;
			}
			if (GetAsyncKeyState('S') == 0 && sheld == true)
			{
				sheld = false;
			}

			if (GetAsyncKeyState('D') < 0 && dheld == false)
			{
				pos.x++;
				move(pos);
				dheld = true;
			}
			if (GetAsyncKeyState('D') == 0 && dheld == true)
			{
				dheld = false;
			}
			if (GetAsyncKeyState('L') & 1) {
				stop = true;
				leave();
			}
		}
	}
}
//creates a move message witht hep of MsgToServer header
void Client::move(Coordinate MovetoPos) {
	MsgToServer.type = Event;

	EventMsg eveMsg;
	eveMsg.type = Move;
	eveMsg.head = MsgToServer;

	MoveEvent movEve;
	movEve.event = eveMsg;
	movEve.pos = MovetoPos;


	send(sock, (char*)&movEve, sizeof(movEve), 0);

}
//creates a leave message witht hep of MsgToServer header
void Client::leave() {
	MsgToServer.type = Leave;

	LeaveMsg bye;
	bye.head = MsgToServer;

	std::cout << "You have left the game" << std::endl;
	send(sock, (char*)&bye, sizeof(bye), 0);

	//close socket
	WSACleanup();
	closesocket(sock);
	return;

}
