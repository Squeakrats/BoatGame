#pragma once
#include "winsock2.h"
#include "../events/Emitter.h"
#include <string>
#include <sstream>
#include <map>
#include <queue>
#include <time.h>
//prob should optimize thos.
struct SocketEvent {
	char* buffer;
	int numBytes;
	sockaddr_in address;
};

struct Packet {
	unsigned int eventId;
	char buffer[500];
};

struct ACK {
	unsigned int reliable = 1; //this is a lie.
	unsigned int packetId = 0;
	unsigned int eventId = 111;
};

struct QueueItem {
	unsigned int reliable;
	unsigned int packetId;
	unsigned int eventId;
	char buffer[500];
	sockaddr_in address;
	unsigned int numBytes;
	time_t timeStamp;
	time_t lastTry;
	int numTries;
};

class UDPSocket : public Emitter<SocketEvent&> {
protected:
	SOCKET mSocket;
	int mType, mProto;
	sockaddr_in mToAddr;
	Packet mBuffer;
	std::map<long int, std::queue<QueueItem*>> mReliableQueue;
	std::map<long int, unsigned int> mLastPacketRecieved;
	unsigned int mCurrentPacketId = 1;
public:
	UDPSocket(){};
	
	bool Init(void);
	bool SetBlocking(bool);
	bool Bind(unsigned int);
	bool Connect(const char*, unsigned int);
	//bool Send(const char*, int);
	bool Send(unsigned int, const char*, int, const char*, short);

	bool SendReliable(unsigned int, const char*, int, const char*, short);
	void PollEvents(void);
	//PollEvents :P

};

//tcp->SendKeys()
//udp->SendMousePos();
//udp->SendKeys() untill its there. 