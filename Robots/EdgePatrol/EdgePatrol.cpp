/*
 * EdgePatrol.cpp
 *
 * Edge Patrol Robot Sample Plugin for RobotGame
 * Patrols arena edges, shoots at center, and adapts to threats
 */

#include "EdgePatrol.h"
#include "Logger.h"
#include "Trigonometry.h"
#include <cstdlib>

extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl)
{
	static RobotGame::EdgePatrol* instance = new RobotGame::EdgePatrol(robCtrl);
	return instance;
}

const struct RobotGame::tune_t RobotGame::EdgePatrol::myTune = {1, 1};
const std::string RobotGame::EdgePatrol::myName("EdgePatrol");

RobotGame::EdgePatrol::EdgePatrol(RobotGame::RobCtrl* robCtrl)
: RobotGame::Robot(robCtrl),
  patrolState(REACH_EDGE),
  targetMode(TARGET_CENTER),
  lastArmor(getArmor()),
  lastThreatRange(0),
  lastThreatAngle(0),
  shotsInAir(0)
{
}

struct RobotGame::tune_t RobotGame::EdgePatrol::init(std::string& name) {
	name = myName;
	return myTune;
}

void RobotGame::EdgePatrol::run()
{
	int centerX = RobotGame::MAX_POS_X / 2;
	int centerY = RobotGame::MAX_POS_Y / 2;

#define STAY_H  0
#define STAY_V  0
#define GO_N   1
#define GO_S   2
#define GO_W   4
#define GO_E   8

	int horz, vert;
	angle_t direction;

	while (1) {
		if (lastArmor > getArmor()) {
			RobotGame::Logger::Log(this, std::string("Have been hurt ") +
										 std::to_string(lastArmor) +
										 std::string(" -> ") +
										 std::to_string(getArmor()));
			lastArmor = getArmor();
		}

		RobotGame::Logger::Log(this, std::string("Current position(") +
										 std::to_string(getX()) +
										 std::string(",") +
										 std::to_string(getY()) +
										 std::string(")"));
		RobotGame::Logger::Log(this, std::string("Current speed: ") +
										 std::to_string(getSpeed()) +
										 std::string(" direction: ") +
										 std::to_string(getDirection()));

		int myX = getX();
		int myY = getY();

		switch (patrolState) {
		case REACH_EDGE: {
			if (myX < edgeDistance) {
				horz = GO_E;
			} else if (myX > RobotGame::MAX_POS_X - edgeDistance) {
				horz = GO_W;
			} else if (myY < edgeDistance) {
				vert = GO_S;
			} else if (myY > RobotGame::MAX_POS_Y - edgeDistance) {
				vert = GO_N;
			} else {
				patrolState = PATROL_TOP_EDGE;
			}
			break;
		}

		case PATROL_TOP_EDGE: {
			if (myX < cornerThreshold || myX > RobotGame::MAX_POS_X - cornerThreshold) {
				patrolState = PATROL_RIGHT_EDGE;
			} else {
				horz = GO_E;
			}
			break;
		}

		case PATROL_BOTTOM_EDGE: {
			if (myX < cornerThreshold || myX > RobotGame::MAX_POS_X - cornerThreshold) {
				patrolState = PATROL_LEFT_EDGE;
			} else {
				horz = GO_E;
			}
			break;
		}

		case PATROL_RIGHT_EDGE: {
			if (myY < cornerThreshold || myY > RobotGame::MAX_POS_Y - cornerThreshold) {
				patrolState = PATROL_BOTTOM_EDGE;
			} else {
				vert = GO_S;
			}
			break;
		}

		case PATROL_LEFT_EDGE: {
			if (myY < cornerThreshold || myY > RobotGame::MAX_POS_Y - cornerThreshold) {
				patrolState = PATROL_TOP_EDGE;
			} else {
				vert = GO_N;
			}
			break;
		}

		case AVOID_OBSTACLE: {
			int tempX, tempY;
			RobotGame::Trigonometry::move(myX, myY, getDirection() + 90, 2000, &tempX, &tempY);

			myX = tempX;
			myY = tempY;

			if (myX < edgeDistance) {
				patrolState = PATROL_LEFT_EDGE;
			} else if (myX > RobotGame::MAX_POS_X - edgeDistance) {
				patrolState = PATROL_RIGHT_EDGE;
			} else if (myY < edgeDistance) {
				patrolState = PATROL_TOP_EDGE;
			} else {
				patrolState = PATROL_BOTTOM_EDGE;
			}
			break;
		}
		}

		switch (vert + horz) {
		case 1:
			direction = 0;
			break;
		case 2:
			direction = 180;
			break;
		case 4:
			direction = 270;
			break;
		case 8:
			direction = 90;
			break;
		case 5:
			direction = 315;
			break;
		case 6:
			direction = 225;
			break;
		case 9:
			direction = 45;
			break;
		case 10:
			direction = 135;
			break;
		case 0:
		default:
			direction = 360;
			break;
		}

		angle_t shootDirection;

		if (targetMode == TARGET_CENTER) {
			shootDirection = RobotGame::Trigonometry::direction(getX(), getY(), centerX, centerY);
		} else if (targetMode == TARGET_THREAT) {
			shootDirection = lastThreatAngle;
		}

		if (getEnergy() >= RobotGame::SHOOT_ENERGY_COST && shotsInAir < RobotGame::MAX_SHOOT) {
			int shotsFired = cannon(shootDirection, detectionRange);
			if (shotsFired) {
				shotsInAir++;
			}
		}

		skip();
	}
}
