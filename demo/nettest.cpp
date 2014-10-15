#include "UDPSocket.h"
#include <iostream>
#include <string>
#include <chrono>
void function1(SocketEvent& event) {
	std::cout << "got an event!" << std::endl;
}

int main(int argc, char*argv[]){
	if(argc < 4){
		printf("use it right u dummie\n");
		exit(1);
	}
	int srcPort = atoi(argv[1]);
	char* destIp = argv[2];
	int destPort = atoi(argv[3]);

	UDPSocket* Socket = new UDPSocket();
	Socket->Init();
	Socket->Bind(srcPort);
	Socket->SetBlocking(false);
	Socket->On(0, function1);

	auto last = std::chrono::high_resolution_clock::now();
	while(1){
		auto now = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

		if(ms.count() > 2000){
			Socket->Send(0, "hello", sizeof("hello"), destIp, destPort);
			last = now;
		}
		Socket->PollEvents();
		
	}
}