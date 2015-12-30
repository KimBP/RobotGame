/*
 * Trigonometry.cpp
 *
 *  Created on: Oct 16, 2015
 *      Author: kbp
 */

#include "Trigonometry.h"
#include <algorithm>
#include <math.h>
#include <cmath>
#include "Logger.h"
#include <string>

//#include <stdlib.h>

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))

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
	double direction = round(unityAngle * 180.0 / PI);
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

		if (dx < 0) {
			unityAngle += PI;
		}
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

void Trigonometry::test()
{
	/* Test of compass unify */
	const struct {
		angle_t input;
		angle_t output;
	} compassUnifyMetrics[] = {
			{ -180, 180},
			{ 0, 0 },
			{ 180, 180},
			{ 360, 0},
			{ 540, 180}
	};

	for (unsigned int i=0; i < SIZEOF_ARRAY(compassUnifyMetrics);i++) {
		if (compassUnifyMetrics[i].output != compassUnify(compassUnifyMetrics[i].output)) {
			Logger::Log("CompassUnify", std::string("Error converting ") + std::to_string(compassUnifyMetrics[i].input));
			exit (-1);
		}
	};

	/* Test of unity / compass conversions */
	const struct {
		angle_t input;
		angle_t output;
	} unityCompassMetrics[] = {
			{-180, 180},
			{-90, 270},
			{-75, 285},
			{-60, 300},
			{-45, 315},
			{-30, 330},
			{0, 0},
			{ 15, 15},
			{30, 30},
			{45, 45},
			{60, 60},
			{75, 75},
			{90, 90},
			{115, 115},
			{145, 145},
			{160, 160},
			{180, 180},
			{269, 269},
			{270, 270},
			{359, 359},
			{360, 0},
			{361, 1}
	};

	for (unsigned int i=0; i < SIZEOF_ARRAY(unityCompassMetrics);i++) {
		if (unityCompassMetrics[i].output != unityToCompass(compassToUnity(unityCompassMetrics[i].input))) {
			Logger::Log("Unity/Compass", std::string("Error converting ") + std::to_string(unityCompassMetrics[i].input));
			exit (-1);

		}
	}

	typedef struct {
		int x;
		int y;
	} point_t;

	/* Test of distance */
	const struct {
		point_t input;
		range_t output;
	} distanceMetrics[] = {
			{ {0, 100}, 100 },
			{ {100, 100}, 141},
			{ {100, 0}, 100 },
			{ {100, -100}, 141},
			{ {0, -100}, 100},
			{ {-100, -100}, 141},
			{ {-100, 0}, 100},
			{ {-100, 100}, 141}
	};

	for (unsigned int i=0; i < SIZEOF_ARRAY(distanceMetrics); i++) {
		int x2 = distanceMetrics[i].input.x;
		int y2 = distanceMetrics[i].input.y;
		if (distanceMetrics[i].output != distance(0,0,x2,y2)) {
			Logger::Log("Distance", std::string("Error converting distanceMetrics ") + std::to_string(i));
			exit (-1);
		}
	}

	/* Test of direction */
	const struct {
		point_t input;
		angle_t output;
	} directionMetrics[] = {
			{ {0, 100}, 180 },
			{ {100, 100}, 135},
			{ {100, 0}, 90 },
			{ {100, -100}, 45},
			{ {0, -100}, 0},
			{ {-100, -100}, 315},
			{ {-100, 0}, 270},
			{ {-100, 100}, 225}
	};
	for (unsigned int i=0; i < SIZEOF_ARRAY(directionMetrics); i++) {
		int x2 = directionMetrics[i].input.x;
		int y2 = directionMetrics[i].input.y;
		if (directionMetrics[i].output != direction(0,0,x2,y2)) {
			Logger::Log("Direction", std::string("Error converting distanceMetrics ") + std::to_string(i));
			exit (-1);
		}
	}


}

} /* namespace RobotGame */
