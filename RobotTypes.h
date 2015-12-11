/*
 * RobotTypes.h
 *
 *  Created on: Oct 14, 2015
 *      Author: kbp
 */

#ifndef ROBOTTYPES_H_
#define ROBOTTYPES_H_

namespace RobotGame {

const unsigned int MAX_SHOOT = 3;       	/* Maximum active shells in air at once */
const int MAX_ACCELERATION = 20;	/* Units/tick/tick */

typedef int angle_t;     		/* 0..359 (modulo 360) - input will be adjusted */
typedef int precision_t; 		/* +/- 1..15 - out of range will be truncated to +/- 15 */
typedef unsigned int range_t;
typedef int speed_t;			/* -75..100 (units/tick) */


typedef int armor_t;			/* 0..MAX_ARMOR - hp ,0 and you are dead */
typedef int energy_t;			/* 0..MAX_ENERGY, 0 no more energy */
typedef int posx_t;				/* 0..MAX_POS_X - horizontal position */
typedef int posy_t; 			/* 0..MAX_POS_Y - vertical position */

const posx_t MAX_POS_Y = 9999;
const posy_t MAX_POS_X = 9999;

const energy_t ENERGY_FILLUP = 50;  /* Energy added for free each tick */
const angle_t MAX_PRECISION = 15;

const armor_t MAX_ARMOR = 100;     	/* Armor when not hurt at all */
const energy_t MAX_ENERGY = 10000;  	/* Energy when full loaded */
const energy_t SHOOT_ENERGY_COST = 200; /* Energy required to fire cannon */

const speed_t MAX_SPEED = 100;			 /* Max speed */
const speed_t MAX_DIR_CHANGE_SPEED = 50; /* Max speed at which direction change can be made */
const angle_t MAX_DIR_CHANGE = 45;       /* Max direction change pr tick */
const speed_t MAX_REVERSE_SPEED = -50;

const speed_t SHELL_SPEED = 700; /* units/tick */

const speed_t speedVals[] = {50, 75, 100};
const range_t rangeVals[] = {5000, 6000, 7000};

const int MAX_TUNE = 2;
/* Sum of tune parameters must be <= 2 */
struct tune_t {
	int tune_speed; /* 0..2 => max speed for this robot - see speedVals */
	int tune_cannon; /* 0..2 => max range for this robot - see rangeVals */
};

const unsigned int DAMAGE_RANGE_CNT = 4;
const range_t damageRanges[DAMAGE_RANGE_CNT] = {100, 200, 300, 400};
const armor_t damageVals[DAMAGE_RANGE_CNT] = {8, 4, 2, 1};



/* Directions:
 * 		Directions follow compas. I.e. north=0, east=90, south=180, west=270
 *
 */
/* Positions:
 * 		Nort-West: 	x=0,y=0,
 * 		North-East: x=9999,y=0,
 * 		South-East: x=9999,y=9999,
 * 		South-West: x=0,y=9999
 */
/* Energy:
 * 		Firing the cannon costs 200 energy points
 * 		Price of driving is analog to speed. I.e. driving 50 units/tick costs 50 energy points/tick
 *      Each tick a robot is assigned 50 energy points (coming from solar panels)
 */
/* Armor:
 * 		Being too close a blast from a cannon (including own) cost armor according to this scheme:
 * 		0..10 units from center :  8 hp
 * 		11..20 units from center : 4 hp
 * 		21..30 units from center : 2 hp
 * 		31..40 units from center : 1 hp
 *
 * 		Damage caused by hitting the wall depend on speed.
 * 		(2 x speed)/10 = hp loss. I.e. driving 50 units/tick -> loss of 10hp
 *
 * 		Damage caused by hitting another robot causes loss of hp at both according to this formula:
 * 		2 x (speed rob1 + speed rob2)/10 = hp.
 * 		I.e rob1 driving 20 units/tick, rob2 driving 50 units/tick => loss of 2x(20+50)/10=14hp for each
 */

} /* namespace RobotGame */
#endif /* ROBOTTYPES_H_ */
