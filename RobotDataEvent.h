/*
 * RobotDataEvent.h
 *
 *  Created on: Nov 28, 2015
 *      Author: kbp
 */

#ifndef ROBOTDATAEVENT_H_
#define ROBOTDATAEVENT_H_

#include "RobEvent.h"
#include <string>

namespace RobotGame {

class RobotDataEvent: public RobotGame::RobEvent {
public:
	RobotDataEvent(int id, std::string name, int armor, int energy)
	: RobEvent(id), name(name),
	  armor(armor), energy(energy)
	{};
	virtual ~RobotDataEvent();
	virtual void execute();
	virtual std::string getName() { return "RobotData"; };

private:
	std::string name;
	int armor;
	int energy;

};

} /* namespace RobotGame */
#endif /* ROBOTDATAEVENT_H_ */
