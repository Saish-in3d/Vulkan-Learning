#pragma once
#include "vk_common.h"

namespace VKFW::vulkancore {

	class Instance
	{
	public:
		explicit Instance(bool enableValidationLayer = true); //explicit + whenever we are creating a new object of class instance as a func arg, we need to specify as Instance(true)
		~Instance();

		VkInstance getInstance() const { return mInstance; }; //we dont want to change anytrhing the function

	private:

		void setupDebugger();

		bool checkValidationLayerSupport();

		std::vector<const char*> getRequiredExtensions();
		void printAvailableExtensions();

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; 

		VkInstance mInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT mDebugger = VK_NULL_HANDLE;
		bool mEnableValidationLayer = true;


	};



}