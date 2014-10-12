#include "UDPSocket.h"
#include <iostream>
#include <string>
#include <chrono>
void function1(SocketEvent& event) {
	std::cout << "got an event!" << std::endl;
}
int main(int argc, char*argv[]){
	int port;
	printf("enter port to bind to\n");
	std::cin >> port;

	printf("enter the destination IP\n");
	std::string destIp;
	std::cin >> destIp;
	printf("enter the destination Port\n");
	int destPort;
	std::cin >> destPort;

	UDPSocket* Socket = new UDPSocket();
	Socket->Init();
	Socket->Bind(port);
	Socket->SetBlocking(false);
	printf("listening on port: %i \n", port);
	std::cout << "Spamming " << destIp << ":" << destPort << std::endl;
	Socket->On(0, function1);
	auto last = std::chrono::high_resolution_clock::now();
	while(1){
		auto now = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

		if(ms.count() < 500){
			Socket->Send(0, "hello", sizeof("hello"), destIp.c_str(), destPort);
			last = now;
		}
		Socket->PollEvents();
		
	}
}