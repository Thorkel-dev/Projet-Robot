/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/**
 * @file  robot.h
 *
 * @brief  description
 *
 * @author Gautier Edouard
 * @date 21-01-2022
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

#ifndef PILOT_H
#define PILOT_H
#include "prose.h"

typedef enum
{
    LEFT = 0,
    RIGHT,
    FORWARD,
    BACKWARD,
    STOP
} Direction;

typedef struct
{
    Direction dir;
    int power;
} VelocityVector;

/**
 * @brief Structure de l'état du robot
 */
typedef struct
{
    int speed;
    bool_e collision;
    int luminosity;
} PilotState;

typedef struct
{
    void (*start)();
    void (*stop)();
    void (*setVelocity)(VelocityVector);
    PilotState (*getState)();
    void (*check)();
    void (*free)();
    void (*new)();
} PilotControle;

/**
 * @brief Démarrer mon pilot (vitesses moteurs à zéro).
 */
extern void Pilot_start();

/**
 * @brief Arrête mon pilot et mon robot (vitesses moteurs à zéro).
 */
extern void Pilot_stop();

/**
 * @brief Initialise le pilot en mémoire
 */
extern void Pilot_new();

/**
 * destruct l'object Pilot en mémoire
 */
extern void Pilot_free();

/**
 * @brief Applique les consignes de vitesse et de direction
 *
 * @param vel Vecteur vitesse
 */
extern void Pilot_setVelocity(VelocityVector vel);

/**
 * @brief Donne l'état général du pilot
 *
 * @return PilotState l'état du pilot
 */
extern PilotState Pilot_getState();

/**
 * @brief Vérifie si le pilot peut bouger, sinon l'arrête
 */
extern void Pilot_check();

#endif /* PILOT_H */