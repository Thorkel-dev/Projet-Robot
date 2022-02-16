/**
 * @file robot.c
 *
 * @see robot.h
 *
 * @author Thorkel-dev
 */

#include <stdlib.h>
#include <stdio.h>

#include "robot.h"
#include "../../common.h"

#define ROBOT_CMD_STOP 0

static Robot_s *p_robot = 0;
static SensorState_s sensorState;

extern Robot_s *Robot_new()
{
	// Initialization for the use of the Intox simulator
	if (ProSE_Intox_init("127.0.0.1", 12345) != 0)
	{
		PProseError("Problème d'initialisation du simulateur Intox");
	}

	p_robot = (Robot_s *)malloc(sizeof(Robot_s));

	// Initialization of the motors
	p_robot->mD = Motor_open(RIGHT_MOTOR);
	if (p_robot->mD == NULL)
	{
		PProseError("Problème d'ouverture du moteur droit (port MD)");
	}

	p_robot->mG = Motor_open(LEFT_MOTOR);
	if (p_robot->mG == NULL)
	{
		PProseError("Problème d'ouverture du moteur gauche (port MG)");
	}

	// Initialization of the sensors
	p_robot->sensorFront = ContactSensor_open(FRONT_BUMPER);
	if (p_robot->sensorFront == NULL)
	{
		PProseError("Problème d'ouverture du capteur de contact haut");
	}

	p_robot->sensorFloor = ContactSensor_open(FLOOR_SENSOR);
	if (p_robot->sensorFloor == NULL)
	{
		PProseError("Problème d'ouverture du capteur de contact avant");
	}

	p_robot->light = LightSensor_open(LIGHT_SENSOR);
	if (p_robot->light == NULL)
	{
		PProseError("Problème d'ouverture du capteur de luminosité");
	}

	return p_robot;
}

extern void Robot_free()
{
	// Closing the access to the motors
	Motor_close(p_robot->mD);
	if (p_robot->mD != NULL)
	{
		PProseError("Problème de fermeture du moteur droit");
	}

	Motor_close(p_robot->mG);
	if (p_robot->mG != NULL)
	{
		PProseError("Problème de fermeture du moteur gauche");
	}

	// Closing the accesses to the sensors
	ContactSensor_close(p_robot->sensorFloor);
	if (p_robot->sensorFloor != NULL)
	{
		PProseError("Problème de fermeture du capteur de contact haut");
	}

	ContactSensor_close(p_robot->sensorFront);
	if (p_robot->sensorFront != NULL)
	{
		PProseError("Problème de fermeture du capteur de contact avant");
	}

	LightSensor_close(p_robot->light);
	if (p_robot->light == NULL)
	{
		PProseError("Problème de fermeture du capteur de luminosité");
	}

	free(p_robot);
	ProSE_Intox_close(); // Closing the link with Intox
}

extern void Robot_start()
{
	p_robot = Robot_new();
	Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
}

extern void Robot_stop()
{
	Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
}

extern void Robot_setWheelsVelocity(int vr, int vl)
{
	if (Motor_setCmd(p_robot->mD, vr) != 0)
	{
		PProseError("Problème de commande du moteur droit");
	}
	if (Motor_setCmd(p_robot->mG, vl) != 0)
	{
		PProseError("Problème de commande du moteur gauche");
	}
}

extern int Robot_getRobotSpeed()
{
	return (Motor_getCmd(p_robot->mG) + Motor_getCmd(p_robot->mD)) / 2;
}

extern SensorState_s Robot_getSensorState()
{
	sensorState.luminosity = LightSensor_getStatus(p_robot->light);
	if (ContactSensor_getStatus(p_robot->sensorFloor) == RELEASED && ContactSensor_getStatus(p_robot->sensorFront) == RELEASED)
	{
		sensorState.collision = NO_BUMP;
	}
	else
	{
		sensorState.collision = BUMPED;
	}

	return sensorState;
}