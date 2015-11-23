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
#include "CannonPosEvent.h"
#include "Viewer.h"
#include <algorithm> // For std::remove_if

namespace RobotGame {

unsigned int CannonShell::nextId = 0;
std::vector<CannonShell> CannonShell::shells;

void CannonShell::FireShell(RobCtrl* owner,
					posx_t posX, posy_t posY,
					angle_t direction, range_t range)
{
	shells.push_back(CannonShell(owner, posX, posY, direction, range));
}

CannonShell::CannonShell(RobCtrl* owner, posx_t posX, posy_t posY,
		angle_t direction, range_t range)
: owner(owner), prevX(posX), prevY(posY),
  startTick(Scheduler::getScheduler().getTick()),
  currX(posX), currY(posY),
  direction(direction), range(range),
  blast(false), id(++nextId)
{
}

CannonShell::~CannonShell() {
	// TODO Auto-generated destructor stub
}


void CannonShell::tick(unsigned int tick) {
	std::vector<CannonShell>::iterator shellIt;

	for (shellIt = shells.begin(); shellIt != shells.end(); ++shellIt) {
		(*shellIt)._tick(tick);
		if ((*shellIt).blasted()) {
			RobCtrl* robCtrl = Scheduler::iterateRobots(0);
			while (robCtrl != 0) {
				robCtrl->shotBlasted((*shellIt).owner, (*shellIt).getX(), (*shellIt).getY());
				robCtrl = Scheduler::iterateRobots(robCtrl);
			}

		}
		Viewer::PostEvent(
				new CannonPosEvent(
						(*shellIt).id,
						(*shellIt).prevX, (*shellIt).prevY,
						(*shellIt).currX, (*shellIt).currY,
						(*shellIt).blast));
	}

	// TODO: Clean up all blasted shells
	shells.erase(
			std::remove_if(
					shells.begin(),
					shells.end(),
					[] (CannonShell& shell) { return shell.blasted();}
			),
			shells.end()
	);
}

bool CannonShell::_tick(unsigned int tick) {

	range_t tickDistance = static_cast<range_t>(SHELL_SPEED);
	unsigned int distance = std::min(range, tickDistance);
	prevX = currX;
	prevY = currY;
	Trigonometry::move(prevX, prevY, direction, distance, &currX, &currY);
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
