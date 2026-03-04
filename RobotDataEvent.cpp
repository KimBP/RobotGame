/*
 * RobotDataEvent.cpp
 *
 *  Created on: Nov 28, 2015
 *      Author: kbp
 */

#include "RobotDataEvent.h"
#include "Viewer.h"

namespace RobotGame {


void RobotDataEvent::execute()
{
	Viewer::RobotDataShow(getId(), name, armor, energy);
}

} /* namespace RobotGame */
