#include "UDPSocket.h"
#include <iostream>
#include <string>
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
	while(1){
		Socket->PollEvents();
		Socket->Send(0, "hello", sizeof("hello"), destIp.c_str(), destPort);
	}
}