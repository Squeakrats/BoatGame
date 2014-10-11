#include "GameEngine.h"
#include "../graphics/util.h"
/*
void GameEngine::Init(int width, int height) {
	glfwInit();
	sEngine->mWindow = glfwCreateWindow(width, height, "~Game Engine",nullptr, nullptr);
	glfwMakeContextCurrent(sEngine->mWindow);
	//glfwSetKeyCallback(mWindow, gWindowKeyCallBack);
	glewExperimental = true;
	glewInit();
	sEngine->mProgram = createProgram("assets\\shaders\\simpleVertex.glsl", "assets\\shaders\\simpleFragment.glsl");
	sEngine->mScene = StrongSceneNodePtr(new SceneNode());


	//set up socket.
	sEngine->mDgramSocket.Init();
	sEngine->mDgramSocket.Bind(1337);
	sEngine->mDgramSocket.Connect("127.0.0.1", 1338);
	sEngine->mDgramSocket.SetBlocking(false);
	sEngine->mDgramSocket.On(0, std::bind(&GameEngine::OnServerGameUpdate, sEngine, std::placeholders::_1));
	//bind the callback too. 
}

void GameEngine::AddActor(StrongActorPtr actor) {
	//prob should check for dupes :/
	mActors[actor->GetId()] = actor;
}
bool GameEngine::IsRunning(void) {
	return !glfwWindowShouldClose(mWindow);
}

void GameEngine::PollEvents(void) {
	glfwPollEvents();
	mDgramSocket.PollEvents();
}

void GameEngine::Render(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mRenderer.Render(mScene, mProgram);
	glfwSwapBuffers(mWindow);
}

StrongSceneNodePtr GameEngine::GetScene(void) {
	return mScene;
}

void GameEngine::OnServerGameUpdate(SocketEvent& event) {
	UpdatePacket update; //should prob make a buffer of this instead of making and destroyin all day
	event.stream.read((char*)&update, sizeof(unsigned int));
	update.actors = new UpdatePacket::Actor[update.numActors];
	event.stream.read((char*)update.actors, sizeof(UpdatePacket::Actor) * update.numActors );

	for (int i = 0; i < update.numActors; i++){
		auto itr = mActors.find(update.actors[i].actorId);
		if (itr != mActors.end()){
			glm::vec3 position(update.actors[i].x, update.actors[i].y, update.actors[i].z);
			itr->second->GetComponent<OrientationComponent>(0)->SetPosition(position);//unsafe assumption that it has an orientation

		}
	}
	}*/