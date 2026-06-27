#include "window.h"

VKFW::Platform::Window::Window(int Width, int Height)
{
	mWidth = Width;
	mHeight = Height;

	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell GLFW not to create an OpenGL context
	mWindow = glfwCreateWindow(mWidth, mHeight, "Vulkan Window", nullptr, nullptr);


	if (!mWindow)
	{
		glfwTerminate();
		throw std::runtime_error("glfwcreatewindowfailed");
	}

}

VKFW::Platform::Window::~Window()
{
	if (mWindow)
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}
}

bool VKFW::Platform::Window::shouldClose() const
{
	if(mWindow)
	return glfwWindowShouldClose(mWindow);
}

void VKFW::Platform::Window::poolEvents()
{
	while (!glfwWindowShouldClose(mWindow))
	{
		glfwPollEvents();
	}
}
