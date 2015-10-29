/*
 * CannonShell.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#include "CannonShell.h"
#include "Scheduler.h"
#include <stdlib.h>
//#include <algorithm>
//#include <math.h>
#include "Trigonometry.h"

namespace RobotGame {

CannonShell::CannonShell(RobCtrl* shooter,
		posx_t posX, posy_t posY,
		angle_t direction, range_t range)
: prevX(posX), prevY(posY), startTick(Scheduler::getScheduler().getTick()),
  currX(posX), currY(posY),
  direction(direction), range(range),
  shooter(shooter),  _blast(false)
{

}

CannonShell::~CannonShell() {
	// TODO Auto-generated destructor stub
}

void CannonShell::tick(unsigned int tick) {

	range_t tickDistance = static_cast<range_t>(SHELL_SPEED);
	unsigned int distance = std::min(range, tickDistance);

	Trigonometry::move(currX, currY, direction, distance, &currX, &currY);
	range -= distance;
	if (range == 0)
		_blast = true;
}


bool CannonShell::blast() const
{
	return _blast;
}

} /* namespace RobotGame */
