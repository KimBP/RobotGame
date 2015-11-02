/*
 * RobEvent.h
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#ifndef ROBEVENT_H_
#define ROBEVENT_H_

#include "RobotTypes.h"

namespace RobotGame {

class RobEvent {
public:
	RobEvent() {};
	virtual ~RobEvent() {};

	virtual posx_t getX() const = 0;
	virtual posy_t getY() const = 0;
};

} /* namespace RobotGame */
#endif /* ROBEVENT_H_ */
