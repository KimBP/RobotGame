/*
 * EdgePatrol.h
 *
 * Edge Patrol Robot Sample Plugin for RobotGame
 * Patrols arena edges, shoots at center, and adapts to threats
 */

#pragma once

#include "Robot.h"
#include <string>

namespace RobotGame {

class EdgePatrol: public Robot {
public:
	EdgePatrol(RobCtrl* robCtrl);
	virtual ~EdgePatrol() = default;

	virtual struct tune_t init(std::string& name);
	virtual void run();
	virtual std::string name() const { return myName; };

private:
	// Patrol state enumeration
	enum PatrolState {
		REACH_EDGE,
		PATROL_TOP_EDGE,
		PATROL_RIGHT_EDGE,
		PATROL_BOTTOM_EDGE,
		PATROL_LEFT_EDGE,
		AVOID_OBSTACLE
	};

	// Targeting state enumeration
	enum TargetMode {
		TARGET_CENTER,
		TARGET_THREAT
	};

	// Member variables
	PatrolState patrolState;
	TargetMode targetMode;
	armor_t lastArmor;
	range_t lastThreatRange;
	angle_t lastThreatAngle;
	int shotsInAir;

	// Configuration parameters (compile-time defaults)
	static const RobotGame::precision_t detectionAngle = 10;
	static const RobotGame::range_t detectionRange = 6000;
	static const RobotGame::range_t edgeDistance = 800;
	static const RobotGame::range_t cornerThreshold = 500;

	// Static tuning configuration
	static const struct RobotGame::tune_t myTune;
	static const std::string myName;
};

} // namespace RobotGame
