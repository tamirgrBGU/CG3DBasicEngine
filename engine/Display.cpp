

#include <chrono>
#include <thread>

#include "../gl.h"
#include "Display.h"

#include "igl/igl_inline.h"
#include <igl/get_seconds.h>
#include "igl/opengl/glfw/renderer.h"

namespace igl
{

namespace opengl
{

namespace glfw
{

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API, "; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System, "; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler, "; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party, "; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application, "; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other, "; break;
	};

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error, "; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour, "; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour, "; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability, "; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance, "; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker, "; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group, "; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group, "; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other, "; break;
	};

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: Medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: Notification"; break;
	} 
	
	std::cout << std::endl;

#if !defined(NDEBUG) && defined(_MSC_VER)
	__debugbreak();
#endif
}

static void glfw_error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

Display::Display(int windowWidth, int windowHeight, const std::string& title, bool compatibilityMode)
{
	bool resizable = true, fullscreen = false;
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 8);

	if (compatibilityMode)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	}
	else
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}

	//#ifdef __APPLE__
	//		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//#endif
	//		if (fullscreen)
	//		{
	//			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	//			window = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
	//			windowWidth = mode->width;
	//			windowHeight = mode->height;
	//		}
	//		else
	//		{
				// Set default windows width
				//if (windowWidth <= 0 & core_list.size() == 1 && renderer->core().viewport[2] > 0)
				//	windowWidth = renderer->core().viewport[2];
				//else 
				//	if (windowWidth <= 0)
				//	windowWidth = 1280;
				//// Set default windows height
				//if (windowHeight <= 0 & core_list.size() == 1 && renderer->core().viewport[3] > 0)
				//	windowHeight = renderer->core().viewport[3];
				//else if (windowHeight <= 0)
				//	windowHeight = 800;
	//			window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
	//		}
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL and its extensions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to load OpenGL and its extensions\n");
		exit(EXIT_FAILURE);
	}

	int contextFlags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
	if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		std::cout << "Debug context created" << std::endl;
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}

	//#if defined(DEBUG) || defined(_DEBUG)
	//		printf("OpenGL Version %d.%d loaded\n", GLVersion.major, GLVersion.minor);
	//		int major, minor, rev;
	//		major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	//		minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	//		rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	//		printf("OpenGL version received: %d.%d.%d\n", major, minor, rev);
	//		printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	//		printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	//#endif
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//Tamir: changes from here
	// Initialize FormScreen
   // __viewer = this;
	// Register callbacks
	//glfwSetKeyCallback(window, glfw_key_callback);
	//glfwSetCursorPosCallback(window,glfw_mouse_move);
	//glfwSetScrollCallback(window, glfw_mouse_scroll);
	//glfwSetMouseButtonCallback(window, glfw_mouse_press);
	//glfwSetWindowSizeCallback(window,glfw_window_size);


	//glfwSetCharModsCallback(window,glfw_char_mods_callback);
	//glfwSetDropCallback(window,glfw_drop_callback);
	// Handle retina displays (windows and mac)
	//int width, height;
	//glfwGetFramebufferSize(window, &width, &height);
	//int width_window, height_window;
	//glfwGetWindowSize(window, &width_window, &height_window);
	//highdpi = windowWidth/width_window;

	//glfw_window_size(window,width_window,height_window);
	//opengl.init();
//		core().align_camera_center(data().V, data().F);
		// Initialize IGL viewer
//		init();

}

bool Display::launch_rendering(bool loop)
{
	// glfwMakeContextCurrent(window);
	// Rendering loop
	const int num_extra_frames = 5;
	int frame_counter = 0;
	int windowWidth, windowHeight;
	//main loop
	Renderer* renderer = (Renderer*)glfwGetWindowUserPointer(window);
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	//	renderer->post_resize(window, windowWidth, windowHeight);
	//	for(int i=0;i< renderer->GetScene()->data_list.size();i++)
	//		renderer->core().toggle(renderer->GetScene()->data_list[i].show_lines);
	while (!glfwWindowShouldClose(window))
	{

		double tic = igl::get_seconds();
		renderer->Animate();
		renderer->draw(window);
		glfwSwapBuffers(window);
		if (renderer->GetScene()->isActive || frame_counter++ < num_extra_frames)
		{//motion
			glfwPollEvents();
			// In microseconds
			double duration = 1000000. * (igl::get_seconds() - tic);
			const double min_duration = 1000000. / 144.f;
			if (duration < min_duration)
			{
				std::this_thread::sleep_for(std::chrono::microseconds((int)(min_duration - duration)));
			}
		}
		else
		{
			glfwPollEvents();
			frame_counter = 0;
		}
		if (!loop)
			return !glfwWindowShouldClose(window);

#ifdef __APPLE__
		static bool first_time_hack = true;
		if (first_time_hack) {
			glfwHideWindow(window);
			glfwShowWindow(window);
			first_time_hack = false;
		}
#endif
	}
	return EXIT_SUCCESS;
}

void Display::AddKeyCallBack(void(*keyCallback)(GLFWwindow*, int, int, int, int))
{
	glfwSetKeyCallback(window, (void(*)(GLFWwindow*, int, int, int, int))keyCallback);//{

}

void Display::AddMouseCallBacks(void (*mousebuttonfun)(GLFWwindow*, int, int, int), void (*scrollfun)(GLFWwindow*, double, double), void (*cursorposfun)(GLFWwindow*, double, double))
{
	glfwSetMouseButtonCallback(window, mousebuttonfun);
	glfwSetScrollCallback(window, scrollfun);
	glfwSetCursorPosCallback(window, cursorposfun);
}

void Display::AddResizeCallBack(void (*windowsizefun)(GLFWwindow*, int, int))
{
	glfwSetWindowSizeCallback(window, windowsizefun);
}

void Display::SetRenderer(void* userPointer)
{

	glfwSetWindowUserPointer(window, userPointer);

}

void* Display::GetScene()
{
	return glfwGetWindowUserPointer(window);
}

void Display::SwapBuffers()
{
	glfwSwapBuffers(window);
}

void Display::PollEvents()
{
	glfwPollEvents();
}

Display::~Display()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

} // glfw

} // opengl

} // igl
