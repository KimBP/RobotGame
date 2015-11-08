/*
 * RobotPosEvent.h
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#ifndef ROBOTPOSEVENT_H_
#define ROBOTPOSEVENT_H_

#include "RobEvent.h"

namespace RobotGame {

class RobotPosEvent: public RobotGame::RobEvent {
public:
	RobotPosEvent(int id, posx_t x, posy_t y);
	virtual ~RobotPosEvent();

	virtual void execute();
	virtual posx_t getX() const;
	virtual posy_t getY() const;

private:
	posx_t posX;
	posy_t posY;
};

} /* namespace RobotGame */
#endif /* ROBOTPOSEVENT_H_ */
