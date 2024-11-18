#include "Transition.h"
#include "object_2D.h"



// Vitezele sunt proportinale sunt rezolutia monitorului (format 16:9)
#define HORIZONTAL_TRANSITION_SPEED ((16.f/9.f) * 1920.f * 0.5f)
#define VERTICAL_TRANSITION_SPEED ((16.f/9.f) * 1080.f * 0.5f)

// Impun ca dreptunghiul sa ajunga in afara ecranului pentru a creea un efect de asteptare (un "wait")
#define SCREEN_PERCENTAGE_TO_FIT 1.15f



Transition* Transition::instance = nullptr;

Transition* Transition::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Transition();
    }
    return instance;
}

Transition::Transition()
{
    this->transitionType = TransitionType::FROM_LEFT_TO_RIGHT;
    this->transitionState = TransitionState::FIRST_HALF;
    this->rectangleMesh = object_2D::MemAllocMesh("chenar pentru tranzitie");

    this->rectangleHeight = 0.f;
    this->rectangleWidth = 0.f;
    this->leftBottomCorner = Point2D();
    this->isInTransition = false;
    this->rectangleColor = glm::vec3(0.f, 0.f, 0.f);
}

Transition::~Transition() {}


void Transition::ReinitTransition(const glm::ivec2 &resolution)
{
    this->rectangleWidth = 0.f;
    this->rectangleHeight = 0.f;
    this->leftBottomCorner.x = 0.f;
    this->leftBottomCorner.y = 0.f;

    if (this->transitionType == TransitionType::FROM_LEFT_TO_RIGHT) {
        // Horizontal transition
        this->rectangleHeight = resolution.x;
    }
    else if (this->transitionType == TransitionType::FROM_RIGHT_TO_LEFT) {
        // Horizontal transition
        this->rectangleHeight = resolution.y;
        this->leftBottomCorner.x = resolution.x;
    }
    else if (this->transitionType == TransitionType::FROM_BOTTOM_TO_TOP) {
        // Vertical transition
        this->rectangleWidth = resolution.x;
    }
    else if (this->transitionType == TransitionType::FROM_TOP_TO_BOTTOM) {
        // Vertical transition
        this->rectangleWidth = resolution.x;
        this->leftBottomCorner.y = resolution.y;
    }

    this->transitionState = TransitionState::FIRST_HALF;
    this->isInTransition = true;
}


void Transition::UpdateTransition(const glm::ivec2 &resolution, float deltaTimeSeconds)
{
    if (this->transitionType == TransitionType::FROM_LEFT_TO_RIGHT) {
        // horizontal transition
        Transition::UpdateTransitionFromLeftToRight(resolution, deltaTimeSeconds);
        return;
    }
    else if (this->transitionType == TransitionType::FROM_RIGHT_TO_LEFT) {
        // horizontal transition
        Transition::UpdateTransitionFromRightToLeft(resolution, deltaTimeSeconds);
        return;
    }
    else if (this->transitionType == TransitionType::FROM_BOTTOM_TO_TOP) {
        // vertical transition
        Transition::UpdateTransitionFromBottomToTop(resolution, deltaTimeSeconds);
        return;
    }
    else if (this->transitionType == TransitionType::FROM_TOP_TO_BOTTOM) {
        // vertical transition
        Transition::UpdateTransitionFromTopToBottom(resolution, deltaTimeSeconds);
        return;
    }
}


void Transition::UpdateTransitionFromLeftToRight(const glm::ivec2 &resolution, float deltaTimeSeconds)
{
    // horizontal transition



    if (this->transitionState == TransitionState::FIRST_HALF) {
        this->rectangleWidth += HORIZONTAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleWidth > resolution.x * SCREEN_PERCENTAGE_TO_FIT) {
            this->transitionState = TransitionState::SECOND_HALF;
        }
    }
    else {
        this->rectangleWidth -= HORIZONTAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleWidth < 0) {
            this->transitionType = TransitionType::FROM_RIGHT_TO_LEFT;
            this->transitionState = TransitionState::END;
            this->isInTransition = false;
        }
    }


    UpdateRectangleMesh();
}


void Transition::UpdateTransitionFromRightToLeft(const glm::ivec2 &resolution, float deltaTimeSeconds)
{
    // horizontal transition

    if (this->transitionState == TransitionState::FIRST_HALF) {
        this->rectangleWidth += HORIZONTAL_TRANSITION_SPEED * deltaTimeSeconds;
        this->leftBottomCorner.x -= HORIZONTAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleWidth > resolution.x * SCREEN_PERCENTAGE_TO_FIT) {
            this->transitionState = TransitionState::SECOND_HALF;
        }
    }
    else {
        this->rectangleWidth -= HORIZONTAL_TRANSITION_SPEED * deltaTimeSeconds;
        this->leftBottomCorner.x += HORIZONTAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleWidth < 0) {
            this->transitionType = TransitionType::FROM_BOTTOM_TO_TOP;
            this->transitionState = TransitionState::END;
            this->isInTransition = false;
        }
    }



    UpdateRectangleMesh();
}


void Transition::UpdateTransitionFromBottomToTop(const glm::ivec2 &resolution, float deltaTimeSeconds)
{
    // vertical transition

    if (this->transitionState == TransitionState::FIRST_HALF) {
        this->rectangleHeight += VERTICAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleHeight > resolution.y * SCREEN_PERCENTAGE_TO_FIT) {
            this->transitionState = TransitionState::SECOND_HALF;
        }
    }
    else {
        this->rectangleHeight -= VERTICAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleHeight < 0) {
            this->transitionType = TransitionType::FROM_TOP_TO_BOTTOM;
            this->transitionState = TransitionState::END;
            this->isInTransition = false;
        }
    }


    UpdateRectangleMesh();
}


void Transition::UpdateTransitionFromTopToBottom(const glm::ivec2 &resolution, float deltaTimeSeconds)
{
    // vetical transition



    if (this->transitionState == TransitionState::FIRST_HALF) {
        this->rectangleHeight += VERTICAL_TRANSITION_SPEED * deltaTimeSeconds;
        this->leftBottomCorner.y -= VERTICAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleHeight > resolution.y * SCREEN_PERCENTAGE_TO_FIT) {
            this->transitionState = TransitionState::SECOND_HALF;
        }
    }
    else {
        this->rectangleHeight -= VERTICAL_TRANSITION_SPEED * deltaTimeSeconds;
        this->leftBottomCorner.y += VERTICAL_TRANSITION_SPEED * deltaTimeSeconds;
        if (this->rectangleHeight < 0) {
            this->transitionState = TransitionState::END;
            this->transitionType = TransitionType::FROM_LEFT_TO_RIGHT;
            this->isInTransition = false;
        }
    }


    UpdateRectangleMesh();
}



void Transition::UpdateRectangleMesh()
{
    object_2D::UpdateRectangle(
        this->rectangleMesh,
        glm::vec3(this->leftBottomCorner.x, this->leftBottomCorner.y, 0),
        this->rectangleWidth, this->rectangleHeight, this->rectangleColor, true);
}