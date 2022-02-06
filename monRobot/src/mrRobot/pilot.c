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

typedef enum State
{
    S_NONE = 0,
    S_RUNNING,
    S_IDLE,
    S_NB_STATE = 3
} State_e;

typedef enum Event
{
    E_CHANGE_MVT = 0,
    E_ASK_LOG,
    E_NB_EVENT = 2
} Event_e;

typedef enum Action
{
    A_NONE = 0,
    A_SET_MVT,
    A_SHOW_LOG,
    A_NB_ACTION = 3
} Action_e;

typedef struct Transition
{
    State_e stateNext;
    Action_e action;
} Transition_s;

static Transition_s a_S_stateMachine[S_NB_STATE][E_NB_EVENT] =
    {
        [S_IDLE][E_ASK_LOG] = {S_IDLE, A_SHOW_LOG},
        [S_RUNNING][E_ASK_LOG] = {S_RUNNING, A_SHOW_LOG},

        [S_IDLE][E_CHANGE_MVT] = {S_RUNNING, A_SET_MVT},
        [S_RUNNING][E_CHANGE_MVT] = {S_RUNNING, A_SET_MVT},
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
static void sendMvt(VelocityVector_s vel);

/**
 * @brief Machine à état de notre pilot
 *
 * @param vel Vecteur vitesse
 */
static void run(Event_e event, VelocityVector_s vel);

static PilotState_s *p_S_pilot = 0;
static State_e S_currentState;

extern void Pilot_new()
{
    p_S_pilot = (PilotState_s *)malloc(sizeof(PilotState_s));
}

extern void Pilot_free()
{
    Robot_free();
}

extern void Pilot_start()
{
    Robot_start();
    S_currentState = S_IDLE;
}

extern void Pilot_stop()
{
    Robot_stop();
    S_currentState = S_IDLE;
}

extern void Pilot_setVelocity(VelocityVector_s vel)
{
    run(E_CHANGE_MVT, vel);
}

extern PilotState_s Pilot_getState()
{
    p_S_pilot->collision = hasBumped();
    p_S_pilot->luminosity = Robot_getSensorState().luminosity;
    p_S_pilot->speed = Robot_getRobotSpeed();
    VelocityVector_s vector = {STOP, 0};
    run(E_ASK_LOG, vector);

    return *p_S_pilot;
}

extern void Pilot_check()
{
    if (!hasBumped())
    {
        S_currentState = S_RUNNING;
    }
    else
    {
        VelocityVector_s vector = {STOP, 0};
        S_currentState = S_IDLE;
        run(E_CHANGE_MVT, vector);
    }
}

static bool_e hasBumped()
{
    const bool_e bumped = (Robot_getSensorState().collision == BUMPED);
    return bumped;
}

static void sendMvt(VelocityVector_s vel)
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
        S_currentState = S_IDLE;
        break;
    }
}

static void run(Event_e event, VelocityVector_s vector)
{
    const Action_e action = a_S_stateMachine[S_currentState][event].action;
    const State_e stateNext = a_S_stateMachine[S_currentState][event].stateNext;

    if (stateNext != S_NONE)
    {
        S_currentState = stateNext;
        if (action == A_SET_MVT)
        {
            sendMvt(vector);
        }
        else if (action == A_SHOW_LOG)
        {
            Pilot_check();
        }
    }
    if (stateNext == S_IDLE)
    {
        Pilot_stop();
    }
}