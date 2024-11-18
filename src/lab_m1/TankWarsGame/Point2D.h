#pragma once

#include <cmath>


class Point2D
{
public:
    float x;
    float y;  // Height

public:
    // Constructors
    Point2D() : x(0.0f), y(0.0f) {}
    Point2D(float x_val, float y_val) : x(x_val), y(y_val) {}
    ~Point2D() = default;

    static void RotatePoint(Point2D& point, float cx, float cy, float angle) {
        float s = sin(angle);
        float c = cos(angle);

        // Translate point back to origin:
        point.x -= cx;
        point.y -= cy;

        // Rotate point
        float xNew = point.x * c - point.y * s;
        float yNew = point.x * s + point.y * c;

        // Translate point back:
        point.x = xNew + cx;
        point.y = yNew + cy;
    }
};
