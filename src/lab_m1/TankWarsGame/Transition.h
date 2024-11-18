#pragma once

#include "Point2D.h"
#include "TrapezoidVertices.h"
#include "Terrain.h"
#include "object_2D.h"


enum class TransitionType {
    FROM_LEFT_TO_RIGHT,
    FROM_RIGHT_TO_LEFT,
    FROM_BOTTOM_TO_TOP,
    FROM_TOP_TO_BOTTOM
};

enum class TransitionState {
    FIRST_HALF,
    SECOND_HALF,
    END
};

class Transition
{



public:
    Mesh* rectangleMesh;
    float rectangleWidth;
    float rectangleHeight;
    glm::vec3 rectangleColor;
    Point2D leftBottomCorner;
    TransitionType transitionType;
    TransitionState transitionState;

    /* 
    Daca atributul "isInTransition" nu este setat la "false", tranzitia va rula la INFINIT
    Pentru a opri tranzitia, seteaza atributul "isInTransition" la "true"
    */
    bool isInTransition;


public:
    static Transition* GetInstance();
    ~Transition();

private:
    Transition();
    static Transition* instance;

public:
    void ReinitTransition(const glm::ivec2 &resolution);
    void UpdateTransition(const glm::ivec2 &resolution, float deltaTimeSeconds);
    void UpdateRectangleMesh();

private:
    void UpdateTransitionFromLeftToRight(const glm::ivec2 &resolution, float deltaTimeSeconds);
    void UpdateTransitionFromRightToLeft(const glm::ivec2 &resolution, float deltaTimeSeconds);
    void UpdateTransitionFromBottomToTop(const glm::ivec2 &resolution, float deltaTimeSeconds);
    void UpdateTransitionFromTopToBottom(const glm::ivec2 &resolution, float deltaTimeSeconds);
};
