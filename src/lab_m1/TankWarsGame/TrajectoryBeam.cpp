#include "TrajectoryBeam.h"


#include "core/engine.h"
#include "utils/gl_utils.h"
#include "object_2D.h"

#include "Tank.h"


#define RESOLUTION_WIDTH 1280.f
#define STEP 1.f


#define WHITE_COLOR glm::vec3(1.f, 1.f, 1.f)

#define CIRCLE_POINT_RADIUS 2.f


using namespace std;

// Forward class declaration
class Tank;



TrajectoryBeam::TrajectoryBeam()
{
    InitTrajectoryBeam();
}

TrajectoryBeam::TrajectoryBeam(Tank *tank_value)
{
    this->tank = tank_value;
    InitTrajectoryBeam();
}

void TrajectoryBeam::InitTrajectoryBeam()
{
    for (float i = 0.f; i < ceil(RESOLUTION_WIDTH / STEP); i += STEP) {
        BeamPoint beamPoint = BeamPoint();
        beamPoint.isVisible = false;
        beamPoint.appearance.mesh = object_2D::MemAllocMesh("punct pentru traiectorie");
        beamPoint.appearance.color = WHITE_COLOR;
        beamPoint.center.x = i;
        beamPoint.center.y = 0.f;
        beamPoint.radius = CIRCLE_POINT_RADIUS;

        this->beamPoints.push_back(beamPoint);
    }

}


TrajectoryBeam::~TrajectoryBeam() {}


void TrajectoryBeam::UpdateTankTrajectoryBeam()
{
    Terrain* terrain = Terrain::GetInstance();

    float x_lower = (tank->turret.bottomLeft.x + tank->turret.bottomRight.x) / 2.f;
    float y_lower = (tank->turret.bottomLeft.y + tank->turret.bottomRight.y) / 2.f;
    float x_upper = (tank->turret.topLeft.x + tank->turret.topRight.x) / 2.f;
    float y_upper = (tank->turret.topLeft.y + tank->turret.topRight.y) / 2.f;

    glm::vec2 launchPosition(x_upper, y_upper);

    glm::vec2 direction = glm::normalize(glm::vec2(x_upper - x_lower, y_upper - y_lower));
    float angle = std::atan2(direction.y, direction.x);

    float timeStep = 0.1f;
    float totalTime = 0.0f;
    float maxTime = 5.0f;
    float gravity = Projectile::gravityForce;

    for (size_t i = 0; i < beamPoints.size(); ++i) {
        if (totalTime > maxTime) break;

        float x = launchPosition.x + Projectile::magnitude * cos(angle) * totalTime;
        float y = launchPosition.y + Projectile::magnitude * sin(angle) * totalTime - 0.5f * gravity * totalTime * totalTime;


        if (terrain->IsTerrainHitByProjectile(Point2D(x, y))) {
            // Daca punctul atinge terenul, marcam toate punctele urmatoare ca invizibile
            for (size_t j = i; j < beamPoints.size(); ++j) {
                beamPoints[j].isVisible = false;
            }
            break;
        }

        beamPoints[i].center.x = x;
        beamPoints[i].center.y = y;
        beamPoints[i].isVisible = true;

        object_2D::UpdateCircle(beamPoints[i].appearance.mesh,
            glm::vec3(beamPoints[i].center.x, beamPoints[i].center.y, 0.0f),
            beamPoints[i].radius,
            beamPoints[i].appearance.color,
            true);

        totalTime += timeStep;
    }
}
