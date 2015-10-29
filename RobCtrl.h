/*
 * RobCtrl.h
 *
 *  Created on: Oct 14, 2015
 *      Author: kbp
 */

#ifndef ROBCTRL_H_
#define ROBCTRL_H_

#include <vector>
#include "RobotTypes.h"
#include "Robot.h"
#include <mutex>

namespace RobotGame {

class CannonShell;

class RobCtrl  {
public:
	static void Runner(RobCtrl* obj);

public:
	RobCtrl(unsigned int posX, unsigned int posY)
	: name(""), robot(0), targetSpeed(0), currSpeed(0),
	targetDirection(0), currDirection(0),
	currArmor(MAX_ARMOR), currEnergy(MAX_ENERGY),
	maxSpeed(speedVals[0]), maxRange(rangeVals[0]),
	posX(posX), posY(posY), currShots(0)
	{
		lock();
	};
	virtual ~RobCtrl();

	void setRobot(Robot* robot);
	Robot* getRobot() const { return robot; };

	bool tick(unsigned int tick); /* Returns true if still alive */
	void lock() { mtx.lock();};
	void unlock() { mtx.unlock(); };

public:
	range_t scan(angle_t direction, precision_t precision);
	int cannon (angle_t direction, range_t range);
	void drive (angle_t direction, speed_t speed);
	void skip();
	speed_t getSpeed() const;
	angle_t getDirection() const;
	unsigned int getX() const;
	unsigned int getY() const;
	int getTick() const;
	armor_t getArmor() const;
	energy_t getEnergy() const ;
	void armorToEnergy(armor_t sell);
	void energyToArmor(energy_t sell);

public:
	armor_t damage(armor_t damage);

private:
	void adjustSpeed(speed_t target);
	void adjustDirection(angle_t target);

private:
	std::string name; // Mostly to ease debug
	Robot* robot;
	speed_t targetSpeed;
	speed_t currSpeed;
	angle_t targetDirection;
	angle_t currDirection;
	armor_t currArmor;
	energy_t currEnergy;
	speed_t maxSpeed;
	range_t maxRange;
	posx_t posX;
	posy_t posY;
	unsigned int currShots;

	std::mutex mtx;
};

} /* namespace RobotGame */
#endif /* ROBCTRL_H_ */
