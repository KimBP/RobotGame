/*
 * Trigonometry.h
 *
 *  Created on: Oct 16, 2015
 *      Author: kbp
 */

#pragma once

#include "RobotTypes.h"

namespace RobotGame {

namespace Trigonometry {
	/* Unifies direction to be in the range [0..360[ */
	angle_t compassUnify(angle_t direction);

	/* Returns distance between two points */
	range_t distance(int x1, int y1, int x2, int y2);

	/* Returns compass direction from point (x1,y1) to point (x2,y2) */
	angle_t direction(int x1, int y1, int x2, int y2);

	/* Calculates new position of (x,y) if moved distance units in specified direction */
	void move(int x, int y,
				angle_t direction, range_t distance,
				int* newX, int* newY);
	/* Calculates new position of (x,y) if moved inspecified direction
	 * with specified speed in number of ticks */
	void move(int x, int y,
				angle_t direction, speed_t speed, unsigned int ticks,
				int* newX, int* newY);
	/* Converts compass direction to unity (radians) */
	double compassToUnity(angle_t direction);

	/* Converts unity angle (radians) to compass direction */
	angle_t unityToCompass(double unityAngle);

	/* Test of functions */
	void test();
} /* namespace Trigonometry */

} /* namespace RobotGame */
