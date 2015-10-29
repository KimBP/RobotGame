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
	CannonShell(posx_t posX, posy_t posY,
			angle_t direction, range_t range);
	virtual ~CannonShell();

	bool tick(unsigned int tick); /* Returns true if still in air */
	bool blasted() const {return blast;};
	posx_t getX() const {return currX;};
	posy_t getY() const {return currY;};

private:
	posx_t prevX;
	posy_t prevY;
	unsigned int startTick;
	posx_t currX;
	posy_t currY;
	angle_t direction;
	range_t range;
	bool blast;
};

} /* namespace RobotGame */
#endif /* CANNONSHELL_H_ */
