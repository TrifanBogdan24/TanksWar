#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"



namespace object_2D
{
    Mesh* MemAllocMesh(const std::string& name);

    
    void UpdateSquare(Mesh *square, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

    void UpdateRectangle(Mesh *rectangle, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);


    void UpdateTriangle(Mesh* triangle, const std::vector<glm::vec3>& vertices, glm::vec3 color, bool fill = false);
    Mesh* CreateTriangle(const std::string& name, const std::vector<glm::vec3>& vertices, glm::vec3 color, bool fill = false);

    void UpdateTrapezoid(Mesh* trapezoid, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 color, bool fill = false);
    Mesh* CreateTrapezoid(const std::string& name, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 color, bool fill = false);

    void UpdateSemiCircle(Mesh* semiCircle, glm::vec3 center, float radius, float startAngle = 0.f, float endAngle = glm::half_pi<float>(), glm::vec3 color = glm::vec3(0.f, 0.f, 0.f), bool fill = false);
    Mesh* CreateSemiCircle(const std::string& name, glm::vec3 center, float radius, float startAngle = 0.f, float endAngle = glm::half_pi<float>(), glm::vec3 color = glm::vec3(0.f, 0.f, 0.f), bool fill = false);

    void UpdateCircle(Mesh* circle, glm::vec3 center, float radius, glm::vec3 color, bool fill);
    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill);
}
