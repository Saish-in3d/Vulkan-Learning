#include "window.h"

namespace VKFW::Platform
{

	Window::Window(int Width, int Height)
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

		glfwSetWindowUserPointer(mWindow, this);
		glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

	}

	Window::~Window()
	{
		if (mWindow)
		{
			glfwDestroyWindow(mWindow);
			glfwTerminate();
		}
	}

	bool Window::shouldClose() const
	{
		if (mWindow)
			return glfwWindowShouldClose(mWindow);
	}

	void Window::poolEvents()
	{
		//while (!glfwWindowShouldClose(mWindow))
		{
			glfwPollEvents();
		}
	}

	void Window::framebuffer_size_callback(GLFWwindow* inWindow, int inWidth, int inHeight)
	{
		auto self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(inWindow));
		self->mWidth = inWidth;
		self->mHeight = inHeight;
		self->mOnResize(inWidth, inHeight); //Broadcasting implementation
	}
}
