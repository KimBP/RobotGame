/*
 * Scanner.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: kbp
 */

#include "Scanner.h"
#include "Logger.h"



extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl)
{
	static Scanner* instance = new Scanner(robCtrl);
	return instance;
}



const struct RobotGame::tune_t Scanner::myTune = {0 , 2 };

Scanner::Scanner(RobotGame::RobCtrl* robCtrl)
: RobotGame::Robot(robCtrl)
{
}

Scanner::~Scanner() {
	// TODO Auto-generated destructor stub
}

void Scanner::run()
{
	RobotGame::armor_t lastArmor = getArmor();
	RobotGame::angle_t lastScan = 0;
	RobotGame::range_t scanRange = 0;

	while (1) {
		if (lastArmor > getArmor()) {
			// Time to move
			RobotGame::Logger::Log(this, "Have been hurt - let's move");

			scanRange = 0;;
		} else if (scanRange) {
			// Shoot
			RobotGame::Logger::Log(this, "Scanned something - try shooting");
			cannon(lastScan, scanRange);
			scanRange = 0;
		} else {
			// Make new scan
			lastScan += 30;
			lastScan %= 360;
			scanRange = scan(lastScan, 10);
			if (scanRange < RobotGame::damageRanges[RobotGame::DAMAGE_RANGE_CNT-1])
				scanRange = 0; // Too close don't shoot
		}
	}
}
