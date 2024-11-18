#pragma once

#include "utils/glm_utils.h"


namespace transform2D
{
    // Translate matrix
    inline glm::mat3 Translate(float translateX, float translateY)
    {
        return glm::mat3(
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            translateX, translateY, 1
        );
    }

    // Scale matrix
    inline glm::mat3 Scale(float scaleX, float scaleY)
    {
        return glm::mat3(
            scaleX, 0.f, 0.f,
            0.f, scaleY, 0.f,
            0.f, 0.f, 1.f
        );

    }

    // Rotate matrix
    inline glm::mat3 Rotate(float radians)
    {
        GLfloat c = cos(radians);
        GLfloat s = sin(radians);
        return glm::mat3(
            c, s, 0.f,
            -s, c, 0.f,
            0.f, 0.f, 1.f
        );

    }

    // Shear matrix
    inline glm::mat3 Shear(float shearX, float shearY)
    {
        return glm::mat3(
            1.f, shearY, 0.f,
            shearX, 1.f, 0.f,
            0.f, 0.f, 1.f
        );
    }


    inline glm::mat3 ShearX(float shearFactor) {
        return glm::mat3(
            1.0f, shearFactor, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        );
    }


}   // namespace transform2D
