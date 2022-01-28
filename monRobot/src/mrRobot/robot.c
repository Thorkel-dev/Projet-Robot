/**
 * @file robot.c
 *
 * @see robot.h
 *
 * @author Gautier Edouard
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "robot.h"

#define ROBOT_CMD_STOP 0

static Robot *robot = 0;
static SensorState sensorState;

extern Robot *Robot_new()
{
	/* Initialisation pour l'utilisation du simulateur Intox. */
	if (ProSE_Intox_init("127.0.0.1", 12345) != 0)
	{
		PProseError("Problème d'initialisation du simulateur Intox");
	}

	robot = (Robot *)malloc(sizeof(Robot));

	/* Initialisation des moteurs. */
	robot->mD = Motor_open(LEFT_MOTOR);
	if (robot->mD == NULL)
	{
		PProseError("Problème d'ouverture du moteur droit (port MD)");
	}

	robot->mG = Motor_open(RIGHT_MOTOR);
	if (robot->mG == NULL)
	{
		PProseError("Problème d'ouverture du moteur gauche (port MA)");
	}

	/* Initialisation des capteurs. */
	robot->sensorFront = ContactSensor_open(FRONT_BUMPER);
	if (robot->sensorFront == NULL)
	{
		PProseError("Problème d'ouverture du capteur de contact avant");
	}

	robot->sensorFloor = ContactSensor_open(FLOOR_SENSOR);
	if (robot->sensorFloor == NULL)
	{
		PProseError("Problème d'ouverture du capteur de contact haut");
	}

	robot->light = LightSensor_open(LIGHT_SENSOR);
	if (robot->light == NULL)
	{
		PProseError("Problème d'ouverture du capteur de luminosité");
	}

	return robot;
}

extern void Robot_free()
{
	/* Fermeture des accès aux moteurs. */
	Motor_close(robot->mD);
	if (robot->mD != NULL)
	{
		PProseError("Problème de fermeture du moteur droit");
	}

	Motor_close(robot->mG);
	if (robot->mG != NULL)
	{
		PProseError("Problème de fermeture du moteur gauche");
	}

	/* Fermeture des accès aux capteurs. */
	ContactSensor_close(robot->sensorFloor);
	if (robot->sensorFloor != NULL)
	{
		PProseError("Problème de fermeture du capteur de contact haut");
	}

	ContactSensor_close(robot->sensorFront);
	if (robot->sensorFront != NULL)
	{
		PProseError("Problème de fermeture du capteur de contact avant");
	}

	LightSensor_close(robot->light);
	if (robot->light == NULL)
	{
		PProseError("Problème de fermeture du capteur de luminosité");
	}

	free(robot);
	ProSE_Intox_close(); // fermeture du lien avec Intox
}

extern void Robot_start()
{
	robot = Robot_new();
	Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
}

extern void Robot_stop()
{
	Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
}

extern void Robot_setWheelsVelocity(int vr, int vl)
{
	if (Motor_setCmd(robot->mD, vr) != 0)
	{
		PProseError("Problème de commande du moteur droit");
	}
	if (Motor_setCmd(robot->mG, vl) != 0)
	{
		PProseError("Problème de commande du moteur gauche");
	}
}

extern int Robot_getRobotSpeed()
{
	return abs(Motor_getCmd(robot->mG) + Motor_getCmd(robot->mD)) / 2;
}

extern SensorState Robot_getSensorState()
{
	sensorState.luminosity = LightSensor_getStatus(robot->light);
	if (ContactSensor_getStatus(robot->sensorFloor) == RELEASED && ContactSensor_getStatus(robot->sensorFront) == RELEASED)
	{
		sensorState.collision = NO_BUMP;
	}
	else
	{
		sensorState.collision = BUMPED;
	}

	return sensorState;
}