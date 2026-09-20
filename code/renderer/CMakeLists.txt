file(GLOB_RECURSE RENDERER ./*.cpp)
add_library(rendererLib ${RENDERER} "tiny_obj_loader.h" "Model.h" "Model.cpp")