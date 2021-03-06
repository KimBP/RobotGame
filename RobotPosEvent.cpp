/*
 * RobotPosEvent.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "RobotPosEvent.h"
#include "Viewer.h"

namespace RobotGame {

RobotPosEvent::RobotPosEvent(int id, posx_t x, posy_t y)
: RobEvent::RobEvent(id), posX(x), posY(y)
{
}

RobotPosEvent::~RobotPosEvent() {
}

void RobotPosEvent::execute()
{
	Viewer::RobotShow(getId(), posX, posY);
}


} /* namespace RobotGame */
