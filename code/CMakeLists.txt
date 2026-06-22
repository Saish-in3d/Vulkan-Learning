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


#slangc slang-file -entrypoint -stage outputfile-name

set(SLANGC ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/slang/bin/slangc.exe)

#finding shader file folder
set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spv)

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

function(compile_slang SLANG_FILE ENTRY STAGE OUTPUT_SPV)
add_custom_command(
	OUTPUT ${OUTPUT_SPV}
	COMMAND ${SLANGC} ${SLANG_FILE}
	-entry ${ENTRY}
	-stage ${STAGE}
	-o ${OUTPUT_SPV}
	DEPENDS ${SLANG_FILE}
)
endfunction()

FILE(GLOB SLANG_FILES ${SHADER_SRC_DIR}/*.slang)

foreach(SLANG_FILE ${SLANG_FILES})

	#getting fine name without extension
	get_filename_component(FILE_NAME, ${SLANG_FILE} NAME_WE)

	#setting output vertex shader file name
	set(VERT_SPV ${SHADER_OUTPUT_DIR}/${FILE_NAME}_vert.spv)

	#setting output fragment shader file name
	set(FRAG_SPV ${SHADER_OUTPUT_DIR}/${FILE_NAME}_frag.spv)

	#calling the compile_slang function for vert and frag shader

	compile_slang(${SLANG_FILE} vertMain vertex ${VERT_SPV})
	compile_slang(${SLANG_FILE} fragMain fragment ${FRAG_SPV})

	#adding the generated spv files to the source list
	list(APPEND SPV_FILES ${VERT_SPV} ${FRAG_SPV})

endforeach()

#Adding custom target to build the shaders before building the executable

add_custom_target(Shaders ALL DEPENDS ${SPV_FILES})

#Adding ibs to the executable while building

add_executable(VulkanFramework ${SRC_LIST} "common.h")
target_link_libraries(VulkanFramework vulkan-1.lib glfw3.lib)

add_dependencies(VulkanFramework Shaders)


add_custom_command(TARGET VulkanFramework POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${SHADER_OUTPUT_DIR}
$<TARGET_FILE_DIR:VulkanFramework>/spvs
)