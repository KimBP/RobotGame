/*
 * Sheduler.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#include "Scheduler.h"
#include "Trigonometry.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include "Logger.h"
#include "Viewer.h"
#include "RobotPosEvent.h"
namespace RobotGame {


// Singleton
Scheduler& Scheduler::getScheduler() {
	static Scheduler instance;
	return instance;
}

/* Schedule out current task */
void Scheduler::end(RobCtrl* robCtrl)
{
	Scheduler& inst = getScheduler();
	Logger::Log(robCtrl->getRobot(), "*** Ended tour");
	inst.schedulerMtx.unlock();
	robCtrl->lock();
}

void Scheduler::end()
{
	Scheduler& inst = getScheduler();
	Logger::Log("Viewer", "Frame update end");
	inst.schedulerMtx.unlock();
}

Scheduler::~Scheduler() {
	// TODO Auto-generated destructor stub
}


bool Scheduler::addRobot( Robot* (*getRobot)(RobCtrl* robCtrl))
{
	Scheduler& inst = getScheduler();

	RobCtrl* robCtrl = new RobCtrl(100,100*(inst.robCnt+1));
	Robot* robot = (*getRobot)(robCtrl);
	robCtrl->setRobot(robot);
	inst.robots.push_back(robCtrl);
	inst.threads.push_back(std::thread(RobCtrl::Runner, robCtrl));
	++inst.robCnt;
	return true;
}

RobCtrl* Scheduler::iterateRobots(RobCtrl* prev)
{
	Scheduler& inst = getScheduler();
	static std::vector<RobCtrl*>::iterator robIt;

	if (prev == 0) {
		robIt = inst.robots.begin();
		return (*robIt);
	} else {
		if (prev != (*robIt)) {
			Logger::LogError("iterateRobots fail");
			robIt = inst.robots.begin();
		}
		robIt++;
		if (robIt == inst.robots.end())
			return 0;
		else
			return (*robIt);
	}
}

void Scheduler::tickEnd()
{
	Logger::LogHead(std::string("Ending tick ") + std::to_string(tick));
	RobCtrl* robCtrl;
	/* Iterate over all robots and update everything */
	robCtrl = iterateRobots(0);
	int id = 0;
	while (0 != robCtrl) {
		robCtrl->tick(tick);
		Viewer::PostEvent(new RobotPosEvent(id, robCtrl->getX(), robCtrl->getY()));
		id++;
		robCtrl = iterateRobots(robCtrl);
	}

	CannonShell::tick(tick);
	Viewer::tick(tick);
	schedulerMtx.lock(); // Wait for frame update


	++tick;


}

void Scheduler::run()
{
	schedulerMtx.lock();
	while (1) {
		std::vector<RobCtrl*>::iterator robIt;
		for (robIt = robots.begin(); robIt != robots.end(); ++robIt) {
			if ((*robIt)->getArmor()) {
				Logger::Log((*robIt)->getRobot(), " *** Scheduled in");
				(*robIt)->unlock();
				schedulerMtx.lock();
			}
		}
		tickEnd();
	}

	// TODO join
}

} /* namespace RobotGame */
