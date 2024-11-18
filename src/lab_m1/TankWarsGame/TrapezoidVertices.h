#pragma once

#include "Point2D.h"


class TrapezoidVertices
{
public:
    Point2D bottomLeft;
    Point2D bottomRight;
    Point2D topLeft;
    Point2D topRight;

public:
    // Constructors
    TrapezoidVertices() : bottomLeft(Point2D()), bottomRight(Point2D()), topLeft(Point2D()), topRight(Point2D()) {}
    TrapezoidVertices(Point2D bottomLeft_val, Point2D bottomRight_val, Point2D topLeft_val, Point2D topRight_val)
        : bottomLeft(bottomLeft), bottomRight(bottomRight), topLeft(topLeft), topRight(topRight) {}
    ~TrapezoidVertices() = default;
};
