/*
 * RobotPosEvent.h
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#ifndef CANNONPOSEVENT_H_
#define CANNONPOSEVENT_H_

#include "RobEvent.h"

namespace RobotGame {

class CannonPosEvent: public RobotGame::RobEvent {
public:
	CannonPosEvent(int id, posx_t x1, posy_t y1, posx_t x2, posy_t y2, bool blasted);
	virtual ~CannonPosEvent();

	virtual void execute();

private:
	posx_t posX1;
	posy_t posY1;
	posx_t posX2;
	posy_t posY2;
	bool blasted;
};

} /* namespace RobotGame */
#endif /* CANNONPOSEVENT_H_ */
