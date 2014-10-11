#pragma once
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "../events/Emitter.h"

struct WindowEvent {
	int key;
	int action;
};

//possibly make a non propegating emitter? 


//right now assume one window per thingy
class Window : public Emitter<WindowEvent*> {
private:
	static void SetActiveWindow(Window* window){
		sActiveWindow = window;
		glfwSetWindowCloseCallback(window->GetWindow(), &Window::WindowCloseCallback);
		glfwSetKeyCallback(window->GetWindow(), &Window::WindowKeyCallback);
	}
	static Window* sActiveWindow;
	static void WindowCloseCallback(GLFWwindow* window) { sActiveWindow->CloseCallback(); };
	static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { sActiveWindow->KeyCallback(key, scancode, action, mods); };
	GLFWwindow* mWindow;
public:
	Window(){};
	bool Initialize(int, int, std::string);
	GLFWwindow* GetWindow() { return mWindow; };
	int ShouldClose(void) { return glfwWindowShouldClose(mWindow); };
	void PollEvents(void);
	void SwapBuffers(void);
	//
	void CloseCallback();
	void KeyCallback(int, int, int, int);
};
