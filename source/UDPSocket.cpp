#include "UDPSocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include "stdlib.h"
#include <string.h>//watwatwat


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
		//std::cout << WSAGetLastError() << std::endl;
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
		//std::cout << WSAGetLastError() << std::endl;
	}
	return true;
}

bool UDPSocket::SetBlocking(bool blocking) {
	unsigned long async = (blocking) ? 0 : 1;
	if (fcntl(mSocket, F_SETFL, O_NONBLOCK) < 0){
		printf("Faild To Set Non-Blocking Mode\n");
		return false;
	}
	return true;
}

bool UDPSocket::Send(unsigned int eventId, const char* message, int messageLen, sockaddr_in* address) {
	QueueItem item;
	item.reliable = 0;
	item.packetId = mCurrentPacketId++;//this maybe. not sure
	item.eventId = eventId;
	memcpy(item.buffer, message, messageLen);
	item.numBytes = messageLen;
	item.address = *address;
	sendto(mSocket, (const char*) &item, sizeof(unsigned int) * 3 + item.numBytes, 0, (sockaddr*) &item.address, sizeof(item.address));
	return true;
}

bool UDPSocket::SendReliable(unsigned int eventId, const char* message, int messageLen, sockaddr_in* address) {
	QueueItem* item = new QueueItem();
	item->reliable = 1;
	item->packetId = mCurrentPacketId++;
	item->eventId = eventId;
	memcpy(item->buffer, message, messageLen);
	item->numBytes = messageLen;
	item->timeStamp = std::chrono::high_resolution_clock::now();	
	item->address = *address;

	if (!mReliableQueue[item->address.sin_addr.s_addr].size()){
		item->lastTry = item->timeStamp;
		item->numTries = 1;
		sendto(mSocket, (const char*) item, sizeof(unsigned int) * 3 + item->numBytes, 0, (sockaddr*) &item->address, sizeof(item->address));
	}
	mReliableQueue[item->address.sin_addr.s_addr].push(item);
	return true;
}


//optimize a lot of this stuff. 
bool UDPSocket::Send(unsigned int eventId, const char* message, int messageLen, const char* ip, short port) {
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &address.sin_addr);
	return Send(eventId, message, messageLen, &address);;
}


//silly approach not sending data untill first bit of data is recieved. Silly. fine for now. 
bool UDPSocket::SendReliable(unsigned int eventId, const char* message, int messageLen, const char* ip, short port) {
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &address.sin_addr);
	return SendReliable(eventId, message, messageLen, &address);
}


void UDPSocket::PollEvents(void) {
	QueueItem item;
	unsigned int senderSize = sizeof(sockaddr_in);

	while (1){
		int numBytes = recvfrom(mSocket, (char*) &item, sizeof(QueueItem), 0, (sockaddr*)&item.address, &senderSize);
		if (numBytes <= 0){
			break;
		}
		//std::cout << "Got Data!" << std::endl;
		item.numBytes = numBytes - sizeof(unsigned int) * 3;

		if (!item.reliable){
			SocketEvent event = { item.buffer,  &item };
			Emit(item.eventId, event);
		}
		else{
			if (item.eventId == 111){
				QueueItem* front =  mReliableQueue[item.address.sin_addr.s_addr].front();
				if (item.packetId == front->packetId){
					mReliableQueue[item.address.sin_addr.s_addr].pop();
					delete front;
				}
				
			}
			else{
				ACK ack;
				ack.packetId = item.packetId;
				sendto(mSocket, (const char*) &ack, sizeof(ack), 0, (const sockaddr*) &item.address, senderSize);
				if (item.packetId > mLastPacketRecieved[item.address.sin_addr.s_addr]){
					//std::cout << item.packetId << std::endl;
					mLastPacketRecieved[item.address.sin_addr.s_addr] = item.packetId;
					SocketEvent event = { item.buffer, &item };
					Emit(item.eventId, event);
				}
				
			}
			
		}

	}

	//get first item in queue
	for (auto itr : mReliableQueue){
		if (itr.second.size()){
			QueueItem* item = itr.second.front();
			auto now = std::chrono::high_resolution_clock::now();
			if (item->numTries == 0 || std::chrono::duration_cast<std::chrono::milliseconds>(now - item->lastTry).count() > 400){
				std::cout << "Failed to send packet, trying again...." << std::endl;
				item->lastTry = now;
				item->numTries +=1;
				sendto(mSocket, (const char*) item, sizeof(unsigned int) * 3 + item->numBytes, 0, (sockaddr*) &item->address, sizeof(item->address));
			}

			if (item->numTries > 10){
				printf("Lost Connection To Server. :(\n");
				exit(1);
			}


		}
	}
}
