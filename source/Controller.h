#pragma once
#include <memory>
#include "Emitter.h"
#include "Window.h"
#include "UDPSocket.h"
#include <arpa/inet.h>
#include <list>
class Controller;

const int MOVING_FORWARD = (1 << 0);
const int TURNING_LEFT = (1 << 1);
const int TURNING_RIGHT = (1 << 2);

struct StateBufferItem{
	unsigned int packetId;
	int state;
};

typedef std::shared_ptr<Controller> StrongControllerPtr;
class Controller : public Emitter<void>{
private:
	Window* mWindow;
	
	sockaddr_in mServerAddress;
public:
	int state = 0;//temp
	std::list<StateBufferItem> stateBuffer; //temp public????
	void SetServer(char*, short);
	void Initialize(Window*);
	void KeyCallback(WindowEvent*);
	void Send(UDPSocket* socket); 
}; 

