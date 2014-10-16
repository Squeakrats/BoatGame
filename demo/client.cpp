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
		StrongSideViewActorPtr actor = actors[i];
		GameUpdateActor& actorStruct = actorStructs[i];

		const glm::vec3& currPos = actor->GetPosition();
		const glm::vec3& currRot = actor->GetRotation();

		glm::vec3 newPos = glm::vec3(actorStruct.x, actorStruct.y, actorStruct.z);
		glm::vec3 newRot = glm::vec3(actorStruct.rx, actorStruct.ry, actorStruct.rz);

		if(actor->mbIsMovementPredicted){

			glm::vec3 posDif = newPos - currPos;
			glm::vec3 rotDif = newRot - currRot;

			if(glm::length(posDif) < .5f){
				actor->SetPosition(newPos);
			}else{
				std::cout << "Bad Predictions dude" << std::endl;
				glm::vec3 corrected = currPos + .10f * posDif;
				actor->SetPosition(corrected);
			}

			if(glm::length(rotDif) < .5f){
				actor->SetRotation(newRot);
			}else{
				std::cout << "Bad Predictions dude" << std::endl;
				glm::vec3 corrected = currRot + .10f * rotDif;
				actor->SetRotation(corrected);
			}




		}else{
			actor->SetPosition(newPos);
			actor->SetRotation(newRot);
		}

	}

	delete actorStructs;
} 

void onSetMovingForwardTrue(void) {
	actors[0]->mbMovingForward = true;
}

void onSetMovingForwardFalse(void) {
	actors[0]->mbMovingForward = false;
}
	
void onSetTurningLeftTrue(void) {
	actors[0]->mbTurningLeft = true;
}

void onSetTurningLeftFalse(void) {
	actors[0]->mbTurningLeft = false;
}

void onSetTurningRightTrue(void) {
	actors[0]->mbTurningRight = true;
}

void onSetTurningRightFalse(void) {
	actors[0]->mbTurningRight = false;
}




int main(int argc, char* argv[]) {
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

	StrongControllerPtr controller(new Controller());
	controller->Initialize(&window);
	controller->SetServer(serverIP, serverPort);

	StrongSceneNodePtr scene(new SceneNode());
	actors[0] = createClientSideViewActor(0, boatMesh);
	actors[1] = createClientSideViewActor(1, boatMesh);
	scene->AddChild(actors[0]->GetComponent<MeshComponent>(1));
	scene->AddChild(actors[1]->GetComponent<MeshComponent>(1));

	actors[0]->mbIsMovementPredicted = false;

	controller->On(SET_MOVING_FORWARD_TRUE, onSetMovingForwardTrue);
	controller->On(SET_MOVING_FORWARD_FALSE, onSetMovingForwardFalse);
	controller->On(SET_TURNING_LEFT_TRUE, onSetTurningLeftTrue);
	controller->On(SET_TURNING_LEFT_FALSE, onSetTurningLeftFalse);
	controller->On(SET_TURNING_RIGHT_TRUE, onSetTurningRightTrue);
	controller->On(SET_TURNING_RIGHT_FALSE, onSetTurningRightFalse);


	UDPSocket* udp = new UDPSocket();
	udp->Init();
	udp->SetBlocking(false);
	udp->Bind(bindTo);

	udp->On(0, OnServerFrameUpdate);
	std::cout << "connecting to server...." << std::endl;
	udp->SendReliable(1, "hello!", sizeof("hello!"), serverIP, serverPort);

	while (!window.ShouldClose()){
		udp->PollEvents();
/*
		for (int i = 0; i < 2; i++){
				StrongSideViewActorPtr& actor = actors[i];
				if (actor != nullptr){
					if (actor->mbMovingForward){
						actor->MoveForward(2);
					}

					if (actor->mbTurningLeft){
						actor->RotateZ(2);
					}

					if (actor->mbTurningRight){
						actor->RotateZ(-2);
					}
				}
			}/*/

		renderer.Render(scene, width, height, program);
		window.SwapBuffers();
		window.PollEvents();
		controller->Send(udp);
		std::this_thread::sleep_for(std::chrono::milliseconds(17));

	};

}
