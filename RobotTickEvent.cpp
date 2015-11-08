/*
 * RobotTickEvent.cpp
 *
 *  Created on: Nov 8, 2015
 *      Author: kbp
 */

#include "RobotTickEvent.h"
#include "Viewer.h"

namespace RobotGame {


RobotTickEvent::~RobotTickEvent() {
	// TODO Auto-generated destructor stub
}

void RobotTickEvent::execute()
{
	if (render)
		Viewer::RenderArena();
	else
		Viewer::ClearArena();
}

}
/* namespace RobotGame */
