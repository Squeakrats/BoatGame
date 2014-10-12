#pragma once
#include <memory>
#include "Emitter.h"
#include "Window.h"
#include "UDPSocket.h"

class Controller;

const int SET_MOVING_FORWARD_TRUE = (1 << 0);
const int SET_MOVING_FORWARD_FALSE = (1 << 1);

const int SET_TURNING_LEFT_TRUE = (1 << 2);
const int SET_TURNING_LEFT_FALSE= (1 << 3);

const int SET_TURNING_RIGHT_TRUE = (1 << 4);
const int SET_TURNING_RIGHT_FALSE = (1 << 5);




class GameEvent{

};

typedef std::shared_ptr<Controller> StrongControllerPtr;
class Controller : public Emitter<GameEvent*>{
private:
	Window* mWindow;
	int state = 0;
public:
	void Initialize(Window*);
	void KeyCallback(WindowEvent*);
	void Send(UDPSocket* socket); 
}; 

