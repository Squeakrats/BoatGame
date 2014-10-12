#include "Window.h"
#include <iostream>

Window* Window::sActiveWindow = nullptr;

bool Window::Initialize(int width, int height, std::string name){
	if (!glfwInit()){
		printf("failed to initialize glfw\n");
		return false;
	}
	mWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	if (mWindow == nullptr){
		printf("failed to create window\n");
		return false;
	}
	glfwMakeContextCurrent(mWindow);
	glewExperimental = true;
	if (glewInit() != GLEW_OK){
		printf("failed to initialize glew\n");
		return false;

	}
	SetActiveWindow(this);
	return true;
}

void Window::PollEvents() {
	glfwPollEvents();
}

void Window::SwapBuffers() {
	glfwSwapBuffers(mWindow);
}
 
void Window::CloseCallback() {
	Emit(0, nullptr);
}

void Window::KeyCallback(int key, int scancode, int action, int mods) {
	WindowEvent event = { key, action };
	Emit(1, &event);
}