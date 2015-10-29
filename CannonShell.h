/*
 * CannonShell.h
 *
 *  Created on: Oct 15, 2015
 *      Author: kbp
 */

#ifndef CANNONSHELL_H_
#define CANNONSHELL_H_

#include "RobotTypes.h"
#include "RobCtrl.h"

namespace RobotGame {

class CannonShell {
private:
	CannonShell() {};
public:
	CannonShell(RobCtrl* shooter,
			posx_t posX, posy_t posY,
			angle_t direction, range_t range);
	virtual ~CannonShell();

	void tick(unsigned int tick);
	bool blast() const; /* Returns true if blasted */
	posx_t getX() const {return currX;};
	posy_t getY() const {return currY;};

private:
	void move(unsigned int x, unsigned int y,
			angle_t direction, range_t distance,
			unsigned int* newX, unsigned int* newY);

private:
	posx_t prevX;
	posy_t prevY;
	unsigned int startTick;
	posx_t currX;
	posy_t currY;
	angle_t direction;
	range_t range;
	RobCtrl* shooter;
	bool _blast;
};

} /* namespace RobotGame */
#endif /* CANNONSHELL_H_ */
