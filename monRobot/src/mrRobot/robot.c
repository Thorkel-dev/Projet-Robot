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

static Robot_s *p_S_robot = 0;
static SensorState_s S_sensorState;

extern Robot_s *Robot_new()
{
	/* Initialisation pour l'utilisation du simulateur Intox. */
	if (ProSE_Intox_init("127.0.0.1", 12345) != 0)
	{
		PProseError("Problème d'initialisation du simulateur Intox");
	}

	p_S_robot = (Robot_s *)malloc(sizeof(Robot_s));

	/* Initialisation des moteurs. */
	p_S_robot->mD = Motor_open(LEFT_MOTOR);
	if (p_S_robot->mD == NULL)
	{
		PProseError("Problème d'ouverture du moteur droit (port MD)");
	}

	p_S_robot->mG = Motor_open(RIGHT_MOTOR);
	if (p_S_robot->mG == NULL)
	{
		PProseError("Problème d'ouverture du moteur gauche (port MA)");
	}

	/* Initialisation des capteurs. */
	p_S_robot->sensorFront = ContactSensor_open(FRONT_BUMPER);
	if (p_S_robot->sensorFront == NULL)
	{
		PProseError("Problème d'ouverture du capteur de contact avant");
	}

	p_S_robot->sensorFloor = ContactSensor_open(FLOOR_SENSOR);
	if (p_S_robot->sensorFloor == NULL)
	{
		PProseError("Problème d'ouverture du capteur de contact haut");
	}

	p_S_robot->light = LightSensor_open(LIGHT_SENSOR);
	if (p_S_robot->light == NULL)
	{
		PProseError("Problème d'ouverture du capteur de luminosité");
	}

	return p_S_robot;
}

extern void Robot_free()
{
	/* Fermeture des accès aux moteurs. */
	Motor_close(p_S_robot->mD);
	if (p_S_robot->mD != NULL)
	{
		PProseError("Problème de fermeture du moteur droit");
	}

	Motor_close(p_S_robot->mG);
	if (p_S_robot->mG != NULL)
	{
		PProseError("Problème de fermeture du moteur gauche");
	}

	/* Fermeture des accès aux capteurs. */
	ContactSensor_close(p_S_robot->sensorFloor);
	if (p_S_robot->sensorFloor != NULL)
	{
		PProseError("Problème de fermeture du capteur de contact haut");
	}

	ContactSensor_close(p_S_robot->sensorFront);
	if (p_S_robot->sensorFront != NULL)
	{
		PProseError("Problème de fermeture du capteur de contact avant");
	}

	LightSensor_close(p_S_robot->light);
	if (p_S_robot->light == NULL)
	{
		PProseError("Problème de fermeture du capteur de luminosité");
	}

	free(p_S_robot);
	ProSE_Intox_close(); // fermeture du lien avec Intox
}

extern void Robot_start()
{
	p_S_robot = Robot_new();
	Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
}

extern void Robot_stop()
{
	Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
}

extern void Robot_setWheelsVelocity(int vr, int vl)
{
	if (Motor_setCmd(p_S_robot->mD, vr) != 0)
	{
		PProseError("Problème de commande du moteur droit");
	}
	if (Motor_setCmd(p_S_robot->mG, vl) != 0)
	{
		PProseError("Problème de commande du moteur gauche");
	}
}

extern int Robot_getRobotSpeed()
{
	return abs(Motor_getCmd(p_S_robot->mG) + Motor_getCmd(p_S_robot->mD)) / 2;
}

extern SensorState_s Robot_getSensorState()
{
	S_sensorState.luminosity = LightSensor_getStatus(p_S_robot->light);
	if (ContactSensor_getStatus(p_S_robot->sensorFloor) == RELEASED && ContactSensor_getStatus(p_S_robot->sensorFront) == RELEASED)
	{
		S_sensorState.collision = NO_BUMP;
	}
	else
	{
		S_sensorState.collision = BUMPED;
	}

	return S_sensorState;
}