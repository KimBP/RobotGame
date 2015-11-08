/*
 * RobotTickEvent.h
 *
 *  Created on: Nov 8, 2015
 *      Author: kbp
 */

#ifndef ROBOTTICKEVENT_H_
#define ROBOTTICKEVENT_H_

#include "RobEvent.h"

namespace RobotGame {

class RobotTickEvent: public RobotGame::RobEvent {
public:
	RobotTickEvent(bool render)
	: RobEvent(0), render(render) {};
	virtual ~RobotTickEvent();

	virtual void execute();

	virtual posx_t getX() const { return 0; };
	virtual posy_t getY() const { return 0; };

private:
	bool render;
};

} /* namespace RobotGame */
#endif /* ROBOTTICKEVENT_H_ */
