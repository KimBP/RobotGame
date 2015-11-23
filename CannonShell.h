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
public:
	static void FireShell(RobCtrl* owner,
						posx_t posX, posy_t posY,
						angle_t direction, range_t range);

	static void tick(unsigned int tick);

private:
	CannonShell() {};
	CannonShell(RobCtrl* owner, posx_t posX, posy_t posY,
			angle_t direction, range_t range);
public:
	virtual ~CannonShell();

	bool _tick(unsigned int tick); /* Returns true if still in air */
	bool blasted() const {return blast;};
	posx_t getX() const {return currX;};
	posy_t getY() const {return currY;};

private:
	static std::vector<CannonShell> shells;
	static unsigned int nextId;

	RobCtrl* owner;
	posx_t prevX;
	posy_t prevY;
	unsigned int startTick;
	posx_t currX;
	posy_t currY;
	angle_t direction;
	range_t range;
	bool blast;
	unsigned int id;
};

} /* namespace RobotGame */
#endif /* CANNONSHELL_H_ */
