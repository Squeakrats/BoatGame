#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>


#include "GL/glew.h"

#define GLM_FORCE_RADIANS
#include "glm/gtx/transform.hpp"
#include "Actor.h"
#include "MeshComponent.h"
#include "SideViewActor.h"
#include "MeshFactory.h"
#include "Renderer.h"
#include "SceneNode.h"
#include "util.h"
#include "Window.h"
#include "Controller.h"
#include "string.h"
#include <FreeImage.h>


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

/*
need to also be able to generate a normal matrix. 
its like the transpose(inverse(normal))
*/
int main(int argc, char* argv[]) {
	FreeImage_Initialise();
	int width = 1920, height = 1080;
	Window window;
	window.Initialize(width, height, "MyGameEngine");
	Renderer renderer;
	int program = createProgram("assets/shaders/simpleVertex.glsl", "assets/shaders/simpleFragment.glsl");
	StrongMeshPtr boatMesh = MeshFactory::ConvertAiScene("assets/models/T-90/T-90.dae");
	//StrongMeshPtr boatMesh = MeshFactory::ConvertAiScene("assets/models/Crate/Crate1.obj");
	controller = StrongControllerPtr(new Controller());
	controller->Initialize(&window);

	StrongSceneNodePtr scene(new SceneNode());
	actors[0] = createClientSideViewActor(0, boatMesh);
	actors[0]->GetComponent<MeshComponent>(1)->SetScale(.12);
	scene->AddChild(actors[0]->GetComponent<MeshComponent>(1));


	//glm::vec3 rotationVector(1, 0.0f, 0);
	//actors[0]->SetRotation(rotationVector);
			// 
	auto last = std::chrono::high_resolution_clock::now();
	float rotation = 0.0f;
	while (!window.ShouldClose()){
		auto now = std::chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
		double leftovers = 0;
		if (dt.count() + leftovers >= 17) { //temo run server and double fps
			last = now;
			leftovers = (dt.count() + leftovers) - 17;
			
			rotation+=.01;
			glm::vec3 rotationVector(rotation, 0.0f, rotation);
			actors[0]->SetRotation(rotationVector);

			renderer.Render(scene, width, height, program);
			window.SwapBuffers();
			window.PollEvents();
			

		}
	};

}
