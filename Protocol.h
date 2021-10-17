#pragma once
#define MAXNAMELEN  32

// Included  first in all messages 
enum MsgType {
	Join,                            // Client joining gam  e at server
	Leave,                            // Client leavinggame
	Change,                            // Information  to clients
	Event,                            // Information  fro m clients to server
	TextMessage                        // Send tex  t messages  to one   or all
};

enum ObjectDesc {
	Human,
	NonHuman,
	Vehicle,
	StaticObject
};

enum ObjectForm {
	Cube,
	Sphere,
	Pyramid,
	Cone
};

struct MsgHead {
	unsigned  int  length = 16;        // Total length for whole message 
	unsigned  int  seq_no = 0;      // Sequencenumber
	unsigned  int  id = 0;            // Client ID or 0; 
	MsgType  type;                    // Type of message
};

struct JoinMsg {
	MsgHead head;
	ObjectDesc desc;
	ObjectForm form;
	char name[MAXNAMELEN];            // nullterminated!,  or empty
};

struct Coordinate {
	int x = 0;                        // Set to zero so dir can be left unhandled
	int y = 0;
};

enum EventType {
	Move
};

// Included first in all Event messages
struct EventMsg {
	MsgHead head;
	EventType type;
};

enum ChangeType {
	NewPlayer,
	PlayerLeave,
	NewPlayerPosition
};

struct ChangeMsg {
	MsgHead head;
	ChangeType type;
};

struct NewPlayerMsg {
	ChangeMsg msg;            //Change message header with new client id
	ObjectDesc desc;
	ObjectForm form;
	char name[MAXNAMELEN]; // nullterminated!, or empty
};

struct NewPlayerPositionMsg {
	ChangeMsg msg; //Change message header
	Coordinate pos; //New object position
	Coordinate dir; //New object direction
};

struct MoveEvent {
	EventMsg event;
	Coordinate pos; //New object position
	Coordinate dir; //New object direction
};

struct LeaveMsg {
	MsgHead head;
};

struct PlayerLeaveMsg {
	ChangeMsg msg;
};