file(GLOB_RECURSE VULKANCORE ./*.cpp)
add_library(vulkancoreLib ${VULKANCORE} "Commandpool.h" "Commandpool.cpp" "Swapchain.h" "Swapchain.cpp" "RenderPass.h" "RenderPass.cpp" "Fence.h" "Fence.cpp" "Semaphore.h" "Semaphore.cpp" "DataBuffer.h" "DataBuffer.cpp" "DescriptorParameter.h" "DescriptorSetLayout.h" "DescriptorSetLayout.cpp" "DescriptorPool.h" "DescriptorPool.cpp" "DescriptorSet.h" "DescriptorSet.cpp" "image.h" "image.cpp" "stb_image.h" "Sampler.h" "Sampler.cpp" "Texture.h" "Texture.cpp") 
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

 "Fence.h" "Fence.cpp" "Semaphore.h" "Semaphore.cpp" "DataBuffer.h" "DataBuffer.cpp" "DescriptorParameter.h" "DescriptorSetLayout.h" "DescriptorSetLayout.cpp" "DescriptorPool.h" "DescriptorPool.cpp" "DescriptorSet.h" "DescriptorSet.cpp" "image.h" "image.cpp" "stb_image.h" "Sampler.h" "Sampler.cpp" "Texture.h" "Texture.cpp")