#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>



#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <assimp/Importer.hpp>  
#include <assimp/scene.h>  
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


void handleSocketInput(SocketEvent& event){
	int state;// = (int) event.buffer;
	memcpy(&state, event.buffer, sizeof(state));

	StrongSideViewActorPtr& actor = (ntohs(event.address.sin_port) == 1337) ? actors[0] : actors[1];
	
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

struct GameUpdateActor {
	unsigned int actorId;
	float x, y, z;
	float rx, ry, rz;
};

void OnServerFrameUpdate(SocketEvent& event) {
	unsigned int numActors;
	memcpy(&numActors, event.buffer, sizeof(unsigned int));
	GameUpdateActor* actorStructs = new GameUpdateActor[numActors];
	memcpy(actorStructs, event.buffer + sizeof(unsigned int), sizeof(GameUpdateActor) * numActors);

	for (int i = 0; i < numActors; i++){
		GameUpdateActor& actorStruct = actorStructs[i];
		glm::vec3 position = glm::vec3(actorStruct.x, actorStruct.y, actorStruct.z);
		actors[actorStruct.actorId]->SetPosition(position);
		glm::vec3 rotation = glm::vec3(actorStruct.rx, actorStruct.ry, actorStruct.rz);
		actors[actorStruct.actorId]->SetRotation(rotation);
	}
	delete actorStructs;
} 




int main(int argc, char** args) {


	bool isServer = false;
	int port = 1337;

	if (argc == 3){
		std::cout << args[1] << std::endl;
		if (args[1][0] == 'S'){
			isServer = true;
			std::cout << "Launching Server" << std::endl;
		}
		else{
			std::cout << "Launching Client" << std::endl;
		}

		port = atoi(args[2]);
		std::cout << "Port : " << port << std::endl;
	}




	UDPSocket* udp = new UDPSocket();
	udp->Init();
	udp->SetBlocking(false);


	if (!isServer){
		udp->Bind(port);
		udp->On(0, OnServerFrameUpdate);
		int width = 1920, height = 1080;
		Window window;
		window.Initialize(width, height, "MyGameEngine");

		Renderer renderer;
		int program = createProgram("assets/shaders/simpleVertex.glsl", "assets/shaders/simpleFragment.glsl");


		const aiScene* aiscene = importer.ReadFile("assets/models/garvey-work-boat.dae", 0);
		StrongMeshPtr boatMesh = MeshFactory::ConvertAiScene(aiscene);

		StrongControllerPtr controller(new Controller());
		controller->Initialize(&window);

		StrongSceneNodePtr scene(new SceneNode());
		actors[0] = createClientSideViewActor(0, boatMesh);
		actors[1] = createClientSideViewActor(1, boatMesh);
		scene->AddChild(actors[0]->GetComponent<MeshComponent>(1));
		scene->AddChild(actors[1]->GetComponent<MeshComponent>(1));

		auto last = std::chrono::high_resolution_clock::now();
		while (!window.ShouldClose()){
			auto  now = std::chrono::high_resolution_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

			if (dt.count() > 17) {
				now = last;
				udp->PollEvents();
				renderer.Render(scene, width, height, program);
				window.SwapBuffers();
				window.PollEvents();
				controller->Send(udp);
			}

		};
	}
	else{

		udp->Bind(port);
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
				udp->Send(0, buffer, sizeof(unsigned int) +numActors * sizeof(GameUpdateActor), "127.0.0.1", 1337);
				udp->Send(0, buffer, sizeof(unsigned int) +numActors * sizeof(GameUpdateActor), "127.0.0.1", 1336);
				//std::cout << actorRotation.x << actorRotation.y << actorRotation.z << std::endl;
			}
		}
	}



	
}
