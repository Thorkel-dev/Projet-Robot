/**
 * @file  robot.h
 *
 * @brief  description
 *
 * @author Thorkel-dev
 * @date 28-01-2022
 * @version version 1
 * @section License
 *
 * The MIT License
 *
 * Copyright (c) 2022, Thorkel-dev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef ROBOT_H
#define ROBOT_H

#include "prose.h"

// Motor and sensor pins
#define LEFT_MOTOR MD
#define RIGHT_MOTOR MA
#define LIGHT_SENSOR S1
#define FRONT_BUMPER S3
#define FLOOR_SENSOR S2

/**
 * @brief A robot with two motors and sensors
 */
typedef struct Robot
{
	Motor *mD;					// Le moteur droit
	Motor *mG;					// Le moteur gauche
	ContactSensor *sensorFront; // Capteur avant
	ContactSensor *sensorFloor; // Capteur plafond
	LightSensor *light;			// Capteur de luminosité
} Robot_s;

/**
 * @brief Gives the status of the collision sensors
 */
typedef enum Collision
{
	NO_BUMP = 0, // No contact
	BUMPED		 // Contact
} Collision_e;	 // TODO Add enum to differentiate collision sensors

/**
 * The robot's sensor states
 */
typedef struct
{
	Collision_e collision;
	float luminosity;
} SensorState_s;

/**
 * @brief Initializes the robot and the connection
 *
 *  @return Robot * Pointer to the robot
 */
extern Robot_s *Robot_new();

/**
 *  @brief Destroy the Robot object in memory
 */
extern void Robot_free();

/**
 * @brief Starts the robot (initializes the communication and opens the pins)
 */
extern void Robot_start();

/**
 * @brief Stopping robot (motor speeds at zero)
 */
extern void Robot_stop();

/**
 * @brief Applies the speed of the motors
 *
 * @param vr Right motor speed
 * @param vl Left motor speed
 */
extern void Robot_setWheelsVelocity(int vr, int vl);

/**
 * @brief Returns the robot speed (positive average of the motors speed)
 *
 * @return Robot speed (between 0 and 100)
 */
extern int Robot_getRobotSpeed();

/**
 * @brief Returns the status of the sensors
 *
 * @return SensorState sensor status
 */
extern SensorState_s Robot_getSensorState();

#endif /* ROBOT_H */
