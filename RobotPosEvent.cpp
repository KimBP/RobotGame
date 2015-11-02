/*
 * RobotPosEvent.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "RobotPosEvent.h"

namespace RobotGame {

RobotPosEvent::RobotPosEvent(posx_t x, posy_t y)
: RobEvent::RobEvent(), posX(x), posY(y)
{
}

RobotPosEvent::~RobotPosEvent() {
}

posx_t RobotPosEvent::getX() const
{
	return posX;
}

posy_t RobotPosEvent::getY() const
{
	return posY;
}

} /* namespace RobotGame */
