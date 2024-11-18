#pragma once

#include "Point2D.h"
#include "Appearance.h"    // Group for Appearance (Color and Mesh)
#include "Tank.h"
#include "Terrain.h"



class Projectile {

public:
	Point2D position;
	Appearance appearance;
	glm::vec2 velocity;
	float radius;
	float tankImpactDamage;
	bool isVisible;

	static float magnitude;
	static float gravityForce;

public:
	Projectile();
	~Projectile();
	
public:
	// Initializes velocity
	void LaunchProjectile(Tank &tank);
	void UpdateProjectilePosition(float deltaTime, Tank &tank1, Tank &tank2);
	void CheckIfHitsTerrain();
	void CheckIfHitsTank(float deltaTimeSeconds, Tank &tank1);

	void UpdateMesh();
};