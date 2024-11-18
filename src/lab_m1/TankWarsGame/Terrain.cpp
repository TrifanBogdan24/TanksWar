#include <vector>

#include <cstdlib>
#include <random>
#include <ctime>


#include "Point2D.h"
#include "Terrain.h"

#include "object_2D.h"
#include "lab_m1/TankWarsGame/transform2D.h"
#include "lab_m1/TankWarsGame/object_2D.h"
#include "utils/gl_utils.h"
#include "Projectile.h"


#define STEP 1.0f

#define LAND_SLIDE_SPEED 20.f
#define LAND_SLIDE_THRESHOLD 0.1f

#define MIN_HEIGHT 20.f

#define MAX_DAMAGE_IN_HEIGHT 100.0f
#define TERRAIN_PROJECTILE_DAMAGE_RADIUS 25.f

using ComposedSinusoidalFunction = Terrain::ComposedSinusoidalFunction;
using SinCoeff = Terrain::SinCoeff;

Terrain* Terrain::instance = nullptr;

Terrain* Terrain::GetInstance(float terrainWidth)
{
    if (instance == nullptr)
    {
        instance = new Terrain(terrainWidth);
    }
    return instance;
}


Terrain::Terrain(float terrainWidth_val)
{
    this->terrainWidth = (terrainWidth_val > 0.f) ? terrainWidth_val : 0.f;
    this->allPossibleTerrainFunctions = this->GetAllPossibleTerrainFunctions();
}

Terrain::~Terrain()
{
    delete instance;
    instance = nullptr;
}



std::vector<ComposedSinusoidalFunction> Terrain::GetAllPossibleTerrainFunctions()
{


    std::vector<ComposedSinusoidalFunction>allPossibleTerrains = {
        // Coeficienti pentru functiile sinusoidale
        // Check them on https://www.geogebra.org

        // THE BEST: f(x)=300+120*sin(0.009*x)+80*sin(0.023*x)+10*sin(0.08*x)
        ComposedSinusoidalFunction(300.0f, { SinCoeff(120.0f, 0.009f), SinCoeff(80.0f, 0.023f), SinCoeff(10.0f, 0.08f) }),

        // f(x)=200+150*sin(0.002*x)+180*sin(0.005*x)+50*sin(0.01*x)
        ComposedSinusoidalFunction(200.0f, { SinCoeff(150.0f, 0.002f), SinCoeff(180.0f, 0.005f), SinCoeff(50.0f, 0.01f) }),

        // f(x)=250+100*sin(0.003*x)+170*sin(0.006*x)+70*sin(0.012*x)
        ComposedSinusoidalFunction(250.0f, { SinCoeff(100.0f, 0.003f), SinCoeff(170.0f, 0.006f), SinCoeff(70.0f, 0.012f) }),

        // f(x)=300+120*sin(0.004*x)+150*sin(0.007*x)+40*sin(0.014*x)
        ComposedSinusoidalFunction(300.0f, { SinCoeff(120.0f, 0.004f), SinCoeff(150.0f, 0.007f), SinCoeff(40.0f, 0.014f) }),

        // f(x)=280+180*sin(0.0035*x)+160*sin(0.008*x)+60*sin(0.015*x)
        ComposedSinusoidalFunction(280.0f, { SinCoeff(180.0f, 0.0035f), SinCoeff(160.0f, 0.008f), SinCoeff(60.0f, 0.015f) }),

        // f(x)=220+200*sin(0.005*x)+120*sin(0.009*x)+50*sin(0.016*x)
        ComposedSinusoidalFunction(280.0f, { SinCoeff(200.0f, 0.005f), SinCoeff(120.0f, 0.009f), SinCoeff(50.0f, 0.016f) }),

        // f(x)=240+160*sin(0.0025*x)+170*sin(0.0055*x)+90*sin(0.01*x)
        ComposedSinusoidalFunction(240.0f, { SinCoeff(160.0f, 0.0025f), SinCoeff(170.0f, 0.0055f), SinCoeff(90.0f, 0.01f) }),

        // f(x)=270+150*sin(0.003*x)+180*sin(0.006*x)+80*sin(0.011*x)
        ComposedSinusoidalFunction(270.0f, { SinCoeff(150.0f, 0.003f), SinCoeff(180.0f, 0.006f), SinCoeff(80.0f, 0.011f) }),

        // f(x)=300+130*sin(0.004*x)+140*sin(0.0075*x)+60*sin(0.012*x)
        ComposedSinusoidalFunction(300.0f, { SinCoeff(130.0f, 0.004f), SinCoeff(140.0f, 0.0075f), SinCoeff(60.0f, 0.012f) }),

        // f(x)=260+170*sin(0.0028*x)+180*sin(0.0065*x)+100*sin(0.0125*x)
        ComposedSinusoidalFunction(260.0f, { SinCoeff(170.0f, 0.0028f), SinCoeff(180.0f, 0.0065f), SinCoeff(100.0f, 0.0125f) }),

        // f(x)=310+160*sin(0.0045*x)+120*sin(0.008*x)+70*sin(0.014*x)
        ComposedSinusoidalFunction(310.0f, { SinCoeff(160.0f, 0.0045f), SinCoeff(120.0f, 0.008f), SinCoeff(70.0f, 0.014f) })
    };

    return allPossibleTerrains;
}


void Terrain::SetRandomTerrainFunction()
{
    if (this->allPossibleTerrainFunctions.empty()) {
        throw std::invalid_argument("The terrain functions vector is empty.");
    }

    std::srand(static_cast<unsigned int>(std::time(NULL)));

    int randomIndex = std::rand() % allPossibleTerrainFunctions.size();


    if (this->indicesOfAlreadySelectedTerrains.count(randomIndex) > 0) {
        // The terrain was plotted before (the set already contains the index)
        randomIndex = std::rand() % allPossibleTerrainFunctions.size();
    }


    if (indicesOfAlreadySelectedTerrains.size() >= allPossibleTerrainFunctions.size()) {
        indicesOfAlreadySelectedTerrains.clear();
    }

    // Marking the terrain with index "randomIndex" as plotted
    // We DON'T want to see this terrain again too soon
    indicesOfAlreadySelectedTerrains.insert(randomIndex);

    this->selectedTerrainFunction = allPossibleTerrainFunctions[randomIndex];
}


// Functioneaza doar formei initiale a terenului (atunci cand nu sufera deformari)
float Terrain::ComputeTerrainHeightWithCurvature(float x)
{
    float f_x = this->selectedTerrainFunction.baseOffset;
    for (SinCoeff f : this->selectedTerrainFunction.sinFunctions) {
        f_x += f.amplitude * sin(f.frequency * x);
    }

    return f_x;
}

// Functioneaza doar formei initiale a terenului (atunci cand nu sufera deformari)
float Terrain::ComputeSinusoidalFunctionSlope(float x)
{
    float slope = 0.0f;
    for (const auto& sinCoeff : this->selectedTerrainFunction.sinFunctions) {
        // Derivata functiei sinusoidale
        slope += sinCoeff.amplitude * sinCoeff.frequency * cos(sinCoeff.frequency * x);
    }
    return slope;
}



void Terrain::ComputeTerrainVertices()
{
    if (this->terrainWidth <= 0) {
        return;
    }

    // Variabile pentru teren
    this->terrainVertices.clear();


    for (float x = 0; x < this->terrainWidth + STEP; x += STEP) {
        float y = ComputeTerrainHeightWithCurvature(x);
        terrainVertices.push_back(Point2D(x, y));
    }
}



void Terrain::CreateTerrainMeshes()
{
    if (terrainWidth <= 0) {
        return;
    }

    this->terrainMeshes.clear();


    for (size_t x = 0; x < terrainWidth + STEP; x += STEP) {
        this->terrainMeshes.push_back(object_2D::CreateRectangle("terrainSegment", glm::vec3(0, 0, 0), 1, 1, this->terrainColor));
    }
}





void Terrain::ComputeLandslide(float deltaTimeSeconds)
{
    for (int i = 0; i < this->terrainVertices.size() - 1; ++i) {
        float dx = terrainVertices[i + 1].x - terrainVertices[i].x;
        float dy = terrainVertices[i + 1].y - terrainVertices[i].y;

        if (std::abs(dx) < 0.001f) {
            dx = 0.001f;
        }

        float slope = dy / dx;

        float slopeFactor = std::min(std::abs(slope), 10.0f);
        float dist = LAND_SLIDE_SPEED * deltaTimeSeconds * slopeFactor;

        float heightDifference = terrainVertices[i + 1].y - terrainVertices[i].y;

        if (std::abs(heightDifference) > 0.001f) {
            float adjustment = std::min(dist, std::abs(heightDifference));

            if (heightDifference > 0) {
                terrainVertices[i + 1].y -= adjustment / 2.0f;
                terrainVertices[i].y += adjustment / 2.0f;
            }
            else {
                terrainVertices[i + 1].y += adjustment / 2.0f;
                terrainVertices[i].y -= adjustment / 2.0f;
            }
        }
    }
}



float Terrain::GetSlope(float x)
{
    int i = static_cast<int>((x - terrainVertices[0].x) / STEP);

    if (i < 0 || i >= terrainVertices.size() - 1) {
        return 0;
    }

    // Calculate the slope
    float dx = STEP;
    return (terrainVertices[i + 1].y - terrainVertices[i].y) / dx;
}

float Terrain::GetHeight(float x)
{
    int i = static_cast<int>((x - terrainVertices[0].x) / STEP);

    if (i < 0 || i >= terrainVertices.size() - 1) {
        return 0;
    }

    Point2D p1 = terrainVertices[i];
    Point2D p2 = terrainVertices[i + 1];

    // Liniar interpolation
    float t = (x - p1.x) / STEP;
    return p1.y + t * (p2.y - p1.y);
}



bool Terrain::IsTerrainHitByProjectile(Point2D projectileCenter)
{
    int i = static_cast<int>((projectileCenter.x - terrainVertices[0].x) / STEP);

    if (i < 0 || i >= terrainVertices.size() - 1) {
        return false;
    }

    Point2D p1 = terrainVertices[i];
    Point2D p2 = terrainVertices[i + 1];

    // Liniar interpolation
    float t = (projectileCenter.x - p1.x) / STEP;
    float terrainHeightAtProjectileX = p1.y + t * (p2.y - p1.y);

    if (projectileCenter.y <= terrainHeightAtProjectileX) {
        return true;
    }

    return false;
}



void Terrain::DemageTerrainByProjectile(Projectile* projectile)
{
    float damageRadius = TERRAIN_PROJECTILE_DAMAGE_RADIUS;
    float damageRadiusSquared = damageRadius * damageRadius;
    float projectileX = projectile->position.x;
    float projectileY = projectile->position.y;

    for (int i = 0; i < terrainVertices.size(); ++i) {
        float dx = terrainVertices[i].x - projectileX;

        if (dx >= -damageRadius && dx <= damageRadius) {
            float localHeightSquared = damageRadiusSquared - dx * dx;

            if (localHeightSquared >= 0) {
                float localHeight = std::sqrt(localHeightSquared);
                float semicircleY = projectileY - localHeight;

                if (terrainVertices[i].y > semicircleY) {
                    terrainVertices[i].y = std::max(MIN_HEIGHT, semicircleY);
                }
            }
        }
    }
}

