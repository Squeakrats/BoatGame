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
			state |= MOVING_FORWARD;
		}
		else if (event->action == GLFW_RELEASE){
			state &= ~(MOVING_FORWARD);
		}
	}

	if (event->key == GLFW_KEY_LEFT){
		if (event->action == GLFW_PRESS){
			state |= TURNING_LEFT;
		}
		else if (event->action == GLFW_RELEASE){
			state &= ~(TURNING_LEFT);
		}
	}

	if (event->key == GLFW_KEY_RIGHT){
		if (event->action == GLFW_PRESS){
			state |= TURNING_RIGHT;
		}
		else if (event->action == GLFW_RELEASE){
			state &= ~(TURNING_RIGHT);
		}
	}

}

void Controller::Send(UDPSocket* socket) {
	StateBufferItem item = { socket->mCurrentPacketId, state};
	stateBuffer.push_back(item);
	socket->Send(0, (const char*)&state, sizeof(state), &mServerAddress);
}