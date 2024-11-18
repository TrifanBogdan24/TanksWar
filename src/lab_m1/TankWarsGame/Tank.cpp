#include "Point2D.h"
#include "Terrain.h"
#include "Tank.h"
#include "object_2D.h"
#include "Projectile.h"

#include <vector>


#define COLOR_BLACK glm::vec3(0.f, 0.f, 0.f)
#define COLOR_WHITE glm::vec3(1.f, 1.f, 1.f)


#define BOTTOM_TRAPEZOID_BOTTOM_WIDTH 30.0f
#define BOTTOM_TRAPEZOID_TOP_WIDTH 70.0f
#define BOTTOM_TRAPEZOID_HEIGHT 10.0f

#define TOP_TRAPEZOID_BOTTOM_WIDTH 80.0f
#define TOP_TRAPEZOID_TOP_WIDTH 60.0f
#define TOP_TRAPEZOID_HEIGHT 25.0f

#define SEMI_CIRCLE_RADIUS 15.0f

#define TURRET_LENGTH 40.f
#define TURRET_WIDTH 5.f

#define TURRET_OFFSET_INCREMENT 0.01f
#define TURRET_MIN_OFFSET_COEFF -0.5f
#define TURRET_MAX_OFFSET_COEFF 0.5f

#define TANK_SPEED 120.0f


#define MAX_TANK_LIFE 100.f
#define MIN_TANK_LIFE 0.f
#define LIFE_BAR_WIDTH 70.f
#define LIFE_BAR_HEIGHT 10.f
#define LIFE_BAR_AND_GROUND_DELTA 100.f

// substract this from this->lifeBar.tankLife
#define LIFE_DECREMENT 10.f

// RGB(252, 227, 3) for lifeBar
#define COLOR_YELLOW glm::vec3(252.f/255.f, 227.f/255.f, 3.f/255.f)

// RGB(250, 5, 5) for lifeBar
#define COLOR_RED glm::vec3(250.f/255.f, 5.f/255.f, 5.f/255.f)

#define MAX_TANK_LIFE_VALUE_FOR_YELLOW 60.f
#define MAX_TANK_LIFE_VALUE_FOR_RED 30.f


#define OFFSET_FROM_MIDDLE_AB 15.f

#include <cmath>

using namespace std;

Tank::Tank()
{
    this->isInitialized = false;
}



Tank::~Tank() = default;

void Tank::ReinitTank()
{
    SetInitialTankPositionAndTurretAngle(0.f, 0.f);
}

void Tank::ReinitTank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val)
{
    SetInitialTankPositionAndTurretAngle(tankPositionX_val, initialTurretDegreesAngleWithOX_val);
}

void Tank::ReinitTank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val,
    glm::vec3 bottomTrapezoidColor_val, glm::vec3 topTrapezoidColor_val,
    glm::vec3 semiCircleColor_val, glm::vec3 turretColor_val)
{
    SetInitialTankPositionAndTurretAngle(tankPositionX_val, initialTurretDegreesAngleWithOX_val);
    SetInitialTankColors(bottomTrapezoidColor_val, topTrapezoidColor_val, semiCircleColor_val, turretColor_val);
}

// Call this function ONLY ONCE!!!
void Tank::InitializeTank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val,
    glm::vec3 bottomTrapezoidColor_val, glm::vec3 topTrapezoidColor_val,
    glm::vec3 semiCircleColor_val, glm::vec3 turretColor_val)
{
    if (this->isInitialized) {
        // Tank's meshes and references have already been initialized before
        // DON'T DO THIS TWICE!!!
        return;
    }

    this->isInitialized = true;

    this->bottomTrapezoid = Trapezoid();
    this->topTrapezoid = Trapezoid();
    this->semiCircle = SemiCircle();
    this->turret = Turret();


    SetInitialTankPositionAndTurretAngle(tankPositionX_val, initialTurretDegreesAngleWithOX_val);
    SetInitialTankColors(bottomTrapezoidColor_val, topTrapezoidColor_val, semiCircleColor_val, turretColor_val);

    this->bottomTrapezoidWasRecenltyHit = false;
    this->topTrapezoidWasRecenltyHit = false;
    this->semiCircleWasRecenltyHit = false;
    this->turretWasRecenltyHit = false;

    this->bottomTrapezoid.appearance.mesh = object_2D::MemAllocMesh("Trapezul de jos");
    this->topTrapezoid.appearance.mesh = object_2D::MemAllocMesh("Trapezul de sus");
    this->semiCircle.appearance.mesh = object_2D::MemAllocMesh("Semicerc tureta");
    this->turret.appearance.mesh = object_2D::MemAllocMesh("Teava turetei");
    this->lifeBar.outlineRectangle.mesh = object_2D::MemAllocMesh("Chenarul pentru bara de viata");
    this->lifeBar.fillRectangle.mesh = object_2D::MemAllocMesh("Dreptunghiul de umplere pentru bara de viata");


    this->trajectoryBeam = new TrajectoryBeam(this);
}

void Tank::SetInitialTankPositionAndTurretAngle(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val)
{
    this->tankLife = MAX_TANK_LIFE;
    this->lifeBar.fillRectangle.color = COLOR_WHITE;

    this->position.x = tankPositionX_val;
    this->position.y = 0;

    this->turret.angleRadiansWithOX = glm::pi<float>();

    if (0.f <= initialTurretDegreesAngleWithOX_val && initialTurretDegreesAngleWithOX_val <= 90.f) {
        this->turret.angleOffset = TURRET_MAX_OFFSET_COEFF * abs((90.f - initialTurretDegreesAngleWithOX_val) / 90.f);
    }
    else if (90.f < initialTurretDegreesAngleWithOX_val && initialTurretDegreesAngleWithOX_val <= 180.f) {
        this->turret.angleOffset = TURRET_MIN_OFFSET_COEFF * abs((initialTurretDegreesAngleWithOX_val - 90.f) / 90.f);
    }
    else {
        this->turret.angleOffset = 0.f;
    }

    this->bottomTrapezoid.bottomWidth = BOTTOM_TRAPEZOID_BOTTOM_WIDTH;
    this->bottomTrapezoid.topWidth = BOTTOM_TRAPEZOID_TOP_WIDTH;
    this->bottomTrapezoid.height = BOTTOM_TRAPEZOID_HEIGHT;

    this->topTrapezoid.bottomWidth = TOP_TRAPEZOID_BOTTOM_WIDTH;
    this->topTrapezoid.topWidth = TOP_TRAPEZOID_TOP_WIDTH;
    this->topTrapezoid.height = TOP_TRAPEZOID_HEIGHT;

    this->semiCircle.radius = SEMI_CIRCLE_RADIUS;

    this->tankSpeed = TANK_SPEED;

    this->turret.length = TURRET_LENGTH;
    this->turret.width = TURRET_WIDTH;

    this->lifeBar.width = LIFE_BAR_WIDTH;
    this->lifeBar.height = LIFE_BAR_HEIGHT;
}

void Tank::SetInitialTankColors(glm::vec3 bottomTrapezoidColor_val, glm::vec3 topTrapezoidColor_val, glm::vec3 semiCircleColor_val, glm::vec3 turretColor_val)
{
    if (!isValidColor(bottomTrapezoidColor_val)) {
        this->bottomTrapezoid.appearance.color = COLOR_BLACK;
    }
    else {
        this->bottomTrapezoid.appearance.color = bottomTrapezoidColor_val;
    }

    if (!isValidColor(topTrapezoidColor_val)) {
        this->topTrapezoid.appearance.color = COLOR_BLACK;
    }
    else {
        this->topTrapezoid.appearance.color = topTrapezoidColor_val;
    }

    if (!isValidColor(semiCircleColor_val)) {
        this->semiCircle.appearance.color = COLOR_BLACK;
    }
    else {
        this->semiCircle.appearance.color = semiCircleColor_val;
    }

    if (!isValidColor(turretColor_val)) {
        this->turret.appearance.color = COLOR_BLACK;
    }
    else {
        this->turret.appearance.color = turretColor_val;
    }

    this->lifeBar.fillRectangle.color = COLOR_WHITE;
    this->lifeBar.outlineRectangle.color = COLOR_WHITE;

}

bool Tank::isValidColor(const glm::vec3& color)
{
    float red = color[0];
    float green = color[1];
    float blue = color[2];

    return (red >= 0.0f && red <= 1.0f)
        && (green >= 0.0f && green <= 1.0f)
        && (blue >= 0.0f && blue <= 1.0f);
}



/// Compute all tank vertices based on the OX value of the midpoint of the bottom trapezoid's bottom line
void Tank::UpdateTankVertices(float tankPositionX_val)
{
    Terrain* terrain = Terrain::GetInstance();

    this->position.x = tankPositionX_val;
    this->position.y = terrain->GetHeight(this->position.x);


    // Pozitia tancului pe axa X
    float x = this->position.x;

    // Alege punctele A si B pe teren, apropiate de pozitia tancului
    // Punctul A este imediat in stanga tancului
    Point2D A(x - OFFSET_FROM_MIDDLE_AB, terrain->GetHeight(x - OFFSET_FROM_MIDDLE_AB));

    // Punctul B este imediat in dreapta tancului
    Point2D B(x + OFFSET_FROM_MIDDLE_AB, terrain->GetHeight(x + OFFSET_FROM_MIDDLE_AB));

    // Calculul lui t pentru pozitia tancului intre A si B pe axa X
    float t = (x - A.x) / (B.x - A.x);

    // Calculul pozitiei tancului pe axa Y
    float y = A.y + t * (B.y - A.y);
    this->position = Point2D(x, y); // Actualizarea pozitiei tancului

    // Calcularea unghiul de rotatie pe baza vectorului A -> B
    this->tankAngleWithOX = atan2(B.y - A.y, B.x - A.x);

    // Calcularea coordonatelor trapezului de jos
    float halfBottomTrapezoidBottomWidth = this->bottomTrapezoid.bottomWidth / 2;
    float halfBottomTrapezoidTopWidth = this->bottomTrapezoid.topWidth / 2;

    // Coordonatele pentru trapezul de jos (centrate pe X)
    this->bottomTrapezoid.vertices.bottomLeft = Point2D(x - halfBottomTrapezoidBottomWidth, y);
    this->bottomTrapezoid.vertices.bottomRight = Point2D(x + halfBottomTrapezoidBottomWidth, y);
    this->bottomTrapezoid.vertices.topLeft = Point2D(x - halfBottomTrapezoidTopWidth, y + this->bottomTrapezoid.height);
    this->bottomTrapezoid.vertices.topRight = Point2D(x + halfBottomTrapezoidTopWidth, y + this->bottomTrapezoid.height);

    // Rotirea punctelor trapezului de jos
    Point2D::RotatePoint(this->bottomTrapezoid.vertices.bottomLeft, x, y, this->tankAngleWithOX);
    Point2D::RotatePoint(this->bottomTrapezoid.vertices.bottomRight, x, y, this->tankAngleWithOX);
    Point2D::RotatePoint(this->bottomTrapezoid.vertices.topLeft, x, y, this->tankAngleWithOX);
    Point2D::RotatePoint(this->bottomTrapezoid.vertices.topRight, x, y, this->tankAngleWithOX);


    // Calculul punctului de mijloc al laturii de jos a trapezului de sus
    float midLowerOfTopTrapezoidX = (this->bottomTrapezoid.vertices.topLeft.x + this->bottomTrapezoid.vertices.topRight.x) / 2;
    float midLowerOfTopTrapezoidY = (this->bottomTrapezoid.vertices.topLeft.y + this->bottomTrapezoid.vertices.topRight.y) / 2;

    // Calcularea punctului de mijloc al laturii de sus a trapezului de sus
    float midUpperOfTopTrapezoidX = midLowerOfTopTrapezoidX - topTrapezoid.height * sin(this->tankAngleWithOX);
    float midUpperOfTopTrapezoidY = midLowerOfTopTrapezoidY + topTrapezoid.height * cos(this->tankAngleWithOX);

    // Latimea trapezului de sus
    float halfTopTrapezoidBottomWidth = this->topTrapezoid.bottomWidth / 2;
    float halfTopTrapezoidTopWidth = this->topTrapezoid.topWidth / 2;

    // Calcularea punctelor de jos ale trapezului de sus
    this->topTrapezoid.vertices.bottomLeft = Point2D(
        midLowerOfTopTrapezoidX - halfTopTrapezoidBottomWidth * cos(this->tankAngleWithOX),
        midLowerOfTopTrapezoidY - halfTopTrapezoidBottomWidth * sin(this->tankAngleWithOX)
    );
    this->topTrapezoid.vertices.bottomRight = Point2D(
        midLowerOfTopTrapezoidX + halfTopTrapezoidBottomWidth * cos(this->tankAngleWithOX),
        midLowerOfTopTrapezoidY + halfTopTrapezoidBottomWidth * sin(this->tankAngleWithOX)
    );

    // Calcularea punctelor de sus ale trapezului de sus
    this->topTrapezoid.vertices.topLeft = Point2D(
        midUpperOfTopTrapezoidX - halfTopTrapezoidTopWidth * cos(this->tankAngleWithOX),
        midUpperOfTopTrapezoidY - halfTopTrapezoidTopWidth * sin(this->tankAngleWithOX)
    );
    this->topTrapezoid.vertices.topRight = Point2D(
        midUpperOfTopTrapezoidX + halfTopTrapezoidTopWidth * cos(this->tankAngleWithOX),
        midUpperOfTopTrapezoidY + halfTopTrapezoidTopWidth * sin(this->tankAngleWithOX)
    );


    // Calcularea centrului semicercului
    this->semiCircle.centerVertex.x = (this->topTrapezoid.vertices.topRight.x + this->topTrapezoid.vertices.topLeft.x) / 2.f;
    this->semiCircle.centerVertex.y = (this->topTrapezoid.vertices.topRight.y + this->topTrapezoid.vertices.topLeft.y) / 2.f;
    this->semiCircle.startAngle = this->tankAngleWithOX;
    this->semiCircle.endAngle = this->tankAngleWithOX + glm::pi<float>();

    // Calcularea punctelor tevei pentru tureta
    float deltaX = this->topTrapezoid.vertices.topRight.x - this->topTrapezoid.vertices.topLeft.x;
    float deltaY = this->topTrapezoid.vertices.topRight.y - this->topTrapezoid.vertices.topLeft.y;
    this->turret.angleRadiansWithOX = atan2(deltaY, deltaX) + this->turret.angleOffset * glm::pi<float>();

    float centerX = this->semiCircle.centerVertex.x;
    float centerY = this->semiCircle.centerVertex.y;

    this->turret.bottomLeft.x = centerX;
    this->turret.bottomLeft.y = centerY;

    this->turret.bottomRight.x = this->turret.bottomLeft.x + this->turret.width * cos(turret.angleRadiansWithOX);
    this->turret.bottomRight.y = this->turret.bottomLeft.y + this->turret.width * sin(turret.angleRadiansWithOX);


    this->turret.topLeft.x = this->turret.bottomLeft.x - this->turret.length * sin(turret.angleRadiansWithOX);
    this->turret.topLeft.y = this->turret.bottomLeft.y + this->turret.length * cos(turret.angleRadiansWithOX);

    this->turret.topRight.x = this->turret.bottomRight.x - this->turret.length * sin(turret.angleRadiansWithOX);
    this->turret.topRight.y = this->turret.bottomRight.y + this->turret.length * cos(turret.angleRadiansWithOX);



    // Life bar
    this->lifeBar.position.x = this->position.x;
    this->lifeBar.position.y = this->position.y;
    this->lifeBar.bottomLeft.x = this->lifeBar.position.x - this->lifeBar.width / 2;
    this->lifeBar.bottomLeft.y = this->lifeBar.position.y + LIFE_BAR_AND_GROUND_DELTA;
}


void Tank::DecreaseIdleTankLife(float deltaTimeSeconds)
{
    this->tankLife -= deltaTimeSeconds * LIFE_DECREMENT;
    if (this->tankLife < MIN_TANK_LIFE) {
        this->tankLife = MIN_TANK_LIFE;
    }
    UpdateTankLifeBar();
}


void Tank::DecreaseHitTankLife(float projectileDamage)
{
    this->tankLife -= projectileDamage;
    if (this->tankLife < MIN_TANK_LIFE) {
        this->tankLife = MIN_TANK_LIFE;
    }
    UpdateTankLifeBar();
}


void Tank::UpdateTankLifeBar()
{
    if (this->tankLife > MAX_TANK_LIFE) {
        this->tankLife = MAX_TANK_LIFE;
    }
    else if (MAX_TANK_LIFE_VALUE_FOR_YELLOW < this->tankLife && this->tankLife <= MAX_TANK_LIFE) {
        this->lifeBar.fillRectangle.color = COLOR_WHITE;
    }
    else if (MAX_TANK_LIFE_VALUE_FOR_RED < this->tankLife && this->tankLife <= MAX_TANK_LIFE_VALUE_FOR_YELLOW) {
        this->lifeBar.fillRectangle.color = COLOR_YELLOW;
    }
    else if (0.f <= this->tankLife && this->tankLife <= MAX_TANK_LIFE_VALUE_FOR_RED) {
        this->lifeBar.fillRectangle.color = COLOR_RED;
    }
    else if (this->tankLife < 0.f) {
        this->lifeBar.fillRectangle.color = COLOR_WHITE;
        this->tankLife = 0.f;
    }
}


void Tank::UpdateTankMeshes()
{
    if (this->tankLife <= 0) {
        return;
    }

    object_2D::UpdateTrapezoid(
        this->bottomTrapezoid.appearance.mesh,
        glm::vec3(this->bottomTrapezoid.vertices.bottomLeft.x, this->bottomTrapezoid.vertices.bottomLeft.y, 0),
        glm::vec3(this->bottomTrapezoid.vertices.bottomRight.x, this->bottomTrapezoid.vertices.bottomRight.y, 0),
        glm::vec3(this->bottomTrapezoid.vertices.topLeft.x, this->bottomTrapezoid.vertices.topLeft.y, 0),
        glm::vec3(this->bottomTrapezoid.vertices.topRight.x, this->bottomTrapezoid.vertices.topRight.y, 0),
        this->bottomTrapezoid.appearance.color,
        true
    );




    object_2D::UpdateTrapezoid(
        this->topTrapezoid.appearance.mesh,
        glm::vec3(this->topTrapezoid.vertices.bottomLeft.x, this->topTrapezoid.vertices.bottomLeft.y, 0),
        glm::vec3(this->topTrapezoid.vertices.bottomRight.x, this->topTrapezoid.vertices.bottomRight.y, 0),
        glm::vec3(this->topTrapezoid.vertices.topLeft.x, this->topTrapezoid.vertices.topLeft.y, 0),
        glm::vec3(this->topTrapezoid.vertices.topRight.x, this->topTrapezoid.vertices.topRight.y, 0),
        this->topTrapezoid.appearance.color,
        true
    );



    object_2D::UpdateSemiCircle(
        this->semiCircle.appearance.mesh,
        glm::vec3(this->semiCircle.centerVertex.x, this->semiCircle.centerVertex.y, 0),
        this->semiCircle.radius,
        this->semiCircle.startAngle,
        this->semiCircle.endAngle,
        this->semiCircle.appearance.color,
        true);


    // Updatam tureta
    object_2D::UpdateTrapezoid(
        this->turret.appearance.mesh,
        glm::vec3(this->turret.bottomLeft.x, this->turret.bottomLeft.y, 0),
        glm::vec3(this->turret.bottomRight.x, this->turret.bottomRight.y, 0),
        glm::vec3(this->turret.topLeft.x, this->turret.topLeft.y, 0),
        glm::vec3(this->turret.topRight.x, this->turret.topRight.y, 0),
        this->turret.appearance.color,
        true
    );


    // Updatam cele doua chenare pentru lifebar

    // Chenarul din exterior
    object_2D::UpdateRectangle(
        this->lifeBar.outlineRectangle.mesh,
        glm::vec3(this->lifeBar.bottomLeft.x, this->lifeBar.bottomLeft.y, 0),
        this->lifeBar.width,
        this->lifeBar.height,
        this->lifeBar.outlineRectangle.color,
        false
    );


    // Dreptunghiul din interior
    object_2D::UpdateRectangle(
        this->lifeBar.fillRectangle.mesh,
        glm::vec3(this->lifeBar.bottomLeft.x, this->lifeBar.bottomLeft.y, 0),
        (this->tankLife / MAX_TANK_LIFE) * this->lifeBar.width,
        this->lifeBar.height,
        this->lifeBar.fillRectangle.color,
        true
    );
}


void Tank::MoveTankToLeft(float deltaTime, glm::ivec2& resolution)
{
    Terrain* terrain = Terrain::GetInstance();
    float slope = terrain->GetSlope(this->position.x);
    float speed = this->tankSpeed;
    float distance = speed * deltaTime;

    this->position.x -= distance;
    this->UpdateTankVertices(this->position.x);

    if (this->topTrapezoid.vertices.bottomLeft.x < 0.f || this->turret.topLeft.x < 0.f) {
        this->position.x += distance;
        this->UpdateTankVertices(this->position.x);
    }
}


void Tank::MoveTankToRight(float deltaTime, glm::ivec2& resolution)
{
    Terrain* terrain = Terrain::GetInstance();
    float slope = terrain->GetSlope(this->position.x);
    float speed = this->tankSpeed;
    float distance = speed * deltaTime;

    this->position.x += distance;
    this->UpdateTankVertices(this->position.x);

    if (this->topTrapezoid.vertices.bottomRight.x > resolution.x || this->turret.topRight.x > resolution.x) {
        this->position.x -= distance;
        this->UpdateTankVertices(this->position.x);
    }

}

void Tank::MoveTurretToLeft(float deltaTime, glm::ivec2& resolution)
{
    this->turret.angleOffset += TURRET_OFFSET_INCREMENT * glm::pi<float>();

    if (this->turret.angleOffset >= TURRET_MAX_OFFSET_COEFF) {
        this->turret.angleOffset = TURRET_MAX_OFFSET_COEFF;
    }

    this->UpdateTankVertices(this->position.x);


    if (this->turret.topRight.x < 0) {
        this->turret.angleOffset -= TURRET_OFFSET_INCREMENT * glm::pi<float>();
        this->UpdateTankVertices(this->position.x);
    }

}

void Tank::MoveTurretToRight(float deltaTime, glm::ivec2& resolution)
{
    this->turret.angleOffset -= TURRET_OFFSET_INCREMENT * glm::pi<float>();

    if (this->turret.angleOffset <= TURRET_MIN_OFFSET_COEFF) {
        this->turret.angleOffset = TURRET_MIN_OFFSET_COEFF;
    }

    this->UpdateTankVertices(this->position.x);

    if (this->turret.topRight.x > resolution.x) {
        this->turret.angleOffset += TURRET_OFFSET_INCREMENT * glm::pi<float>();
        this->UpdateTankVertices(this->position.x);
    }
}




// Tank hit detection implementations
bool Tank::IsTankHitByProjectile(Projectile* projectile) {
    bool tankWasHit = false;
    if (IsTankBottomTrapezoidHitByProjectile(projectile)) {
        tankWasHit = true;
        this->bottomTrapezoidWasRecenltyHit = true;
        this->elapsedTimeBottomTrapezoidWasHit = Engine::GetElapsedTime();
    }

    if (IsTankTopTrapezoidHitByProjectile(projectile)) {
        tankWasHit = true;
        this->topTrapezoidWasRecenltyHit = true;
        this->elapsedTimeTopTrapezoidWasHit = Engine::GetElapsedTime();
    }
    if (IsTankSemiCircleHitByProjectile(projectile)) {
        tankWasHit = true;
        this->semiCircleWasRecenltyHit = true;
        this->elapsedTimeSemiCircleWasHit = Engine::GetElapsedTime();

    }
    if (IsTankTurretHitByProjectile(projectile)) {
        tankWasHit = true;
        this->turretWasRecenltyHit = true;
        this->elapsedTimeTurretWasHit = Engine::GetElapsedTime();
    }


    return tankWasHit;
}

bool Tank::IsTankBottomTrapezoidHitByProjectile(Projectile* projectile) {
    return IsPointInsideTrapezoid(
        projectile->position.x, projectile->position.y,
        this->bottomTrapezoid.vertices.bottomLeft.x, this->bottomTrapezoid.vertices.bottomLeft.y,
        this->bottomTrapezoid.vertices.bottomRight.x, this->bottomTrapezoid.vertices.bottomRight.y,
        this->bottomTrapezoid.vertices.topLeft.x, this->bottomTrapezoid.vertices.topLeft.y,
        this->bottomTrapezoid.vertices.topRight.x, this->bottomTrapezoid.vertices.topRight.y
    );
}

bool Tank::IsTankTopTrapezoidHitByProjectile(Projectile* projectile) {
    return IsPointInsideTrapezoid(
        projectile->position.x, projectile->position.y,
        this->topTrapezoid.vertices.bottomLeft.x, this->topTrapezoid.vertices.bottomLeft.y,
        this->topTrapezoid.vertices.bottomRight.x, this->topTrapezoid.vertices.bottomRight.y,
        this->topTrapezoid.vertices.topLeft.x, this->topTrapezoid.vertices.topLeft.y,
        this->topTrapezoid.vertices.topRight.x, this->topTrapezoid.vertices.topRight.y
    );
}

bool Tank::IsTankSemiCircleHitByProjectile(Projectile* projectile) {
    return IsPointInsideSemiCircle(
        projectile->position.x, projectile->position.y,
        this->semiCircle.centerVertex.x, this->semiCircle.centerVertex.y,
        this->semiCircle.radius,
        this->semiCircle.startAngle, this->semiCircle.endAngle
    );
}

bool Tank::IsTankTurretHitByProjectile(Projectile* projectile) {
    return IsPointInsideRectangle(
        projectile->position.x, projectile->position.y,
        this->turret.bottomLeft.x, this->turret.bottomLeft.y,
        this->turret.bottomRight.x, this->turret.bottomRight.y,
        this->turret.topLeft.x, this->turret.topLeft.y,
        this->turret.topRight.x, this->turret.topRight.y
    );
}

bool Tank::IsPointInsideTrapezoid(
    float point_x, float point_y,
    float bottomLeft_x, float bottomLeft_y,
    float bottomRight_x, float bottomRight_y,
    float topLeft_x, float topLeft_y,
    float topRight_x, float topRight_y) {
    int intersections = 0;
    auto checkIntersection = [&](float x1, float y1, float x2, float y2) {
        if ((point_y > y1) != (point_y > y2) && (point_x < (x2 - x1) * (point_y - y1) / (y2 - y1) + x1)) {
            intersections++;
        }
    };

    checkIntersection(bottomLeft_x, bottomLeft_y, bottomRight_x, bottomRight_y);
    checkIntersection(bottomRight_x, bottomRight_y, topRight_x, topRight_y);
    checkIntersection(topRight_x, topRight_y, topLeft_x, topLeft_y);
    checkIntersection(topLeft_x, topLeft_y, bottomLeft_x, bottomLeft_y);

    return intersections % 2 == 1;
}

bool Tank::IsPointInsideRectangle(
    float point_x, float point_y,
    float bottomLeft_x, float bottomLeft_y,
    float bottomRight_x, float bottomRight_y,
    float topLeft_x, float topLeft_y,
    float topRight_x, float topRight_y) {
    return IsPointInsideTrapezoid(point_x, point_y, bottomLeft_x, bottomLeft_y, bottomRight_x, bottomRight_y, topLeft_x, topLeft_y, topRight_x, topRight_y);
}

bool Tank::IsPointInsideSemiCircle(
    float point_x, float point_y,
    float c_x, float center_y,
    float radius,
    float startAngle, float endAngle) {
    float dx = point_x - c_x;
    float dy = point_y - center_y;
    float distanceSquared = dx * dx + dy * dy;

    if (distanceSquared > radius * radius)
        return false;

    // Check if the point is within the angular range
    // Unghi in radiani
    float angle = atan2(dy, dx);
    // Normalizeaza la [0, 2π]
    if (angle < 0) angle += 2 * M_PI;


    startAngle = fmod(startAngle, 2 * M_PI);
    endAngle = fmod(endAngle, 2 * M_PI);

    if (startAngle < endAngle) {
        return angle >= startAngle && angle <= endAngle;
    }
    else {
        return angle >= startAngle || angle <= endAngle;
    }
}


void Tank::UpdateTankTrajectoryBeamMeshes()
{
    this->trajectoryBeam->UpdateTankTrajectoryBeam();
}