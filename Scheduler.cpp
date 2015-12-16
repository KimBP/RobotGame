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
#include "RobotDataEvent.h"
#include <chrono>
#include <thread>

namespace RobotGame {


// Singleton
Scheduler& Scheduler::getScheduler() {
	static Scheduler instance;

	srand (time(NULL));
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

//	const int borderMargin = 100;
//    const int minDistance =  1000;
	int x;
	int y;
//	bool retry = true;
//
//	do {
//		retry = false;
//		x = rand() % (MAX_POS_X - borderMargin) - borderMargin;
//		y = rand() % (MAX_POS_Y - borderMargin) - borderMargin;
//
//		std::vector<RobCtrl*>::iterator robIt;
//		for (robIt = inst.robots.begin(); robIt != inst.robots.end(); ++robIt) {
//			unsigned int dist = Trigonometry::distance(x,y, (*robIt)->getX(), (*robIt)->getY());
//			if (dist < minDistance) {
//				retry = true;
//			}
//		}
//	} while (retry);

	if (inst.robots.size() == 0) {
		x = 150;
		y = 150;
	} else
	{
		x = 100;
		y = 100;
	}

	RobCtrl* robCtrl = new RobCtrl(x,y);
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


	robCtrl = iterateRobots(0);
	id = 0;
	while (0 != robCtrl) {
		Viewer::PostEvent(new RobotDataEvent(id, robCtrl->getName(),
						robCtrl->getArmor(), robCtrl->getEnergy()));
		id++;
		robCtrl = iterateRobots(robCtrl);
	}

	Viewer::tick(tick);
	schedulerMtx.lock(); // Wait for frame update


	++tick;

	robots.erase(
			std::remove_if(
					robots.begin(),
					robots.end(),
					[] (RobCtrl* rob) {
						if (rob->getArmor() == 0) {
							rob->doDie();
							return true;
						}
						return false;
					}
			),
			robots.end()
	);

	//std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Scheduler::run()
{
	schedulerMtx.lock();
	while (robots.size() > 1) {
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

	Logger::LogHead("Game over");
	Logger::Log(robots[0]->getName(), "WINNER");

	// Kill winner
	robots[0]->doDie();
	robots[0]->unlock();
	while(!threads.empty()) {
		threads.back().join();
		threads.pop_back();
	}


}

} /* namespace RobotGame */
