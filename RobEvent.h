/*
 * RobEvent.h
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#pragma once

#include "RobotTypes.h"
#include <string>

namespace RobotGame {

class RobEvent {
public:
	RobEvent(int id)
	: id(id) {};
	virtual ~RobEvent() = default;

	virtual void execute() = 0;
	virtual std::string getName() = 0;
	int getId() {return id; };

private:
	int id;

};

} /* namespace RobotGame */
