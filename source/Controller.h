#pragma once
#include <memory>
#include "Emitter.h"
#include "Window.h"
#include "UDPSocket.h"
#include <arpa/inet.h>

class Controller;

const int MOVING_FORWARD = (1 << 0);
const int TURNING_LEFT = (1 << 1);
const int TURNING_RIGHT = (1 << 2);


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

