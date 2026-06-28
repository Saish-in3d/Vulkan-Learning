#pragma once
#include "../common.h"

namespace VKFW::Platform {
	class Window
	{
	public:
		Window(int Width, int Height);
		~Window();

		bool shouldClose() const;
		void poolEvents();


		using ResizeFn = std::function< void (int,int)>;
		void setOnResize(ResizeFn fn) { mOnResize = std::move(fn); } //Implementing binder function 

	private:

		GLFWwindow* mWindow;
		int mWidth, mHeight;

		static void framebuffer_size_callback(GLFWwindow* inWindow, int inWidth, int inHeight);
		
		ResizeFn mOnResize;
	};

	
}