
//#include "vulkan/vulkan.h"

#include <iostream>
#include <memory> // For smart pointers
#include <utility>


//GLM block //gLM Follows opengl convenion for depth range, which is [-1, 1], we need [0, 1] for Vulkan.
#define GLM_FORCE_ZERO_TO_ONE

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

//GLFW block,m defining behaviour

#define GLFW_INCLUDE_VULKAN //exposes vulkan helpers from glfw3.h, helps to create glfw window and vulkan instance, surface, etc.
#include "GLFW/glfw3.h"

namespace VkFW {

	template<class T>
	using ref = std::shared_ptr<T>;

	template<class T>
	using Scope = std::unique_ptr<T>;

	template<class T>
	using WeakRef = std::weak_ptr<T>;

	template<class T, class...Args>
	[[nodiscard]] inline ref<T> MakeRef(Args&&... args) 
	{
		return std::make_shared<T>(std::forward<Args>(args)...)
	}


}