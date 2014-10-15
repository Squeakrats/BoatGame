#pragma once
#include <memory>
#include "Emitter.h"
#include "Window.h"
#include "UDPSocket.h"
#include <arpa/inet.h>

class Controller;

const int SET_MOVING_FORWARD_TRUE = (1 << 0);
const int SET_MOVING_FORWARD_FALSE = (1 << 1);

const int SET_TURNING_LEFT_TRUE = (1 << 2);
const int SET_TURNING_LEFT_FALSE= (1 << 3);

const int SET_TURNING_RIGHT_TRUE = (1 << 4);
const int SET_TURNING_RIGHT_FALSE = (1 << 5);


typedef std::shared_ptr<Controller> StrongControllerPtr;
class Controller : public Emitter<void>{
private:
	Window* mWindow;
	int state = 0;
	sockaddr_in mServerAddress;
public:
	void SetServer(char*, short);
	void Initialize(Window*);
	void KeyCallback(WindowEvent*);
	void Send(UDPSocket* socket); 
}; 

