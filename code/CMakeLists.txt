cmake_minimum_required(VERSION 3.20)
project(VulkanFramework) #creates project with name VulkanFramework

set(CMAKE_CXX_STANDARD 17) #telling compiler to use c++17 standard
 
aux_source_directory(. SRC_LIST) #this scans the current directory for all source files and adds them to the SRC_LIST variable (main.cpp) . = current directory

#Fetcxhing vulkan sdk path from environmenta variablres
set(VULKAN_SDK_PATH $ENV{VULKAN_SDK}) # $ENV{VULKAN_SDK} environment variable that contains the path to the Vulkan SDK installation directory

#FINDNG heeader files
include_directories(  # when compiler sees the #include <vulkan/vulkan.h> it knows where to find them
	SYSTEM ${VULKAN_SDK_PATH}/include  #SYSTEM keyword is used to tell the compiler that these are system headers and it should not generate warnings for them
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/GLFW/include 
)

#library search path
link_directories( #when the linker is looking for libraries to link against, it will look in these directories
	${VULKAN_SDK_PATH}/lib 
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/GLFW/lib
)


#slangc slang-file -entrypoint -stage outputfile-name

set(SLANGC ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/slang/bin/slangc.exe) #set is basically setting value to variable
#here we are setting the path to slang compiler exe that will convert slang to spirv

#finding shader file folder
set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders) #CMAKE_CURRENT_SOURCE_DIR is the path where the CMakeLists.txt file is located,
#in this case it is the root directory of the project. So we are setting the path to the shaders folder

set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spv)

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

function(compile_slang SLANG_FILE ENTRY STAGE OUTPUT_SPV) # function_name, Path to slang file, shader entry point function, shader stage (VSPSCS), output spv file name
add_custom_command( #runs a custom command just like we would have don in cmd or terminal #slangc slang-file -entrypoint -stage outputfile-name
	OUTPUT ${OUTPUT_SPV} #Tells CMake: “This command will produce this output file”
	COMMAND ${SLANGC} ${SLANG_FILE} #
	-entry ${ENTRY}                 #actual command that runs slangc, equivalent to typing this in terminal: slangc myShader.slang -entry main -stage fragment -o myShader.frag.spv
	-stage ${STAGE}                 #
	-o ${OUTPUT_SPV}                #
	DEPENDS ${SLANG_FILE} # f the .slang file changes, CMake will re-run the compilation automatically.
)
endfunction()

FILE(GLOB SLANG_FILES ${SHADER_SRC_DIR}/*.slang) #find all .slang files in the shader source directory and store them in the SLANG_FILES variable

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

add_custom_target(Shaders ALL DEPENDS ${SPV_FILES}) #ALL: "Build this target automatically whenever someone runs make or builds the project (like pressing Build in Visual Studio)."

#Adding libs to the executable while building

add_executable(VulkanFramework ${SRC_LIST} "common.h" "VulkanApp.cpp") #produces VulkanFramework.exe
target_link_libraries(VulkanFramework vulkan-1.lib glfw3.lib) #linking libs to vulkanframework.ece target

add_dependencies(VulkanFramework Shaders) #add_dependencies(A B): Make sure target B is built before target A


add_custom_command(TARGET VulkanFramework POST_BUILD #Runs after executable finishes building.
COMMAND ${CMAKE_COMMAND} -E copy_directory #Uses CMake's portable copy command.
${SHADER_OUTPUT_DIR} #Source
$<TARGET_FILE_DIR:VulkanFramework>/spvsc #Destination
)