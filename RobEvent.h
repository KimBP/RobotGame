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
	RobEvent(int id)
	: id(id) {};
	virtual ~RobEvent() {};

	virtual void execute() = 0;

	virtual posx_t getX() const = 0;
	virtual posy_t getY() const = 0;

	int getId() {return id; };

private:
	int id;

};

} /* namespace RobotGame */
#endif /* ROBEVENT_H_ */
