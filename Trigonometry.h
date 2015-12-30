/*
 * Trigonometry.h
 *
 *  Created on: Oct 16, 2015
 *      Author: kbp
 */

#ifndef TRIGONOMETRY_H_
#define TRIGONOMETRY_H_

#include "RobotTypes.h"

namespace RobotGame {

class Trigonometry {
private:
	Trigonometry();
	virtual ~Trigonometry();
public:
	/* Unifies direction to be in the range [0..360[ */
	static angle_t compassUnify(angle_t direction);

	/* Returns distance between two points */
	static range_t distance(int x1, int y1, int x2, int y2);

	/* Returns compass direction from point (x1,y1) to point (x2,y2) */
	static angle_t direction(int x1, int y1, int x2, int y2);

	/* Calculates new position of (x,y) if moved distance units in specified direction */
	static void move(int x, int y,
					angle_t direction, range_t distance,
					int* newX, int* newY);
	/* Calculates new position of (x,y) if moved inspecified direction
	 * with specified speed in number of ticks */
	static void move(int x, int y,
					angle_t direction, speed_t speed, unsigned int ticks,
					int* newX, int* newY);
	/* Converts compass direction to unity (radians) */
	static double compassToUnity(angle_t direction);

	/* Converts unity angle (radians) to compass direction */
	static angle_t unityToCompass(double unityAngle);

	/* Test of functions */
	static void test();
};

} /* namespace RobotGame */
#endif /* TRIGONOMETRY_H_ */
