#include "Projectile.h"
#include "Appearance.h"    // Group for Appearance (Color and Mesh)
#include "Tank.h"
#include "Point2D.h"
#include "object_2D.h"


#define PROJECTILE_COLOR glm::vec3(78.f/255.f, 78.f/255.f, 82.f/255.f)
#define PROJECTILE_MAGNITUDE 330.f
#define GRAVITY_FORCE 200.f
#define RADIUS 4.0f
#define PROJECTILE_TANK_IMPACT_DAMAGE  7.5f      // Value to decrease tank life (0-100)

#include <iostream>

using namespace std;


float Projectile::magnitude = PROJECTILE_MAGNITUDE;
float Projectile::gravityForce = GRAVITY_FORCE;

Projectile::Projectile()
{
	this->appearance.mesh = object_2D::MemAllocMesh("Un nou proiectil");
	this->appearance.color = PROJECTILE_COLOR;
	this->velocity = glm::vec2(0.f, 0.f);
	this->radius = RADIUS;
	this->tankImpactDamage = PROJECTILE_TANK_IMPACT_DAMAGE;
	this->isVisible = false;
	this->position = Point2D();
}


Projectile::~Projectile() {}



// Initializes velocity
void Projectile::LaunchProjectile(Tank& tank)
{
	float x_lower = (tank.turret.bottomLeft.x + tank.turret.bottomRight.x) / 2.f;
	float y_lower = (tank.turret.bottomLeft.y + tank.turret.bottomRight.y) / 2.f;
	float x_upper = (tank.turret.topLeft.x + tank.turret.topRight.x) / 2.f;
	float y_upper = (tank.turret.topLeft.y + tank.turret.topRight.y) / 2.f;

	glm::vec2 middlePointA(x_lower, y_lower);
	glm::vec2 middlePointB(x_upper, y_upper);

	glm::vec2 direction = middlePointB - middlePointA;

	// Calculate the angle between the two points using atan2
	float angle = std::atan2(direction.y, direction.x);

	// Initialize velocity by v = [cos(angle), sin(angle)] * magnitudine
	this->velocity = PROJECTILE_MAGNITUDE * glm::vec2(cos(angle), sin(angle));

	this->position.x = middlePointB.x + RADIUS * cos(angle);
	this->position.y = middlePointB.y + RADIUS * sin(angle);

	this->isVisible = true;
	UpdateMesh();
}

void Projectile::UpdateProjectilePosition(float deltaTime, Tank &tank1, Tank &tank2)
{
	if (!this->isVisible) {
		return;
	}

	// v =[cos(unghiTun) sin(unghiTun)] * magnitudine
	// v =v − g * deltaTime
	this->velocity.y -= GRAVITY_FORCE * deltaTime;
	this->position.x += this->velocity.x * deltaTime;
	this->position.y += this->velocity.y * deltaTime;


	this->CheckIfHitsTerrain();
	this->CheckIfHitsTank(deltaTime, tank1);
	this->CheckIfHitsTank(deltaTime, tank2);


	UpdateMesh();
}


void Projectile::CheckIfHitsTerrain()
{
	Terrain* terrain = Terrain::GetInstance();
	if (terrain->IsTerrainHitByProjectile(Point2D(this->position.x, this->position.y))) {
		terrain->DemageTerrainByProjectile(this);
		this->isVisible = false;
		return;
	}
}


void Projectile::CheckIfHitsTank(float deltaTimeSeconds, Tank &tank)
{
	if (tank.tankLife <= 0) {
		return;
	}

	if (tank.IsTankHitByProjectile(this) == true) {
		tank.DecreaseHitTankLife(this->tankImpactDamage);
		this->isVisible = false;
		return;
	}
}




void Projectile::UpdateMesh()
{
	object_2D::UpdateCircle(
		this->appearance.mesh,
		glm::vec3(this->position.x, this->position.y, 0.f),
		this->radius,
		this->appearance.color,
		true
	);
}



