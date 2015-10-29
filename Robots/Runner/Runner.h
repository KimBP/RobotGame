/*
 * Runner.h
 *
 *  Created on: Oct 23, 2015
 *      Author: kbp
 */

#ifndef RUNNER_H_
#define RUNNER_H_

#include <Robot.h>

class Runner: public RobotGame::Robot {
public:
	Runner(RobotGame::RobCtrl* robCtrl);
	virtual ~Runner();

	virtual struct RobotGame::tune_t init() { return myTune; };
	virtual void run();
	virtual std::string name() const { return "RUNNER"; };

private:
	static const struct RobotGame::tune_t myTune;
};

#endif /* RUNNER_H_ */
