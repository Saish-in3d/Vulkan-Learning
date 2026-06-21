cmake_minimum_required(VERSION 3.20)
project(VulkanFramework)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. SRC_LIST)

#Fetcxhing vulkan sdk path from environmenta variablres
set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

#FINDNG heeader files
include_directories(
	SYSTEM ${VULKAN_SDK_PATH}/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/GLFW/include
)

#library search path
link_directories(
	${VULKAN_SDK_PATH}/lib
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/GLFW/lib
)

#Adding ibs to the executable while building

add_executable(VulkanFramework ${SRC_LIST})
target_link_libraries(VulkanFramework vulkan-1.lib glfw3.lib)