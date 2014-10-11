#pragma once
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "../graphics/Renderer.h"
#include <list>
#include "../actors/Actor.h"
#include "../graphics/SceneNode.h"
#include "../network/Socket.h"
#include "glm\glm.hpp"
/*
//these might need to go somewhere else :/
const int MOVE_UP = 1 < 0;
const int MOVE_DOWN = 1 < 1;
const int MOVE_LEFT = 1 < 2;
const int MOVE_RIGHT = 1 < 3;

struct UpdatePacket {
	struct Actor {
		unsigned int actorId;
		float x;
		float y;
		float z;
	};
	unsigned int numActors;
	Actor* actors;
};



class GameEngine {
private:

	//GameEngine()// :mDgramSocket(SOCK_DGRAM, IPPROTO_UDP){};
	GLFWwindow* mWindow;
	unsigned int mProgram;
	Renderer mRenderer;
	StrongSceneNodePtr mScene;
	Socket mDgramSocket;
	std::map<unsigned int, StrongActorPtr> mActors;

	static GameEngine* sEngine;
public:
	
	StrongSceneNodePtr GetScene(void);
	void AddActor(StrongActorPtr);
	bool IsRunning(void);
	void UpdateMS();
	void PollEvents();//?????GameEngine needs access to the socket then :O
	void Render(void);
	void OnServerGameUpdate(SocketEvent&);

	static void Init(int, int);
	static GameEngine& Get(void) { return *sEngine; }

	};*/