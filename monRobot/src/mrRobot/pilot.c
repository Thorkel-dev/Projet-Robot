/**
 * @file pilot.c
 *
 * @see pilot.h
 *
 * @author Gautier Edouard
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pilot.h"
#include "robot.h"

/**
 * @brief Les différent état du pilot
 */
typedef enum
{
    IDLE = 0,
    RUNNING
} StateMachine;

/**
 * @brief Vérifie s'il y a un contact sur les capteurs
 *
 * @return TRUE s'il y a un contact, sinon FALSE
 */
static bool_e hasBumped();

/**
 * @brief Convertit le vecteur vitesse en cosigne pour le robot
 *
 * @param vel Vecteur vitesse
 */
static void sendMvt(VelocityVector vel);

/**
 * @brief Machine à état de notre pilot
 *
 * @param vel Vecteur vitesse
 */
static void run(VelocityVector vel);

static PilotState *pilot = 0;
static StateMachine stateMachine;

extern void Pilot_new()
{
    pilot = (PilotState *)malloc(sizeof(PilotState));
}

extern void Pilot_free()
{
    Robot_free();
}

extern void Pilot_start()
{
    Robot_start();
    stateMachine = RUNNING;
}

extern void Pilot_stop()
{
    Robot_stop();
    stateMachine = IDLE;
}

extern void Pilot_setVelocity(VelocityVector vel)
{
    run(vel);
}

extern PilotState Pilot_getState()
{
    pilot->collision = hasBumped();
    pilot->luminosity = Robot_getSensorState().luminosity;
    pilot->speed = Robot_getRobotSpeed();
    Pilot_check();

    return *pilot;
}

extern void Pilot_check()
{
    if (!hasBumped())
    {
        stateMachine = RUNNING;
    }
    else
    {
        printf("stop");
        Pilot_stop();
        VelocityVector vector = {STOP, 0};
        run(vector);
    }
}

static bool_e hasBumped()
{
    const bool_e bumped = (Robot_getSensorState().collision == BUMPED);
    return bumped;
}

static void sendMvt(VelocityVector vel)
{
    switch (vel.dir)
    {
    case FORWARD:
        Robot_setWheelsVelocity(vel.power, vel.power);
        break;
    case BACKWARD:
        Robot_setWheelsVelocity(-vel.power, -vel.power);
        break;
    case LEFT:
        Robot_setWheelsVelocity(vel.power, -vel.power);
        break;
    case RIGHT:
        Robot_setWheelsVelocity(-vel.power, vel.power);
        break;
    default:
        Pilot_stop();
        break;
    }
}

static void run(VelocityVector vector)
{
    switch (stateMachine)
    {
    default:
    case IDLE:
        if (vector.dir != STOP && !pilot->collision)
        {
            Pilot_start();
            sendMvt(vector);
            stateMachine = RUNNING;
        }
        else
        {
            Pilot_stop();
        }
        break;

    case RUNNING:
        if (pilot->collision == TRUE)
        {
            vector.power = 0;
        }

        if (vector.power == 0)
        {
            stateMachine = IDLE;
        }
        sendMvt(vector);
        break;
    }
}
