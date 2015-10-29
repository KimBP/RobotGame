/*
 * Robot.cpp
 *
 *  Created on: Oct 14, 2015
 *      Author: kbp
 */

#include "Robot.h"
#include "RobCtrl.h"

namespace RobotGame {

Robot::~Robot() {
	// TODO Auto-generated destructor stub
}


range_t Robot::scan(angle_t direction, precision_t precision)
{
	return robCtrl->scan(direction, precision);
}

/*
 * Fire cannon out of range in specified direction.
 * I.e. to fire 6000 units in North East use cannon(45,6000)
 * Return 1 if cannon fired otherwise 0.
 *
 * A cannon shot travels 140 units pr tick.
 * Only 3 shots can be 'in the air' at any time
 * The shots blast radius is of 40 units. Be careful not to hit yourself
 */
int Robot::cannon (angle_t direction, range_t range)
{
	return robCtrl->cannon(direction, range);
}

/*
 * Order robot to drive in direction at specified speed.
 * direction and speed isn't instant. Acceleration is max 20 units/tick/tick
 * direction change can't only be done if driving less that 50 units/tick
 * If speed is above 50units/tick and a turn is requested, robot will decrease speed to 50 units,
 * turn and then speed up again.
 */
void Robot::drive (angle_t direction, speed_t speed)
{
	robCtrl->drive(direction, speed);
}

void Robot::skip()
{
	robCtrl->skip();
}

speed_t Robot::getSpeed()
{
	return robCtrl->getSpeed();
}

angle_t Robot::getDirection()
{
	return robCtrl->getDirection();
}

unsigned int Robot::getX()
{
	return robCtrl->getX();
}

unsigned int Robot::getY()
{
	return robCtrl->getY();
}

int Robot::getTick()
{
	return robCtrl->getTick();
}

armor_t Robot::getArmor()
{
	return robCtrl->getArmor();
}

energy_t Robot::getEnergy()
{
	return robCtrl->getEnergy();
}

void Robot::armorToEnergy(armor_t sell)
{
	robCtrl->armorToEnergy(sell);
}

void Robot::energyToArmor(energy_t sell)
{
	robCtrl->energyToArmor(sell);
}

} /* namespace RobotGame */
