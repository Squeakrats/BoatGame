#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>


#include "GL/glew.h"
#include <assimp/Importer.hpp>  
#include <assimp/scene.h>  
#define GLM_FORCE_RADIANS
#include "glm/gtx/transform.hpp"
#include "Actor.h"
#include "MeshComponent.h"
#include "SideViewActor.h"
#include "MeshFactory.h"
#include "Renderer.h"
#include "SceneNode.h"
#include "util.h"
#include "UDPSocket.h"
#include "Window.h"
#include "Controller.h"
#include "string.h"

Assimp::Importer importer;




StrongSideViewActorPtr createClientSideViewActor(unsigned int id, StrongMeshPtr mesh) {
	StrongSideViewActorPtr actor(new SideViewActor());
	actor->SetId(id);
	StrongMeshComponentPtr meshComponent(new MeshComponent());
	meshComponent->SetActor(actor);
	meshComponent->SetMesh(mesh);
	actor->AddComponent(meshComponent);
	actor->On(0, std::bind(&MeshComponent::OnActorMove, meshComponent));
	return actor;

}


UDPSocket* udp;
StrongSideViewActorPtr actors[2];
StrongSideViewActorPtr controlledActor;
StrongControllerPtr controller;

struct GameUpdateActor {
	unsigned int actorId;
	float x, y, z;
	float rx, ry, rz;
};

unsigned int LAST_PACKET_ID;
void OnServerFrameUpdate(SocketEvent& event) {
	if(event.item->packetId <= LAST_PACKET_ID){
		return;
	}
	LAST_PACKET_ID = event.item->packetId;
	//std::cout << controller
	unsigned int numActors;
	memcpy(&numActors, event.buffer, sizeof(unsigned int));
	unsigned int lastPacketId;
	memcpy(&lastPacketId, event.buffer + sizeof(unsigned int), sizeof(unsigned int));


	GameUpdateActor* actorStructs = new GameUpdateActor[numActors];
	memcpy(actorStructs, event.buffer + 2 * sizeof(unsigned int), sizeof(GameUpdateActor) * numActors);


//	std::cout << lastPacketId << std::endl;
	bool allOld = true;
	for(auto itr = controller->stateBuffer.begin(); itr != controller->stateBuffer.end(); ++itr){
		if(itr->packetId > lastPacketId){
			controller->stateBuffer.erase(controller->stateBuffer.begin(), itr);
			allOld = false;
			break;
		}
	}

	if(allOld){
		std::cout << "CLEAR!" << std::endl;
		controller->stateBuffer.clear();
	}

	//set the server position
	glm::vec3 newPos = glm::vec3(actorStructs[0].x, actorStructs[0].y, actorStructs[0].z);
	glm::vec3 newRot = glm::vec3(actorStructs[0].rx, actorStructs[0].ry, actorStructs[0].rz);
	glm::vec3 oldPos = actors[0]->GetPosition();
	glm::vec3 oldRot = actors[0]->GetRotation();

	//std::cout << newPos.x << " " << newPos.y << " " << newPos.z << std::endl;

	actors[0]->SetPosition(newPos);
	actors[0]->SetRotation(newRot);

///*
	for(auto item : controller->stateBuffer){
		if(item.state & MOVING_FORWARD){
			actors[0]->MoveForward(2);
		}
		if(item.state & TURNING_LEFT) {
			actors[0]->RotateZ(.2);
		}
		if(item.state & TURNING_RIGHT) {
			actors[0]->RotateZ(-.2);
		}
	}

	glm::vec3 posDif = newPos - oldPos;
	glm::vec3 rotDif = newRot - oldRot;

	
	if(glm::length(posDif) > .20f){
		//glm::vec3 corrected = oldPos + .20f * posDif;
		//actors[0]->SetPosition(corrected);
	}
	if(glm::length(rotDif) > .10f){
		//glm::vec3 corrected = oldRot + .40f * rotDif;
		//actors[0]->SetRotation(corrected);
	}
	


//*/
	//do some correcting after that?????? lerp the dif. 

	

	delete actorStructs;
} 


/*
no more sendReliable for key inputs. that shit needs to be spammed.
work on slidey formulas. they need considerable tweakin. 
already totally ignoring old messages. 
*/

int main(int argc, char* argv[]) {
	LAST_PACKET_ID = 0;
	if(argc < 4){
		std::cout << "Seriously 3 args man" << std::endl;
		exit(1);
	}

	int bindTo = atoi(argv[1]);
	char* serverIP = argv[2];
	int serverPort = atoi(argv[3]);


	int width = 1920, height = 1080;
	Window window;
	window.Initialize(width, height, "MyGameEngine");
	Renderer renderer;
	int program = createProgram("assets/shaders/simpleVertex.glsl", "assets/shaders/simpleFragment.glsl");
	const aiScene* aiscene = importer.ReadFile("assets/models/garvey-work-boat.dae", 0);
	StrongMeshPtr boatMesh = MeshFactory::ConvertAiScene(aiscene);

	controller = StrongControllerPtr(new Controller());
	controller->Initialize(&window);
	controller->SetServer(serverIP, serverPort);

	StrongSceneNodePtr scene(new SceneNode());
	actors[0] = createClientSideViewActor(0, boatMesh);
	actors[1] = createClientSideViewActor(1, boatMesh);
	scene->AddChild(actors[0]->GetComponent<MeshComponent>(1));
	scene->AddChild(actors[1]->GetComponent<MeshComponent>(1));
	actors[0]->mbIsMovementPredicted = true;


	UDPSocket* udp = new UDPSocket();
	udp->Init();
	udp->SetBlocking(false);
	udp->Bind(bindTo);

	udp->On(0, OnServerFrameUpdate);
	std::cout << "connecting to server...." << std::endl;
	udp->SendReliable(1, "hello!", sizeof("hello!"), serverIP, serverPort);

	auto last = std::chrono::high_resolution_clock::now();
	while (!window.ShouldClose()){
		auto now = std::chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
		double leftovers = 0;
		
		if (dt.count() + leftovers >= 17) { //temo run server and double fps
			last = now;
			leftovers = (dt.count() + leftovers) - 17;

			//maybe???
			///*
			
			StrongSideViewActorPtr actor = actors[0];
			int actorState = controller->state;
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
			}//*/
			controller->Send(udp);
			udp->PollEvents();


			
			renderer.Render(scene, width, height, program);
			window.SwapBuffers();
			window.PollEvents();
			

		}
	};

}


/*
for (int i = 0; i < numActors; i++){
		StrongSideViewActorPtr actor = actors[i];
		GameUpdateActor& actorStruct = actorStructs[i];

		const glm::vec3& currPos = actor->GetPosition();
		const glm::vec3& currRot = actor->GetRotation();

		glm::vec3 newPos = glm::vec3(actorStruct.x, actorStruct.y, actorStruct.z);
		glm::vec3 newRot = glm::vec3(actorStruct.rx, actorStruct.ry, actorStruct.rz);

		if(actor->mbIsMovementPredicted){

			glm::vec3 posDif = newPos - currPos;
			glm::vec3 rotDif = newRot - currRot;

			if(glm::length(posDif) < .15f || false){
				actor->SetPosition(newPos);
			}else{
				std::cout << "Bad Predictions dude" << std::endl;
				glm::vec3 corrected = currPos + .25f * posDif;
				actor->SetPosition(corrected);
			}

			if(glm::length(rotDif) < .15f || false){
				actor->SetRotation(newRot);
			}else{
				std::cout << "Bad Predictions dude" << std::endl;
				glm::vec3 corrected = currRot + .25f * rotDif;
				actor->SetRotation(corrected);
			}




		}else{
			//this is the only thing that is going on. 
			actor->SetPosition(newPos);
			actor->SetRotation(newRot);
		}

	}

*/