#include "instance.h"

namespace VKFW::vulkancore {


	Instance::Instance(bool enableValidationLayer)
	{
		mEnableValidationLayer =  enableValidationLayer;
	}

	Instance::~Instance()
	{

		vkDestroyInstance(mInstance, nullptr);
	}

}