/*
 * Runner.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: kbp
 */

#include "Runner.h"
#include "Logger.h"

extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl)
{
	static Runner* instance = new Runner(robCtrl);
	return instance;
}

const struct RobotGame::tune_t Runner::myTune = {2 , 0 };
const std::string Runner::myName("Runner");

Runner::Runner(RobotGame::RobCtrl* robCtrl)
: RobotGame::Robot(robCtrl)
{
}

Runner::~Runner() {
}

void Runner::run()
{
#define STAY_H 	0
#define STAY_V 	0
#define GO_N	1
#define GO_S	2
#define GO_W	4
#define GO_E	8

	int horz, vert;
	RobotGame::angle_t direction;
	RobotGame::armor_t lastArmor = getArmor();

	bool firstTime = true;

	while (1) {
		if (lastArmor > getArmor()) {
			RobotGame::Logger::Log(this, std::string("Have been hurt ")+
										 std::to_string(lastArmor) +
										 std::string(" -> ")+
										 std::to_string(getArmor()));

		}

		RobotGame::Logger::Log(this, std::string("Current position(")+
									 std::to_string(getX())+
									 std::string(",")+
									 std::to_string(getY())+
									 std::string(")"));
		RobotGame::Logger::Log(this, std::string("Current speed: ")+
									 std::to_string(getSpeed()) +
									 std::string(" direction: ") +
									 std::to_string(getDirection()));
		if (firstTime) {
			if (getX() < RobotGame::MAX_POS_X / 2) {
				horz = GO_E;
			} else {
				horz = GO_W;
			}
			if (getY() < RobotGame::MAX_POS_Y / 2) {
				vert = GO_S;
			} else {
				vert = GO_N;
			}
			firstTime = false;
		} else {
			if (getX() < 1000) {
				horz = GO_E;
			} else if (getX() > RobotGame::MAX_POS_X - 1000) {
				horz = GO_W;
			} else {
				horz = STAY_H;
			}
			if (getY() < 1000) {
				vert = GO_S;
			} else if (getY() > RobotGame::MAX_POS_Y - 1000) {
				vert = GO_N;
			} else {
				vert = STAY_V;
			}
		}

		switch (vert + horz) {
		case 1: // N
			direction = 0;
			break;
		case 2: // S
			direction = 180;
			break;
		case 4: // W
			direction = 270;
			break;
		case 5: // NW
			direction = 315;
			break;
		case 6: // SW
			direction = 225;
			break;
		case 8: // E
			direction = 90;
			break;
		case 9: // NE
			direction = 45;
			break;
		case 10: // SE
			direction = 135;
			break;
		case 0: // stay
		default: // Stay
			direction = 360; // No change
			break;
		}


		if (direction < 360) {
			drive(direction, RobotGame::MAX_SPEED);
		} else {
			skip();
		}
	}
}
