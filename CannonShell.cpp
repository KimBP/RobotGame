/*
 * CannonShell.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#include "CannonShell.h"
#include "Scheduler.h"
#include <stdlib.h>
#include "Trigonometry.h"
#include "Logger.h"

namespace RobotGame {

CannonShell::CannonShell(posx_t posX, posy_t posY,
		angle_t direction, range_t range)
: prevX(posX), prevY(posY), startTick(Scheduler::getScheduler().getTick()),
  currX(posX), currY(posY),
  direction(direction), range(range),
  blast(false)
{

}

CannonShell::~CannonShell() {
	// TODO Auto-generated destructor stub
}

bool CannonShell::tick(unsigned int tick) {

	range_t tickDistance = static_cast<range_t>(SHELL_SPEED);
	unsigned int distance = std::min(range, tickDistance);

	Trigonometry::move(currX, currY, direction, distance, &currX, &currY);
	range -= distance;
	if (range == 0) {
		Logger::Log(std::string("@¤¤@¤¤@"),
				std::string("Blast at (") +
				std::to_string(currX) +
				std::string(",") +
				std::to_string(currY) +
				std::string(")"));
		blast = true;
		return false;
	}
	return true;
}

} /* namespace RobotGame */
