/*
 * RobCtrl.cpp
 *
 *  Created on: Oct 14, 2015
 *      Author: kbp
 */

#include "RobCtrl.h"
#include "Scheduler.h"
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include "Trigonometry.h"
#include <string>
#include "Logger.h"

namespace RobotGame {

void RobCtrl::Runner(RobCtrl* obj)
{
	obj->lock();
	obj->robot->run();
}


RobCtrl::~RobCtrl() {
	// TODO Auto-generated destructor stub
}

void RobCtrl::setRobot(Robot* robot)
{
	int tune_params = MAX_TUNE;
	this->robot = robot;
	tune_t tune = robot->init(name);
	maxSpeed = speedVals[std::min(tune_params, tune.tune_speed)];
	tune_params -= std::min(tune_params, tune.tune_speed);
	maxRange = rangeVals[std::min(tune_params, tune.tune_cannon)];
	tune_params -= std::min(tune_params, tune.tune_cannon);

	Logger::Log(robot, std::string("Max Speed: ") + std::to_string(maxSpeed));
	Logger::Log(robot, std::string("Max Cannon Range: ") + std::to_string(maxRange));
	if (tune_params) {
		Logger::Log(robot, "WARN Unused tune points");
	}
};

range_t RobCtrl::scan(angle_t direction, precision_t precision)
{
	range_t range = 0;
	Logger::Log(robot, std::string("Scanning direction=")+std::to_string(direction)+
						std::string(", precision=")+std::to_string(precision));

	precision = std::min(abs(precision), MAX_PRECISION);

	direction = Trigonometry::compassUnify(direction);

	angle_t minDirection = direction - precision;
	angle_t maxDirection = direction + precision;
	angle_t offset = 0; // To simplify calculations crossing 0/360
	if (minDirection < 0) {
		offset = MAX_PRECISION;
	}
	if (maxDirection > 360) {
		offset = -MAX_PRECISION;
	}
	minDirection += offset;
	maxDirection += offset;

	RobCtrl* robCtrl = Scheduler::iterateRobots(0);
	while( robCtrl != 0) {
		if (robCtrl != this) {
			angle_t dirToRob = Trigonometry::direction(posX, posY, robCtrl->getX(), robCtrl->getY());
			dirToRob += offset; // adjust with same offset
			dirToRob = Trigonometry::compassUnify(dirToRob);

			if (dirToRob >= minDirection && dirToRob <= maxDirection) {
				// Scan success
				range = Trigonometry::distance(posX, posY, robCtrl->getX(), robCtrl->getY());
			}
		}
		robCtrl = Scheduler::iterateRobots(robCtrl);
	}

	Scheduler::end(this);
	return range; // TODO
}


int RobCtrl::cannon (angle_t direction, range_t range)
{
	if (currShots >= MAX_SHOOT || currEnergy < SHOOT_ENERGY_COST) {
		Logger::Log(robot, std::string("Cannon shot wasted"));

		Scheduler::end(this);
		return 0;
	} else {
		currShots++;
		if (range > maxRange)
			range = maxRange;
		Scheduler::addShoot(this, posX, posY, direction, range);
		currEnergy -= SHOOT_ENERGY_COST;
		Logger::Log(robot, std::string("Cannon shot range=")+std::to_string(range));
		Scheduler::end(this);
		return 1;
	}
}
void RobCtrl::drive (angle_t direction, speed_t speed)
{
	targetDirection = direction;
	if (speed > maxSpeed)
		speed = maxSpeed;
	if (speed < MAX_REVERSE_SPEED)
		speed = MAX_REVERSE_SPEED;
	targetSpeed = speed;
	std::string logStr = std::string("Drive: Speed=")+std::to_string(speed) + std::string(" direction=")+std::to_string(direction);
	Logger::Log(robot, logStr);
	Scheduler::end(this);
}

void RobCtrl::skip()
{
	Logger::Log(robot, "Skip");
	Scheduler::end(this);
}

speed_t RobCtrl::getSpeed() const
{
	return currSpeed;
}
angle_t RobCtrl::getDirection() const
{
	return currDirection;
}
unsigned int RobCtrl::getX() const
{
	return posX;
}
unsigned int RobCtrl::getY() const
{
	return posY;
}
int RobCtrl::getTick() const
{
	return 0; // TODO
}
armor_t RobCtrl::getArmor() const
{
	return currArmor;
}
energy_t RobCtrl::getEnergy() const
{
	return currEnergy;
}
void RobCtrl::armorToEnergy(armor_t sell)
{
	if (sell < currArmor) {
		currEnergy += sell * 100;
		currArmor -= sell;
	}
}

void RobCtrl::energyToArmor(energy_t sell)
{
	if (sell < currEnergy) {
		int newArmor = sell / 100;
		currArmor += newArmor;
		currEnergy -= 100*newArmor;
	}
}

void RobCtrl::adjustSpeed(speed_t target)
{
	speed_t delta = target - currSpeed;
	if (delta > 0) {
		// Must accelerate
		currSpeed += std::min(delta, MAX_ACCELERATION);
	}
	if (delta < 0) {
		// Must decelerate
		currSpeed -= std::min(-delta, MAX_ACCELERATION);
	}
}

void RobCtrl::adjustDirection(angle_t target)
{
	angle_t delta = target - currDirection;
	if (delta > 180)
		delta -= 360;
	if (delta < -180)
		delta += 360;
	if (delta > 0)
		currDirection += std::min(MAX_DIR_CHANGE, delta);
	if (delta < 0)
		currDirection -= std::min(MAX_DIR_CHANGE, -delta);
}

bool RobCtrl::tick(unsigned int tick)
{
	if (targetDirection == currDirection) {
		adjustSpeed(targetSpeed);
	} else if (currSpeed == 0) {
		// Instant turn possible
		currDirection = targetDirection;
		adjustSpeed(targetSpeed);
	} else if (currSpeed > MAX_DIR_CHANGE_SPEED) {
		adjustSpeed(MAX_DIR_CHANGE_SPEED);
		if (currSpeed <= MAX_DIR_CHANGE_SPEED) {
			adjustDirection(targetDirection);
		}
	} else if (currSpeed <= MAX_DIR_CHANGE_SPEED) {
		adjustDirection(targetDirection);
		adjustSpeed(std::min(MAX_DIR_CHANGE_SPEED, targetSpeed));
	}

	energy_t driveCost = static_cast<energy_t>(std::abs(currSpeed));
	if (driveCost > currEnergy) {
		// No more gas - stop
		currSpeed = 0;
	} else {
		currEnergy -= driveCost;
	}

	if (currSpeed) {
		bool wallhit = false;
		posx_t prevX = posX;
		posy_t prevY = posY;

		Trigonometry::move(prevX, prevY, currDirection, currSpeed, &posX, &posY);

		if (posX < 0) {
			posX = 0;
			wallhit = true;
		}
		if (posX > MAX_POS_X) {
			posX = MAX_POS_X;
			wallhit = true;
		}
		if (posY < 0) {
			posY = 0;
			wallhit = true;
		}
		if (posY > MAX_POS_Y) {
			posY = MAX_POS_Y;
			wallhit = true;
		}
		if (wallhit) {
			armor_t damage = (currSpeed * 2) / 10;
			currArmor -= std::min(damage, currArmor);
		}
	}
	return (currArmor > 0);
}

armor_t RobCtrl::damage(armor_t damage)
{
	currArmor -= std::min(currArmor, damage);
	return currArmor;
}

} /* namespace RobotGame */
