#include "Controller.h"
#include "GLFW/glfw3.h"
#include <iostream>

void Controller::SetServer(char* ip, short port){
	mServerAddress.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &mServerAddress.sin_addr);
	mServerAddress.sin_port = htons(port);
}
void Controller::Initialize(Window* window) {
	window->On(1, std::bind(&Controller::KeyCallback, this, std::placeholders::_1));
}

void Controller::KeyCallback(WindowEvent* event) {
	if (event->key == GLFW_KEY_W){
		if (event->action == GLFW_PRESS){
			state |= SET_MOVING_FORWARD_TRUE;
			Emit(SET_MOVING_FORWARD_TRUE);
		}
		else if (event->action == GLFW_RELEASE){
			state |= SET_MOVING_FORWARD_FALSE;
			Emit(SET_MOVING_FORWARD_FALSE);
		}
	}

	if (event->key == GLFW_KEY_LEFT){
		if (event->action == GLFW_PRESS){
			state |= SET_TURNING_LEFT_TRUE;
			Emit(SET_TURNING_LEFT_TRUE);
		}
		else if (event->action == GLFW_RELEASE){
			state |= SET_TURNING_LEFT_FALSE;
			Emit(SET_TURNING_LEFT_FALSE);
		}
	}

	if (event->key == GLFW_KEY_RIGHT){
		if (event->action == GLFW_PRESS){
			state |= SET_TURNING_RIGHT_TRUE;
			Emit(SET_TURNING_RIGHT_TRUE);
		}
		else if (event->action == GLFW_RELEASE){
			state |= SET_TURNING_RIGHT_FALSE;
			Emit(SET_TURNING_RIGHT_FALSE);
		}
	}

}

void Controller::Send(UDPSocket* socket) {
	if (state){
		//super temp.
		socket->SendReliable(0, (const char*)&state, sizeof(state), &mServerAddress);
		state = 0;
	}
}