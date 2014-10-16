#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include "string.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <arpa/inet.h>


#include "Actor.h"
#include "SideViewActor.h"
#include "UDPSocket.h"


const int MOVING_FORWARD = (1 << 0);
const int TURNING_LEFT = (1 << 1);
const int TURNING_RIGHT = (1 << 2);

struct GameUpdateActor {
	unsigned int actorId;
	float x, y, z;
	float rx, ry, rz;
};


StrongSideViewActorPtr actors[2];
int actorStates[2]; //this should prob be part of the recieving controller class?
std::map<long, sockaddr_in> addresses;
void handleSocketInput(SocketEvent& event){
	int state;// = (int) event.buffer;
	memcpy(&state, event.buffer, sizeof(state));
	StrongSideViewActorPtr& actor = actors[0];//(ntohs(event.address.sin_port) == 1337) ? actors[0] : actors[1];
	//this needs to discard if its an older packet tho. srsly :/ er wait idk, 
	actorStates[0] = state;
}

void OnClientConnect(SocketEvent& event){
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &event.item->address.sin_addr, ip, INET_ADDRSTRLEN);
	addresses[event.item->address.sin_addr.s_addr] = event.item->address;
	std::cout << "Client Connected, IP: " <<  ip << " Port: " << ntohs(event.item->address.sin_port) <<  std::endl;
}
/*
UDPSocket needs to get cleaned up a lot
1) culling for timestamps > lastaccepted Timestamp
2) fix the mess with double sends/SendReliable 
3) make event loop the same timing system. 
4) create a connect message. 
*/
int main(int argc, char*argv[]) {

	int bindTo = 1337;
	if(argc > 1){
		bindTo = atoi(argv[1]);
	}

	printf("Listening on port %i\n", bindTo);
	

	UDPSocket* udp = new UDPSocket();
	udp->Init();
	udp->SetBlocking(false);
	udp->Bind(bindTo);
	udp->On(0, handleSocketInput);
	udp->On(1, OnClientConnect);

	for (int i = 0; i < 2; i++){
		actors[i] = StrongSideViewActorPtr(new SideViewActor());
		actors[i]->SetId(i);
	}

	auto last = std::chrono::high_resolution_clock::now();
	while (1){
		udp->PollEvents();

		auto now = std::chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

		if (dt.count() > 17) { //temo run server and double fps
			last = now;

			for (int i = 0; i < 2; i++){
				StrongSideViewActorPtr& actor = actors[i];
				int actorState = actorStates[i];
				if (actor != nullptr){
					if(actorState & MOVING_FORWARD){
						actor->MoveForward(2);
					}
					if(actorState & TURNING_LEFT) {
						actor->RotateZ(.2);
					}
					if(actorState & TURNING_RIGHT) {
						actor->RotateZ(-.2);
					}
				}
			}
			


			//SEND THE DAMN GAME STATE PLEASE. 
			char buffer[500];
			unsigned int numActors = 2;
			memcpy(buffer, &numActors, sizeof(numActors));

			GameUpdateActor actorStruct;
			for (int i = 0; i < numActors; i++){
				glm::vec3 actorPosition = actors[i]->GetPosition();
				glm::vec3 actorRotation = actors[i]->GetRotation();
				actorStruct.actorId = actors[i]->GetId();
				actorStruct.x = actorPosition.x;
				actorStruct.y = actorPosition.y;
				actorStruct.z = actorPosition.z;
				actorStruct.rx = actorRotation.x;
				actorStruct.ry = actorRotation.y;
				actorStruct.rz = actorRotation.z;	
				memcpy(buffer + sizeof(numActors) + sizeof(GameUpdateActor) * i, &actorStruct, sizeof(GameUpdateActor));

			}

			for(auto kv : addresses){
				udp->Send(0, buffer, sizeof(unsigned int) * numActors * sizeof(GameUpdateActor), &kv.second);
			}
			//generic send to these 2 places. 
			//udp->Send(0, buffer, sizeof(unsigned int) +numActors * sizeof(GameUpdateActor), "24.240.35.26", sendTo);
			//udp->Send(0, buffer, sizeof(unsigned int) +numActors * sizeof(GameUpdateActor), "127.0.0.1", sendTo);
			//std::cout << actorRotation.x << actorRotation.y << actorRotation.z << std::endl;
		}
	}
}