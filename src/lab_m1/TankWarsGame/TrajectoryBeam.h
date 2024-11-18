#pragma once


#include "core/engine.h"
#include "utils/gl_utils.h"
#include "object_2D.h"
#include "Appearance.h"

#include "Terrain.h"
#include "Point2D.h"
#include "Tank.h"



using namespace std;

// Forward class definition
class Tank;

class TrajectoryBeam {

public:
	struct BeamPoint {
		Appearance appearance;
		Point2D center;
		float radius;
		bool isVisible;
	};

public:
	vector<BeamPoint> beamPoints;
	Tank* tank;

public:
	TrajectoryBeam();
	TrajectoryBeam(Tank *tank_value);
	~TrajectoryBeam();

private:
	void InitTrajectoryBeam();


public:
	void UpdateTankTrajectoryBeam();
};
