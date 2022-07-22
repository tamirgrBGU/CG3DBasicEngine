#pragma once
#include <string>
#include <GLFW/glfw3.h>
#define EXIT_FAILURE 1
struct GLFWwindow;

namespace igl
{

namespace opengl
{

namespace glfw
{

class Display
{
public:
	Display(int windowWidth, int windowHeight, const std::string& title, bool compatibilityMode = true);

	bool launch_rendering(bool loop);

	void SwapBuffers();
	void PollEvents();

	void SetRenderer(void* userPointer);
	void* GetScene();
	void AddKeyCallBack(void(*func)(GLFWwindow*, int, int, int, int));
	void AddMouseCallBacks(void (*mousebuttonfun)(GLFWwindow*, int, int, int), void(*scrollfun)(GLFWwindow*, double, double), void (*cursorposfun)(GLFWwindow*, double, double));
	void AddResizeCallBack(void (*windowsizefun)(GLFWwindow*, int, int));

	~Display();
	//private:
	GLFWwindow* window;
	//Renderer* renderer;
	//int highdpi;  //relation between width and height?
};

} // glfw

} // opengl

} // igl
