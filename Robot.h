/*
 * Robot.h
 *
 *  Created on: Oct 14, 2015
 *      Author: kbp
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#include <string>
#include "RobotTypes.h"

namespace RobotGame {

class RobCtrl;

class Robot {
private:
	Robot();
	RobCtrl* robCtrl;

public:
	Robot(RobCtrl* robCtrl)
	: robCtrl(robCtrl) {};

	virtual ~Robot();
	virtual struct tune_t init(std::string& name) = 0;
	virtual void run() = 0;
	virtual std::string name() const = 0;

public:
	/* Functions ending tick, i.e. scheduling out robot processing */

	/*
	 * scans in direction with +/- precision. I.e. scan North East with 10degree precision
	 * use scan(45,10).
	 * Returns range to nearest enemy or 0 if none is seen
	 */
	range_t scan(angle_t direction, precision_t precision);

	/*
	 * Fire cannon out of range in specified direction.
	 * I.e. to fire 6000 units in North East use cannon(45,6000)
	 * Return 1 if cannon fired otherwise 0.
	 *
	 * A cannon shot travels 140 units pr tick.
	 * Only 3 shots can be 'in the air' at any time
	 * The shots blast radius is 40 units. Be careful not to hit yourself
	 */
	int cannon (angle_t direction, range_t range);

	/*
	 * Order robot to drive in direction at specified speed.
	 * direction and speed isn't instant. Acceleration is max 20 units/tick/tick
	 * direction change can't only be done if driving less that 50 units/tick
	 * If speed is above 50units/tick and a turn is requested, robot will decrease speed to 50 units,
	 * turn and then speed up again.
	 */
	void drive (angle_t direction, speed_t speed);

	/*
	 * Call this to end tick if none of the other 'enders' are appropriate to use
	 */
	void skip();

public:
	/* Function not scheduling out robot */

	/* Returns actual speed */
	speed_t getSpeed();

	/* Returns actual direction */
	angle_t getDirection();

	/* Returns actual x position */
	unsigned int getX();

	/* Returns actual y position */
	unsigned int getY();

	/* Returns tick number */
	int getTick();

	/* Returns armor left
	 * Armor increase 1 hp/tick
	 */
	armor_t getArmor();

	/* Returns amount of energy
	 * Energy increase 10 point/tick
	 */
	energy_t getEnergy();

	/* Convert armor to energy
	 * 1 hp -> 100 energy points
	 */
	void armorToEnergy(armor_t sell);

	/* Convert energy to armor
	 * 100 energy points -> 1 hp
	 */
	void energyToArmor(energy_t sell);

};


/* A 'factory' function returning the Robot singleton */
extern "C" Robot* getRobot(RobCtrl* robCtrl);

} /* namespace RobotGame */
#endif /* ROBOT_H_ */
