#pragma once

#include "core/engine.h"
#include "utils/gl_utils.h"
#include "object_2D.h"


// Group for Appearance (Color and Mesh)
struct Appearance {
    glm::vec3 color;
    Mesh* mesh;
};
