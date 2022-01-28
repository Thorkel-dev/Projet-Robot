/**
 * @file  robot.h
 *
 * @brief  description
 *
 * @author Edouard Gautier
 * @date 28-01-2022
 * @version version 1
 * @section License
 *
 * The MIT License
 *
 * Copyright (c) 2016, Gautier Edouard
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

// Pins des moteurs et capteurs
#define LEFT_MOTOR MD
#define RIGHT_MOTOR MA
#define LIGHT_SENSOR S1
#define FRONT_BUMPER S3
#define FLOOR_SENSOR S2

/**
 * @brief Robot
 *
 * Un robot muni de deux moteurs et de capteurs
 */
typedef struct
{
	Motor *mD;					// Le moteur droit
	Motor *mG;					// Le moteur gauche
	ContactSensor *sensorFront; // Capteur avant
	ContactSensor *sensorFloor; // Capteur plafond
	LightSensor *light;			// Capteur de luminosité
} Robot;

typedef enum
{
	NO_BUMP = 0, // Pas de contact
	BUMPED		 // Contact
} Collision;

/**
 * Les états des capteurs du robot
 */
typedef struct
{
	Collision collision;
	float luminosity;
} SensorState;

/**
 * @brief Initialise le robot ainsi que la connexion
 *
 *  @return Robot * Pointeur vers le robot
 */
extern Robot *Robot_new();

/**
 *  @brief Detruit l'objet Robot en mémoire
 */
extern void Robot_free();

/**
 * @brief Démarre le Robot (initialise la communication et ouvre les pins)
 */
extern void Robot_start();

/**
 * @brief Arrêt de mon robot (vitesses moteurs à zéro).
 */
extern void Robot_stop();

/**
 * @brief Applique la vitesse des moteurs
 *
 * @param vr vitesse du moteur droit
 * @param vl vitesse du moteur gauche
 */
extern void Robot_setWheelsVelocity(int vr, int vl);

/**
 * @brief Retourne la vitesse du robot (moyenne positive de la vitesse des moteurs)
 *
 * @return Vitesse du robot (entre 0 et 100)
 */
extern int Robot_getRobotSpeed();

/**
 * @brief Retourne l'état des capteurs
 *
 * @return SensorState état des capteurs
 */
extern SensorState Robot_getSensorState();

#endif /* ROBOT_H */
