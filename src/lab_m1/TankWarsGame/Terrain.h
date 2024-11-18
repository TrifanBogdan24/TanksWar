#pragma once

#include <vector>

#include "Point2D.h"
#include "Terrain.h"
#include "object_2D.h"
#include "transform2D.h"
#include "object_2D.h"
#include "utils/gl_utils.h"

#include "Projectile.h"

#include <set>

// Forward declare the class
class Projectile;


class Terrain
{
public:
    struct SinCoeff
    {
        float amplitude;   // Alphas
        float frequency;   // Omegas

        // Constructors
        SinCoeff() : amplitude(0.0f), frequency(0.0) {}
        SinCoeff(float alpha, float omega) : amplitude(alpha), frequency(omega) {}
        ~SinCoeff() = default;
    };

    struct ComposedSinusoidalFunction
    {
        // functie compusa din mai multe functii sinusoidale
        float baseOffset;
        std::vector<SinCoeff> sinFunctions;

        ComposedSinusoidalFunction(): baseOffset(0), sinFunctions() {}
        ComposedSinusoidalFunction(float baseOffset_val, std::vector<SinCoeff> sinFunctions_val)
            : baseOffset(baseOffset_val), sinFunctions(sinFunctions_val) {}
        ~ComposedSinusoidalFunction() = default;
    };



public:
    float terrainWidth;
    std::vector<ComposedSinusoidalFunction> allPossibleTerrainFunctions;
    ComposedSinusoidalFunction selectedTerrainFunction;
    glm::vec3 terrainColor;
    std::vector<Point2D> terrainVertices;
    std::vector<Mesh*> terrainMeshes;

private:
    std::set<int> indicesOfAlreadySelectedTerrains;


public:
    static Terrain* GetInstance(float terrainWidth = 0.0f);
    ~Terrain();

private:
    Terrain(float terrainWidth = 0.0f);
    static Terrain* instance;

public:
    static std::vector<ComposedSinusoidalFunction> GetAllPossibleTerrainFunctions();
    void SetRandomTerrainFunction();
    void ComputeTerrainVertices();
    void CreateTerrainMeshes();

private:
    float ComputeSinusoidalFunctionSlope(float x);
    float ComputeTerrainHeightWithCurvature(float x);

public:
    float GetHeight(float x);
    float GetSlope(float x);

public:
    void ComputeLandslide(float deltaTimeSeconds);
    bool IsTerrainHitByProjectile(Point2D projectileCenter);
    void DemageTerrainByProjectile(Projectile* projectile);

};

