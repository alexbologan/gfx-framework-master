#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
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

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

// Function to create a trapezoid
Mesh* object2D::CreateTrapezoid(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float topLength,
    float bottomLength,
    float height,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices = {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(bottomLength, 0, 0), color),
        VertexFormat(corner + glm::vec3((bottomLength - topLength) / 2, height, 0), color),
        VertexFormat(corner + glm::vec3((bottomLength + topLength) / 2, height, 0), color)
    };

    Mesh* trapezoid = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 3, 2 };

    if (!fill) {
        trapezoid->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles to fill the trapezoid
        indices.push_back(0);
        indices.push_back(3);
    }

    trapezoid->InitFromData(vertices, indices);
    return trapezoid;
}

// Function to create a circular arc (approximated with line segments)
Mesh* object2D::CreateDisk(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int numSegments = 100; // Number of segments to approximate the circle

    // Add the center of the disk as the first vertex
    vertices.push_back(VertexFormat(center, color));

    // Generate points around the circle
    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;
        glm::vec3 point = center + glm::vec3(radius * cos(angle), radius * sin(angle), 0);
        vertices.push_back(VertexFormat(point, color));

        // Add indices for the triangles
        if (i > 0) {
            indices.push_back(0);       // Center vertex
            indices.push_back(i);       // Current edge vertex
            indices.push_back(i + 1);   // Next edge vertex
        }
    }

    Mesh* disk = new Mesh(name);
    disk->InitFromData(vertices, indices);
    return disk;
}

// Function to create a rectangle (for the cannon)
Mesh* object2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices = {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles to fill the rectangle
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}
