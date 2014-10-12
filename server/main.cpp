#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include "string.h"



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
void handleSocketInput(SocketEvent& event){
	std::cout << "Got client input" << std::endl;
	int state;// = (int) event.buffer;
	memcpy(&state, event.buffer, sizeof(state));

	StrongSideViewActorPtr& actor = actors[0];//(ntohs(event.address.sin_port) == 1337) ? actors[0] : actors[1];
	
	if (state & SET_MOVING_FORWARD_TRUE){
		actor->mMovingForward = true;
	}

	if (state & SET_MOVING_FORWARD_FALSE){
		actor->mMovingForward = false;
	}

	if (state & SET_TURNING_LEFT_TRUE){
		actor->mTurningLeft = true;
	}

	if (state & SET_TURNING_LEFT_FALSE){
		actor->mTurningLeft = false;
	}

	if (state & SET_TURNING_RIGHT_TRUE){
		actor->mTurningRight = true;
	}

	if (state & SET_TURNING_RIGHT_FALSE){
		actor->mTurningRight = false;
	}
}


int main(int argc, char*argv[]) {

	int BindTo = 1337;
	int sendTo = 1337;

	UDPSocket* udp = new UDPSocket();
	udp->Init();
	udp->SetBlocking(false);
	udp->Bind(BindTo);
	udp->On(0, handleSocketInput);

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
					if (actor->mMovingForward){
						actor->MoveForward(2);
					}

					if (actor->mTurningLeft){
						actor->RotateZ(2);
					}

					if (actor->mTurningRight){
						actor->RotateZ(-2);
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
			//generic send to these 2 places. 
			udp->Send(0, buffer, sizeof(unsigned int) +numActors * sizeof(GameUpdateActor), "24.240.35.26", sendTo);
			udp->Send(0, buffer, sizeof(unsigned int) +numActors * sizeof(GameUpdateActor), "127.0.0.1", sendTo);
			//std::cout << actorRotation.x << actorRotation.y << actorRotation.z << std::endl;
		}
	}
}