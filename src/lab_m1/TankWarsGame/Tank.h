#pragma once

#include "Point2D.h"
#include "Appearance.h"    // Group for Appearance (Color and Mesh)
#include "TrapezoidVertices.h"
#include "Terrain.h"
#include "object_2D.h"
#include "Projectile.h"
#include "TrajectoryBeam.h"

// Forward declare the class
class Projectile;
class TrajectoryBeam;


struct Trapezoid {
    TrapezoidVertices vertices;

    float bottomWidth;
    float topWidth;
    float height;

    Appearance appearance;
};

struct SemiCircle {
    Point2D centerVertex;
    float radius;
    float startAngle;
    float endAngle;

    Appearance appearance;
};

struct Turret {
    Point2D topRight;
    Point2D topLeft;
    Point2D bottomRight;
    Point2D bottomLeft;

    float angleRadiansWithOX; // Angle of the turret relative to the OX-axis
    float angleOffset;        // Values between [-0.5, 0.5]
    float width;
    float length;

    Appearance appearance;
};


struct LifeBar {
    Point2D bottomLeft;
    Point2D position;
    float width;
    float height;

    Appearance outlineRectangle;
    Appearance fillRectangle;

};




// Main Tank Class
class Tank {
public:
    Trapezoid bottomTrapezoid;
    Trapezoid topTrapezoid;
    SemiCircle semiCircle;
    Turret turret;

    float tankAngleWithOX;
    float tankSpeed;

    Point2D position;               // Bottom Middle Point of the lower trapezoid (includes tankPositionX and tankPositionY)
    LifeBar lifeBar;
    float tankLife;                 // Values between [0, 100]

    TrajectoryBeam *trajectoryBeam;

    // Timpul (in secunde) scurs de la pornirea jocului si momentul in care o parte din tanc a fost lovita
    double elapsedTimeBottomTrapezoidWasHit;
    double elapsedTimeTopTrapezoidWasHit;
    double elapsedTimeSemiCircleWasHit;
    double elapsedTimeTurretWasHit;
    
    bool bottomTrapezoidWasRecenltyHit;
    bool topTrapezoidWasRecenltyHit;
    bool semiCircleWasRecenltyHit;
    bool turretWasRecenltyHit;

public:
    // Constructors
    Tank();
    Tank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val);
    Tank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val, glm::vec3 bottomTrapezoidColor_val, glm::vec3 topTrapezoidColor_val, glm::vec3 semiCircleColor_val, glm::vec3 turretColor_val);
    ~Tank();

public:
    void ReinitTank();
    void ReinitTank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val);
    void ReinitTank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val, glm::vec3 bottomTrapezoidColor_val, glm::vec3 topTrapezoidColor_val, glm::vec3 semiCircleColor_val, glm::vec3 turretColor_val);
    void InitializeTank(float tankPositionX_val, float initialTurretDegreesAngleWithOX_val, glm::vec3 colorBottomTrapezoid_val, glm::vec3 colorTopTrapezoid_val, glm::vec3 semicircleColor_val, glm::vec3 turretColor_val);

private:
    bool isInitialized;
    void SetInitialTankPositionAndTurretAngle(float tankPositionX_val, float initialTurretAnglesDegreesWithOX_val);
    void SetInitialTankColors(glm::vec3 bottomTrapezoidColor_val, glm::vec3 topTrapezoidColor_val, glm::vec3 semiCircleColor_val, glm::vec3 turretColor_val);
    bool isValidColor(const glm::vec3& color);



public:
    void UpdateTankMeshes();
    void UpdateTankVertices(float tankPositionX_val);
    void UpdateTankTrajectoryBeamMeshes();

public:
    void MoveTankToLeft(float deltaTime, glm::ivec2& resolution);
    void MoveTankToRight(float deltaTime, glm::ivec2& resolution);
    void MoveTurretToLeft(float deltaTime, glm::ivec2& resolution);
    void MoveTurretToRight(float deltaTime, glm::ivec2& resolution);


public:
    void DecreaseIdleTankLife(float deltaTimeSeconds);
    void DecreaseHitTankLife(float projectileDamage);
    void UpdateTankLifeBar();


public:
    bool IsTankHitByProjectile(Projectile* projectile);


private:
    bool isInitializedTrajectoryBeam;

private:
    bool IsTankBottomTrapezoidHitByProjectile(Projectile* projectile);

    bool IsTankTopTrapezoidHitByProjectile(Projectile* projectile);

    bool IsTankSemiCircleHitByProjectile(Projectile* projectile);

    bool IsTankTurretHitByProjectile(Projectile* projectile);

    bool IsPointInsideTrapezoid(
        float point_x, float point_y,
        float bottomLeft_x, float bottomLeft_y,
        float bottomRight_x, float bottomRight_y,
        float topLeft_x, float topLeft_y,
        float topRight_x, float topRight_y);

    bool IsPointInsideRectangle(
        float point_x, float point_y,
        float bottomLeft_x, float bottomLeft_y,
        float bottomRight_x, float bottomRight_y,
        float topLeft_x, float topLeft_y,
        float topRight_x, float topRight_y);

    bool IsPointInsideSemiCircle(
        float point_x, float point_y,
        float c_x, float c_y,
        float radius,
        float startAngle, float endAngle);
};