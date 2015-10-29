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

namespace RobotGame {


// Singleton
Scheduler& Scheduler::getScheduler() {
	static Scheduler instance;
	return instance;
}

/* Schedule out out current task */
void Scheduler::end(RobCtrl* robCtrl)
{
	Scheduler& inst = getScheduler();
	Logger::Log(robCtrl->getRobot(), "*** Ended tour");
	inst.schedulerMtx.unlock();
	robCtrl->lock();
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

bool Scheduler::addShoot(RobCtrl* shooter, unsigned int x, unsigned int y, angle_t direction, range_t range)
{
	Scheduler& inst = getScheduler();

	CannonShell shell(shooter, x, y, direction, range);
	inst.shells.push_back(shell);

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
	++tick;
	Logger::LogHead(std::string("Starting tick") + std::to_string(tick));

	/* Iterate over all robots and update everything */
	std::vector<RobCtrl*>::iterator robIt;

	for (robIt = robots.begin(); robIt != robots.end(); ++robIt) {
		(*robIt)->tick(tick);
	}

	std::vector<CannonShell>::iterator shellIt;
	for (shellIt = shells.begin(); shellIt != shells.end(); ++shellIt) {
		shellIt->tick(tick);
		if (shellIt->blast()) {
			determineDamage(shellIt->getX(), shellIt->getY());
		}
	}

	// Clean up
// TODO: Won't compile - get back later
//	robots.erase(
//			std::remove_if(
//					robots.begin(),
//					robots.end(),
//					[] (const RobCtrl* rob) { (rob->getArmor() == 0);}
//			),
//			robots.end()
//	);

	shells.erase(
			std::remove_if(
					shells.begin(),
					shells.end(),
					[] (const CannonShell& shell) { return shell.blast();}
			),
			shells.end()
	);
}

void Scheduler::determineDamage(unsigned int x, unsigned int y)
{
	std::vector<RobCtrl*>::iterator robIt;
	for (robIt = robots.begin(); robIt != robots.end(); ++robIt) {
		unsigned int robX = (*robIt)->getX();
		unsigned int robY = (*robIt)->getY();

		range_t dist = Trigonometry::distance(x,y, robX, robY);
		for (unsigned int i=0; i < DAMAGE_RANGE_CNT; i++) {
			if (dist <= damageRanges[i] ) {
				(*robIt)->damage(damageVals[i]);
				break;
			}
		}
	}
}

void Scheduler::run()
{
	schedulerMtx.lock();
	while (1) {
		std::vector<RobCtrl*>::iterator robIt;
		for (robIt = robots.begin(); robIt != robots.end(); ++robIt) {
			Logger::Log((*robIt)->getRobot(), " *** Scheduled in");
			(*robIt)->unlock();
			schedulerMtx.lock();
		}
		tickEnd();
	}

	// TODO join
}

} /* namespace RobotGame */
