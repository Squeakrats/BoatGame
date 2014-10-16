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


const int SET_MOVING_FORWARD_TRUE = (1 << 0);
const int SET_MOVING_FORWARD_FALSE = (1 << 1);

const int SET_TURNING_LEFT_TRUE = (1 << 2);
const int SET_TURNING_LEFT_FALSE= (1 << 3);

const int SET_TURNING_RIGHT_TRUE = (1 << 4);
const int SET_TURNING_RIGHT_FALSE = (1 << 5);


struct GameUpdateActor {
	unsigned int actorId;
	float x, y, z;
	float rx, ry, rz;
};


StrongSideViewActorPtr actors[2];
std::map<long, sockaddr_in> addresses;
void handleSocketInput(SocketEvent& event){
	int state;// = (int) event.buffer;
	memcpy(&state, event.buffer, sizeof(state));

	StrongSideViewActorPtr& actor = actors[0];//(ntohs(event.address.sin_port) == 1337) ? actors[0] : actors[1];
	
	if (state & SET_MOVING_FORWARD_TRUE){
		actor->mbMovingForward = true;
	}

	if (state & SET_MOVING_FORWARD_FALSE){
		actor->mbMovingForward = false;
	}

	if (state & SET_TURNING_LEFT_TRUE){
		actor->mbTurningLeft = true;
	}

	if (state & SET_TURNING_LEFT_FALSE){
		actor->mbTurningLeft = false;
	}

	if (state & SET_TURNING_RIGHT_TRUE){
		actor->mbTurningRight = true;
	}

	if (state & SET_TURNING_RIGHT_FALSE){
		actor->mbTurningRight = false;
	}
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
	if(argc < 2){
		std::cout << "Seriously 1 args man" << std::endl;
		exit(1);
	}
	int bindTo = atoi(argv[1]);

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
				if (actor != nullptr){
					if (actor->mbMovingForward){
						actor->MoveForward(2);
					}

					if (actor->mbTurningLeft){
						actor->RotateZ(.2);
					}

					if (actor->mbTurningRight){
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