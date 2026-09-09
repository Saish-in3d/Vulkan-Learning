file(GLOB_RECURSE VULKANCORE ./*.cpp)
add_library(vulkancoreLib ${VULKANCORE} "Commandpool.h" "Commandpool.cpp" "Swapchain.h" "Swapchain.cpp" "RenderPass.h" "RenderPass.cpp" "Fence.h" "Fence.cpp" "Semaphore.h" "Semaphore.cpp" "DataBuffer.h" "DataBuffer.cpp" "UniformParameter.h" "DescriptorSetLayout.h" "DescriptorSetLayout.cpp") 
#Saving
add_library(
    Device.cpp
    Device.h
    instance.cpp
    instance.h
    WindowSurface.cpp
    WindowSurface.h
    vk_common.h
    Commandpool.h
    Commandpool.cpp 
    Swapchain.h
 Swapchain.cpp
 RenderPass.h 
 RenderPass.cpp
  Shader.h 
 Shader.cpp

 "Fence.h" "Fence.cpp" "Semaphore.h" "Semaphore.cpp" "DataBuffer.h" "DataBuffer.cpp" "UniformParameter.h" "DescriptorSetLayout.h" "DescriptorSetLayout.cpp")