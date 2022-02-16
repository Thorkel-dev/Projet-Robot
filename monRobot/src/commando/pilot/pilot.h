/**
 * @file  pilot.h
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

#ifndef PILOT_H
#define PILOT_H

#include "../../common.h"

typedef struct
{
    Direction_e dir;
    int power;
} VelocityVector_s;

typedef struct
{
    void (*start)();
    void (*stop)();
    void (*setVelocity)(VelocityVector_s);
    PilotState_s (*getState)();
    void (*check)();
    void (*free)();
    void (*new)();
} PilotControle_s;

/**
 * @brief Initialize the pilot in memory
 */
extern void Pilot_new();

/**
 * @brief Destruct the Pilot object in memory
 */
extern void Pilot_free();

/**
 * @brief Start pilot (engine speeds at zero)
 */
extern void Pilot_start();

/**
 * @brief Stop pilot and robot (motor speeds at zero)
 *
 * @param vector Speed vector (Not taken into consideration)
 */
extern void Pilot_stop(VelocityVector_s vector);

/**
 * @brief Applies speed and directional instructions
 *
 * @param vector Speed vector
 */
extern void Pilot_setVelocity(VelocityVector_s vector);

/**
 * @brief Gives the general state of the pilot
 *
 * @return PilotState_s the state of the pilot
 */
extern PilotState_s Pilot_getState();

/**
 * @brief Check if the pilot can move, if not stop it
 *
 * @param vector Speed vector (Not taken into consideration)
 */
extern void Pilot_check(VelocityVector_s vector);

#endif /* PILOT_H */
