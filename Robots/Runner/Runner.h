/*
 * Runner.h
 *
 *  Created on: Oct 23, 2015
 *      Author: kbp
 */

#pragma once

#include "Robot.h"

class Runner: public RobotGame::Robot {
public:
	Runner(RobotGame::RobCtrl* robCtrl);
	virtual ~Runner() = default;

	virtual struct RobotGame::tune_t init(std::string& name) {
		name = myName;
		return myTune;
	};
	virtual void run();
	virtual std::string name() const { return "RUNNER"; };

private:
	static const struct RobotGame::tune_t myTune;
	static const std::string myName;
};

