#include "object_2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


#define NUM_SEGMENTS_IN_SEMI_CIRCLE (int) 50
#define NUM_SEGMENTS_IN_FULL_CIRCLE (int) 100


Mesh* object_2D::MemAllocMesh(const std::string& name)
{
    // Crearea obiectului Mesh
    return new Mesh(name);
}



void object_2D::UpdateSquare(
    Mesh *square,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
}


Mesh* object_2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    Mesh* square = new Mesh(name);
    UpdateSquare(square, leftBottomCorner, length, color, fill);
    return square;
}



void object_2D::UpdateRectangle(
    Mesh *rectangle,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    // Definirea celor 4 colturi ale dreptunghiului
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        // contur (linie)
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Daca vrem sa umplem dreptunghiul, folosim doua triunghiuri
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
}

Mesh* object_2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    Mesh* rectangle = new Mesh(name);
    UpdateRectangle(rectangle, leftBottomCorner, width, height, color, fill);
    return rectangle;
}


void object_2D::UpdateTriangle(
    Mesh* triangle,
    const std::vector<glm::vec3>& vertices,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertexData;
    for (const auto& v : vertices) {
        vertexData.push_back(VertexFormat(v, color));
    }


    std::vector<unsigned int> indices;

    if (!fill) {
        for (unsigned int i = 0; i < vertices.size(); i++) {
            indices.push_back(i);
        }
        indices.push_back(0);
        triangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        for (unsigned int i = 0; i < vertices.size() - 2; i++) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
        }
        triangle->SetDrawMode(GL_TRIANGLES);
    }

    // Initialize the mesh with vertex and index data
    triangle->InitFromData(vertexData, indices);

}

Mesh* object_2D::CreateTriangle(
    const std::string& name,
    const std::vector<glm::vec3>& vertices,
    glm::vec3 color,
    bool fill)
{

    Mesh* triangle = new Mesh(name);
    UpdateTriangle(triangle, vertices, color, fill);
    return triangle;
}



void object_2D::UpdateTrapezoid(
    Mesh* trapezoid,
    glm::vec3 bottomLeft,
    glm::vec3 bottomRight,
    glm::vec3 topLeft,
    glm::vec3 topRight,
    glm::vec3 color,
    bool fill)
{
    // Definirea celor 4 varfuri ale trapezului
    std::vector<VertexFormat> vertices = {
        VertexFormat(bottomLeft, color),
        VertexFormat(bottomRight, color),
        VertexFormat(topRight, color),
        VertexFormat(topLeft, color)
    };


    // Definirea indicilor pentru triunghiuri
    std::vector<unsigned int> indices;

    if (!fill) {
        indices = { 0, 1, 2, 3 }; 
        trapezoid->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // doua triunghiuri
        indices = { 0, 1, 2, 0, 2, 3 };
        trapezoid->SetDrawMode(GL_TRIANGLES);
    }

    trapezoid->InitFromData(vertices, indices);
}

Mesh* object_2D::CreateTrapezoid(
    const std::string& name,
    glm::vec3 bottomLeft,
    glm::vec3 bottomRight,
    glm::vec3 topLeft,
    glm::vec3 topRight,
    glm::vec3 color,
    bool fill)
{


    // Crearea obiectului Mesh pentru trapez
    Mesh* trapezoid = new Mesh(name);
    UpdateTrapezoid(trapezoid, bottomLeft, bottomRight, topLeft, topRight, color, fill);
    return trapezoid;
}




void object_2D::UpdateSemiCircle(
    Mesh* semiCircle,
    glm::vec3 center,
    float radius,
    float startAngle,
    float endAngle,
    glm::vec3 color,
    bool fill)
{
    // Crearea punctelor pe semicerc
    std::vector<VertexFormat> vertices;
    unsigned int numSegments = NUM_SEGMENTS_IN_SEMI_CIRCLE;
    for (unsigned int i = 0; i <= numSegments; ++i) {
        float angle = startAngle + (endAngle - startAngle) * i / numSegments;
        glm::vec3 vertex(center.x + radius * cos(angle), center.y + radius * sin(angle), center.z);
        vertices.push_back(VertexFormat(vertex, color));
    }

    std::vector<unsigned int> indices;
    if (fill) {
        // Triunghiuri pentru a umple semicercul
        for (unsigned int i = 1; i < vertices.size() - 1; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        semiCircle->SetDrawMode(GL_TRIANGLES);
    }
    else {
        // Conturul semicercului (linie)
        for (unsigned int i = 0; i < vertices.size() - 1; ++i) {
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        semiCircle->SetDrawMode(GL_LINE_STRIP);
    }

    semiCircle->InitFromData(vertices, indices);
}

Mesh* object_2D::CreateSemiCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    float startAngle,
    float endAngle,
    glm::vec3 color,
    bool fill)
{
    Mesh* semiCircle = new Mesh(name);
    UpdateSemiCircle(semiCircle, center, radius, startAngle, endAngle, color, fill);
    return semiCircle;
}





void object_2D::UpdateCircle(
    Mesh* circle,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    unsigned int numSegments = NUM_SEGMENTS_IN_FULL_CIRCLE;

    if (fill) {
        vertices.push_back(VertexFormat(center, color));
    }

    // Generate vertices along the circle's edge
    for (unsigned int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        glm::vec3 vertex(center.x + radius * cos(angle), center.y + radius * sin(angle), center.z);
        vertices.push_back(VertexFormat(vertex, color));
    }

    // Define indices
    std::vector<unsigned int> indices;
    if (fill) {
        // Use triangle fan to fill the circle
        for (unsigned int i = 1; i < vertices.size() - 1; ++i) {
            // Center vertex
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        circle->SetDrawMode(GL_TRIANGLES);
    }
    else {
        // Use line loop to outline the circle
        for (unsigned int i = 1; i < vertices.size(); ++i) {
            indices.push_back(i);
        }
        // Close the loop
        indices.push_back(1);
        circle->SetDrawMode(GL_LINE_LOOP);
    }

    circle->InitFromData(vertices, indices);
}

Mesh* object_2D::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    Mesh* circle = new Mesh(name);
    UpdateCircle(circle, center, radius, color, fill);
    return circle;
}
