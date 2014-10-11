#include "UDPSocket.h"
#include "Ws2tcpip.h"
#include <iostream>
#include "stdlib.h"
bool UDPSocket::Init(void) {
	mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mSocket < 0){
		printf("Failed To Create Socket\n");
		return false;
	}
	return true;
}

bool UDPSocket::Bind(unsigned int port) {
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(mSocket, (const sockaddr*) &addr, sizeof(sockaddr_in)) < 0){
		std::cout << WSAGetLastError() << std::endl;
		printf("Failed To Bind Socket");
		return false;
	}
	return true;
}

bool UDPSocket::Connect(const char* ip, unsigned int port) {
	mToAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(mToAddr.sin_addr));
	mToAddr.sin_port = htons(port);
	if (connect(mSocket, (const sockaddr*) &mToAddr, sizeof(sockaddr_in)) < 0) {
		printf("Faild To Connect\n");
		std::cout << WSAGetLastError() << std::endl;
	}
	return true;
}

bool UDPSocket::SetBlocking(bool blocking) {
	unsigned long async = (blocking) ? 0 : 1;
	if (ioctlsocket(mSocket, FIONBIO, &async) < 0){
		printf("Faild To Set Non-Blocking Mode\n");
		return false;
	}
	return true;
}
/*
bool UDPSocket::Send(const char* message, int messageLen) {
	send(mSocket, message, messageLen, 0);
	return true;// this could be wrong? ;/
}
*/

//optimize a lot of this stuff. 
bool UDPSocket::Send(unsigned int eventId, const char* message, int messageLen, const char* ip, short port) {
	QueueItem item;
	item.reliable = 0;
	item.packetId = 0;
	item.eventId = eventId;
	memcpy(item.buffer, message, messageLen);
	item.numBytes = messageLen;

	item.address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(item.address.sin_addr));
	item.address.sin_port = htons(port);
	sendto(mSocket, (const char*) &item, sizeof(unsigned int) * 3 + item.numBytes, 0, (sockaddr*) &item.address, sizeof(item.address));
	return true;
}

//silly approach not sending data untill first bit of data is recieved. Silly. fine for now. 
bool UDPSocket::SendReliable(unsigned int eventId, const char* message, int messageLen, const char* ip, short port) {
	QueueItem* item = new QueueItem();
	item->reliable = 1;
	item->packetId = mCurrentPacketId++;
	item->eventId = 0;
	memcpy(item->buffer, message, messageLen);
	item->numBytes = messageLen;
	item->timeStamp = time(0);
	//new UDPSocket<udp::reliable
	
	item->address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(item->address.sin_addr));
	item->address.sin_port = htons(port);

	if (!mReliableQueue[item->address.sin_addr.s_addr].size()){
		item->lastTry = item->timeStamp;
		item->numTries = 1;
		sendto(mSocket, (const char*) item, sizeof(unsigned int) * 3 + item->numBytes, 0, (sockaddr*) &item->address, sizeof(item->address));
	}
	mReliableQueue[item->address.sin_addr.s_addr].push(item);
	return true;
}

void UDPSocket::PollEvents(void) {
	QueueItem item;
	sockaddr_in sender;
	int senderSize = sizeof(sender);

	while (1){
		int numBytes = recvfrom(mSocket, (char*) &item, sizeof(QueueItem), 0, (sockaddr*)&sender, &senderSize);
		if (numBytes <= 0){
			break;
		}
		item.numBytes = numBytes - sizeof(unsigned int) * 3;

		if (!item.reliable){
			SocketEvent event = { item.buffer, item.numBytes, sender };
			Emit(item.eventId, event);
		}
		else{
			if (item.eventId == 111){
				QueueItem* front =  mReliableQueue[sender.sin_addr.s_addr].front();
				if (item.packetId == front->packetId){
					mReliableQueue[sender.sin_addr.s_addr].pop();
					//printf("ACK: %u\n", item.packetId);
					delete front;
				}
				
			}
			else{
				ACK ack;
				ack.packetId = item.packetId;
				sendto(mSocket, (const char*) &ack, sizeof(ack), 0, (const sockaddr*) &sender, sizeof(sender));
				if (item.packetId > mLastPacketRecieved[sender.sin_addr.s_addr]){
					//std::cout << item.packetId << std::endl;
					mLastPacketRecieved[sender.sin_addr.s_addr] = item.packetId;
					SocketEvent event = { item.buffer, item.numBytes, sender };
					Emit(item.eventId, event);
				}
				
			}
			
		}

	}

	//get first item in queue
	for (auto itr : mReliableQueue){
		if (itr.second.size()){
			QueueItem* item = itr.second.front();
			if (item->numTries == 0 || difftime(time(0), item->lastTry)){
				item->lastTry = time(0);
				item->numTries +=1;
				sendto(mSocket, (const char*) item, sizeof(unsigned int) * 3 + item->numBytes, 0, (sockaddr*) &item->address, sizeof(item->address));
			}

			if (item->numTries > 10){
				printf("I think we failed to send this guy. ABORT ABORT ABORT. :(\n");
			}


		}
	}
}
