/*
 * Sheduler.h
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#pragma once

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
	void setStepMode(bool enabled);
	// Enable/disable step mode. When enabled, pauseForStepMode() will wait for user input after each tick.
	// This is useful for debugging robot behavior frame-by-frame.
	void pauseForStepMode();
	void run();
	unsigned int getTick() { return tick;}

private:
	Scheduler() :
		robCnt(0),
		stepModeEnabled(false) {};
	virtual ~Scheduler() = default;
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
	bool stepModeEnabled;
	bool stepPaused; // true while scheduler is waiting for user input
	std::mutex schedulerMtx;

public:
	// Accessors for step mode state (used by viewer)
	bool isStepModeEnabled() const { return stepModeEnabled; }
	bool isStepPaused() const { return stepPaused; }

};

} /* namespace RobotGame */
