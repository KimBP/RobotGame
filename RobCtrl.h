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
	RobCtrl(unsigned int posX, unsigned int posY);
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
	speed_t getSpeed() const { return currSpeed; };
	angle_t getDirection() const {return currDirection; };
	unsigned int getX() const {return posX;};
	unsigned int getY() const { return posY; };
	int getTick() const;
	armor_t getArmor() const { return currArmor; };
	energy_t getEnergy() const { return currEnergy; };
	void armorToEnergy(armor_t sell);
	void energyToArmor(energy_t sell);
	void shotBlasted(RobCtrl* owner, posx_t x, posy_t y);
	std::string getName() const { return name; };
	void doDie() { goDie = true; };
	void checkGoDie() { if (goDie) {exit(0); } };

public:
	armor_t damage(armor_t damage);

private:
	void adjustSpeed(speed_t target);
	void adjustDirection(angle_t target);
	bool addShoot(posx_t x, posy_t y, angle_t direction, range_t range);

private:
	std::string name;
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
	unsigned int activeShells;

	bool goDie;
	std::mutex mtx;
};

} /* namespace RobotGame */
#endif /* ROBCTRL_H_ */
