/*
 * Sheduler.h
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>
#include "Robot.h"
#include "RobCtrl.h"
#include "CannonShell.h"
#include <thread>
#include <mutex>
#include <chrono>

namespace RobotGame {

class Scheduler {
public:
	static Scheduler& getScheduler();
	static void end(RobCtrl* robCtrl);
	static bool addRobot( Robot* (*getRobot)(RobCtrl* robCtrl));
	static RobCtrl* iterateRobots(RobCtrl* prev);
	static void end();
	void setBattleDelay(int delayMs);
	int getBattleDelay() const;
	void run();
	const unsigned int getTick() { return tick;}

private:
	Scheduler() :
		robCnt(0) {};
	virtual ~Scheduler();
	Scheduler(Scheduler const&) = delete;      // No copy
	void operator=(Scheduler const&) = delete; // No assignment

private:
	void tickEnd();

private:
	std::vector<RobCtrl*> robots;
	std::vector<std::thread> threads;

	unsigned int robCnt;
	unsigned int tick;
	int battleDelayMs = 16; // Default 16ms (60 FPS)
	std::mutex schedulerMtx;
};

} /* namespace RobotGame */
#endif /* SCHEDULER_H_ */
