// render_common.h
#pragma once
#include "../Common.h"

// Camera UBO -- binding 0
// View + Projection follow the camera
struct CameraUBO {
    glm::mat4 view{ 1.0f };
    glm::mat4 proj{ 1.0f };
};

// Object UBO -- binding 1
// Model matrix follows the object
struct ObjectUBO {
    glm::mat4 model{ 1.0f };
};