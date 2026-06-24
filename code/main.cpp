#include "common.h"
#include "VulkanApp.h"

int main()
{
	VKFW::VulkanApp MYapp;
	try 
	{
		MYapp.run();
	}
	catch (const std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}