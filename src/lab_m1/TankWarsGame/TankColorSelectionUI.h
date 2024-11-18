#pragma once


#include "Point2D.h"
#include "TrapezoidVertices.h"
#include "Terrain.h"
#include "object_2D.h"



enum class SelectionType {
    NONE,
    BOY,
    GIRL
};

struct RectangleBox
{
    Mesh* mesh;
    glm::vec3 color;
    Point2D bottomLeft;
    float width;
    float height;
};


struct TankColors
{
    glm::vec3 upperPartColor;
    glm::vec3 lowerPartColor;
};

class PlayerSelection
{
public:
    RectangleBox firstOptionBox;     // chenarul de jos
    RectangleBox secondOptionBox;    // chenarul de sus
    RectangleBox selectionBox;

    SelectionType selectionType;
    TankColors selectedTrapezoidColors;

public:
    void UpdateSelectionBox(SelectionType selectionType_value, bool isFirstPlayer);

public:
    PlayerSelection() : selectionType(SelectionType::NONE) {}
    ~PlayerSelection() = default;
};


class TankColorSelectionUI
{
  
private:
    // Hide the constructors by making them private (for Singleton)
    TankColorSelectionUI();
    TankColorSelectionUI(glm::ivec2 &resolution);
    static TankColorSelectionUI* instance;

public:
    // Wrapper for constructors (for Singleton)
    // The first instance must be called with the PARAMETIRZED constructor
    static TankColorSelectionUI* GetInstance(glm::ivec2 &resolution);
    // Any other instance must be called with the DEFAULT constructor
    static TankColorSelectionUI* GetInstance();
    ~TankColorSelectionUI();

private:
    void CreateMeshes();
    void InitColors();
    void InitBottomLeftPoints(glm::ivec2 &resolution);
    void InitDimensions(glm::ivec2 &sresolution);
    void InitMeshes();



public:
    PlayerSelection playerSelection1;
    PlayerSelection playerSelection2;
    RectangleBox timerBarBox;
    RectangleBox middleSeparationLine;
    bool isInIntro;

public:
    void UpdateTimer(float deltaTimeSeconds, glm::ivec2 &resolution);
};
