#pragma once

#include <string>
#include <sstream>
#include <map>
#include <queue>
#include <time.h>
#include <chrono>

#include "sys/socket.h"
#include "netinet/in.h"

#include "Emitter.h"


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
	std::chrono::time_point<std::chrono::high_resolution_clock> timeStamp;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTry;
	int numTries;
};

//prob should optimize thos.
struct SocketEvent {
	char* buffer;
	QueueItem* item; //not sure why item.
};

class UDPSocket : public Emitter<SocketEvent&> {
protected:
	int mSocket;
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
	bool Send(unsigned int, const char*, int, sockaddr_in*);
	bool Send(unsigned int, const char*, int, const char*, short);

	bool SendReliable(unsigned int, const char*, int, const char*, short);
	bool SendReliable(unsigned int, const char*, int, sockaddr_in*);
	void PollEvents(void);
	//PollEvents :P

};

//tcp->SendKeys()
//udp->SendMousePos();
//udp->SendKeys() untill its there. 