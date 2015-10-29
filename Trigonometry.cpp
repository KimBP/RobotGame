/*
 * Trigonometry.cpp
 *
 *  Created on: Oct 16, 2015
 *      Author: kbp
 */

#include "Trigonometry.h"
#include <algorithm>
#include <math.h>
//#include <stdlib.h>

namespace RobotGame {

Trigonometry::Trigonometry() {
	// TODO Auto-generated constructor stub

}

Trigonometry::~Trigonometry() {
	// TODO Auto-generated destructor stub
}

angle_t Trigonometry::compassUnify(angle_t direction)
{
	while (direction < 0) {
		direction += 360;
	}
	return direction % 360;
}

static const double  PI = 3.14159265;

double Trigonometry::compassToUnity(angle_t direction)
{
	direction = compassUnify(direction);
	double unityAngle = compassUnify(-direction + 90); // Convert from compass to unity circle angles
	unityAngle *= PI / 180.0;	// Convert to radians

	return unityAngle;
}

angle_t Trigonometry::unityToCompass(double unityAngle)
{
	double direction = unityAngle * 180.0 / PI;
	direction = -direction + 90;
	return compassUnify(direction);
}


angle_t Trigonometry::direction(int x1, int y1, int x2, int y2)
{

	double dx = x2-x1;
	double dy = y1-y2; // Y-axis reverted compared to unity circle

	if (dx == 0) {
		if (dy > 0)
			return 0;
		else
			return 180;
	} else if (dy == 0) {
		if (dx > 0)
			return 90;
		else
			return 270;
	} else {

		double unityAngle = atan(dy / dx);

		return unityToCompass(unityAngle);
	}
}

range_t Trigonometry::distance(int x1, int y1, int x2, int y2)
{

	double dx = x2-x1;
	double dy = y2-y1;

	double dist = sqrt(dx*dx + dy*dy);

	return dist;
}


void Trigonometry::move(int x, int y,
		angle_t direction, range_t distance,
		int* newX, int* newY)
{
	direction = compassUnify(direction);

	double unityAngle = compassToUnity(direction);
	double deltaX = cos(unityAngle) * distance;
	double deltaY = sin(unityAngle) * distance;
	*newX = x + deltaX;
	*newY = y - deltaY; // Y - axis reverted compared to unity circle
}

void Trigonometry::move(int x, int y,
				angle_t direction, speed_t speed, unsigned int ticks,
				int* newX, int* newY)
{
	range_t distance = static_cast<range_t>(speed*ticks);
	if (speed < 0) {
		direction += 180;
		direction = compassUnify(direction);
	}
	move(x,y, direction, abs(distance), newX, newY);
}



} /* namespace RobotGame */
