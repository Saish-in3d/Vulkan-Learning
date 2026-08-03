file(GLOB_RECURSE VULKANCORE ./*.cpp)
add_library(vulkancoreLib ${VULKANCORE}) 
#Saving
add_library(
    Device.cpp
    Device.h
    instance.cpp
    instance.h
    WindowSurface.cpp
    WindowSurface.h
    vk_common.h
)