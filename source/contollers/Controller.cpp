#include "Controller.h"
#include "GLFW\glfw3.h"
#include <iostream>
void Controller::Initialize(Window* window) {
	window->On(1, std::bind(&Controller::KeyCallback, this, std::placeholders::_1));
}

void Controller::KeyCallback(WindowEvent* event) {
	if (event->key == GLFW_KEY_W){
		if (event->action == GLFW_PRESS){
			state |= SET_MOVING_FORWARD_TRUE;
			Emit(SET_MOVING_FORWARD_TRUE, nullptr);
		}
		else if (event->action == GLFW_RELEASE){
			state |= SET_MOVING_FORWARD_FALSE;
			Emit(SET_MOVING_FORWARD_FALSE, nullptr);
		}
	}

	if (event->key == GLFW_KEY_LEFT){
		if (event->action == GLFW_PRESS){
			state |= SET_TURNING_LEFT_TRUE;
			Emit(SET_TURNING_LEFT_TRUE, nullptr);
		}
		else if (event->action == GLFW_RELEASE){
			state |= SET_TURNING_LEFT_FALSE;
			Emit(SET_TURNING_LEFT_FALSE, nullptr);
		}
	}

	if (event->key == GLFW_KEY_RIGHT){
		if (event->action == GLFW_PRESS){
			state |= SET_TURNING_RIGHT_TRUE;
			Emit(SET_TURNING_RIGHT_TRUE, nullptr);
		}
		else if (event->action == GLFW_RELEASE){
			state |= SET_TURNING_RIGHT_FALSE;
			Emit(SET_TURNING_RIGHT_FALSE, nullptr);
		}
	}

}

void Controller::Send(UDPSocket* socket) {
	if (state){
		socket->SendReliable(0, (const char*)&state, sizeof(state), "127.0.0.1", 1338);
		state = 0;
	}
}