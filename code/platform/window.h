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



	private:

		GLFWwindow* mWindow;
		int mWidth, mHeight;
	};

	
}