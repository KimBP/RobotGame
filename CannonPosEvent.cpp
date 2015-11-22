/*
 * RobotPosEvent.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "CannonPosEvent.h"
#include "Viewer.h"

namespace RobotGame {

CannonPosEvent::CannonPosEvent(int id, posx_t x1, posy_t y1, posx_t x2, posy_t y2, bool blasted)
: RobEvent::RobEvent(id), posX1(x1), posY1(y1), posX2(x2), posY2(y2), blasted(blasted)
{
}

CannonPosEvent::~CannonPosEvent() {
}

void CannonPosEvent::execute()
{
	Viewer::CannonShow(getId(), posX1, posY1, posX2, posY2, blasted);
}


} /* namespace RobotGame */
