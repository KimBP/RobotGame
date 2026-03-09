/*
 * Sheduler.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#include "Scheduler.h"
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include "Trigonometry.h"
#include <string>
#include "Logger.h"
#include "Viewer.h"
#include "RobotPosEvent.h"
#include "RobotDataEvent.h"
#include <iostream>
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


bool Scheduler::addRobot( Robot* (*getRobot)(RobCtrl* robCtrl))
{
	Scheduler& inst = getScheduler();

	const int borderMargin = 100;
    const int minDistance =  1000;
	int x;
	int y;
	bool retry = true;

	do {
		retry = false;
		x = rand() % (MAX_POS_X - borderMargin) - borderMargin;
		y = rand() % (MAX_POS_Y - borderMargin) - borderMargin;

		std::vector<RobCtrl*>::iterator robIt;
		for (robIt = inst.robots.begin(); robIt != inst.robots.end(); ++robIt) {
			unsigned int dist = Trigonometry::distance(x,y, (*robIt)->getX(), (*robIt)->getY());
			if (dist < minDistance) {
				retry = true;
			}
		}
	} while (retry);

// Use this to start 2 robots at 'known' positions
//	if (inst.robots.size() == 0) {
//		x = 150;
//		y = 150;
//	} else
//	{
//		x = 100;
//		y = 100;
//	}

	RobCtrl* robCtrl = new RobCtrl(inst.robCnt,x,y);
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
	std::vector<int> deadRobots;
	while (0 != robCtrl) {
		bool stillAlive = robCtrl->tick(tick);
		if (stillAlive) {
			Viewer::PostEvent(new RobotPosEvent(robCtrl->getId(), robCtrl->getX(), robCtrl->getY()));
		} else {
			// Robot died - schedule for cleanup
			deadRobots.push_back(robCtrl->getId());
			Logger::LogDebug(std::string("Robot ") + std::to_string(robCtrl->getId()) + std::string(" died, scheduling cleanup"));
		}
		robCtrl = iterateRobots(robCtrl);
	}
	
	// Mark dead robots (keep them visible at 50% opacity)
	for (int deadId : deadRobots) {
		Viewer::markRobotDead(deadId);
	}

	CannonShell::tick(tick);


	robCtrl = iterateRobots(0);
	while (0 != robCtrl) {
		Viewer::PostEvent(new RobotDataEvent(robCtrl->getId(), robCtrl->getName(),
					robCtrl->getArmor(), robCtrl->getEnergy()));
		robCtrl = iterateRobots(robCtrl);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(battleDelayMs));

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
}

void Scheduler::run()
{
	// Initial lock to synchronize with viewer thread
	schedulerMtx.lock();

	// When step mode is enabled we want to show the initial robot positions
	// before any ticks occur.  Post events for the current state so the
	// viewer can render a starting frame.
	if (stepModeEnabled) {
		Logger::LogHead("Step mode initial render");
		for (RobCtrl* rob : robots) {
			Viewer::PostEvent(new RobotPosEvent(rob->getId(), rob->getX(), rob->getY()));
			Viewer::PostEvent(new RobotDataEvent(rob->getId(), rob->getName(), rob->getArmor(), rob->getEnergy()));
		}
		// Trigger the viewer and wait for it to finish drawing
		Viewer::tick(tick);
		schedulerMtx.lock();
		// Pause after the initial frame as well
		pauseForStepMode();
	}

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
		pauseForStepMode();  // Pause for user input in step mode, otherwise continue immediately
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

void Scheduler::setBattleDelay(int delayMs) {
	// Validate input range (1-1000ms)
	if (delayMs < 1 || delayMs > 1000) {
		// Use default (16ms) if invalid
		battleDelayMs = 16;
		// Log warning for invalid values
		Logger::LogHead("Warning: Invalid battle delay " + std::to_string(delayMs) + 
					   "ms, using default 16ms");
	} else {
		battleDelayMs = delayMs;
	}
}

int Scheduler::getBattleDelay() const {
	return battleDelayMs;
}

void Scheduler::setStepMode(bool enabled) {
	stepModeEnabled = enabled;
}

void Scheduler::pauseForStepMode() {
	if (!stepModeEnabled) {
		return;
	}

	schedulerMtx.unlock();

	std::cout << "\n[Step Mode] Tick " << tick << " - Press Enter to advance to next tick: ";
	std::cout.flush();
	std::string input;
	std::getline(std::cin, input);

	schedulerMtx.lock();
}

} /* namespace RobotGame */
