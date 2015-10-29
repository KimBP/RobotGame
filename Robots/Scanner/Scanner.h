/*
 * Scanner.h
 *
 *  Created on: Oct 23, 2015
 *      Author: kbp
 */

#ifndef SCANNER_H_
#define SCANNER_H_

#include <Robot.h>

class Scanner: public RobotGame::Robot {
public:
	Scanner(RobotGame::RobCtrl* robCtrl);
	virtual ~Scanner();

	virtual struct RobotGame::tune_t init() { return myTune; };
	virtual void run();
	virtual std::string name() const { return "SCANNER"; };

private:
	static const struct RobotGame::tune_t myTune;
};

#endif /* SCANNER_H_ */
