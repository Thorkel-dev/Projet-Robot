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

enum State_e
{
    S_NONE = 0,
    S_VERS_RUNNING,
    S_RUNNING,
    S_IDLE,
    S_NB_STATE
};

typedef enum
{
    E_CHANGE_MVT = 0,
    E_ASK_LOG,
    E_NB_EVENT
} Event;

enum Action_e
{
    A_NONE = 0,
    A_SET_MVT,
    A_SHOW_LOG,
    A_NB_ACTION
};

struct transition_t
{
    enum State_e stateNext;
    enum Action_e action;
};

static struct transition_t s_stateMachine[S_NB_STATE][E_NB_EVENT] =
    {
        [S_IDLE][E_ASK_LOG] = {S_IDLE, A_SHOW_LOG},
        [S_RUNNING][E_ASK_LOG] = {S_RUNNING, A_SHOW_LOG},

        [S_IDLE][E_CHANGE_MVT] = {S_VERS_RUNNING, A_SET_MVT},
        [S_RUNNING][E_CHANGE_MVT] = {S_VERS_RUNNING, A_SET_MVT},

        [S_VERS_RUNNING][E_CHANGE_MVT] = {S_RUNNING, A_SET_MVT},
        [S_VERS_RUNNING][E_CHANGE_MVT] = {S_IDLE, A_SET_MVT},
};

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
static State_e stateMachine;

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

extern void Pilot_check() // TODO
{
    if (!hasBumped())
    {
        stateMachine = RUNNING;
    }
    else
    {
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

static void run(Event event, VelocityVector vector)
{
    const enum Action_e action = s_stateMachine[s_currentState][event].action;
    const enum State_e stateNext = s_stateMachine[s_currentState][event].stateNext;

    switch (action)
    {
    case A_SET_MVT:
        Pilot_setVelocity(vector);
        break;

    case A_SHOW_LOG:
        PilotState l_state = Pilot_getState();
        break;
    default:
        break;
    }

    if (stateNext != S_NONE)
    {
        s_currentState = l_stateNext;
    }
}
