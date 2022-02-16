/**
 * @file pilot.c
 *
 * @see pilot.h
 *
 * @author Thorkel-dev
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pilot.h"
#include "../robot/robot.h"

typedef enum State
{
    S_NONE = 0,
    S_RUNNING,
    S_IDLE,
    S_VERS_RUNNING,
    S_NB_STATE
} State_e;

typedef enum Event
{
    E_CHANGE_MVT = 0,
    E_ASK_LOG,
    E_BUMPED,
    E_STOP,
    E_NB_EVENT
} Event_e;

typedef enum Action
{
    A_SHOW_LOG = 0,
    A_CHECK_VECTOR,
    A_STOP,
    A_SET_MVT,
    A_NB_ACTION
} Action_e;

typedef struct Transition
{
    State_e stateNext;
    Action_e action;
} Transition_s;

static Transition_s a_stateMachine[S_NB_STATE][E_NB_EVENT] =
    {
        [S_IDLE][E_ASK_LOG] = {S_IDLE, A_SHOW_LOG},
        [S_RUNNING][E_ASK_LOG] = {S_RUNNING, A_SHOW_LOG},

        [S_IDLE][E_CHANGE_MVT] = {S_VERS_RUNNING, A_CHECK_VECTOR},
        [S_RUNNING][E_CHANGE_MVT] = {S_VERS_RUNNING, A_CHECK_VECTOR},

        [S_RUNNING][E_BUMPED] = {S_IDLE, A_STOP},
        [S_IDLE][E_BUMPED] = {S_IDLE, A_STOP},

        [S_VERS_RUNNING][E_CHANGE_MVT] = {S_RUNNING, A_SET_MVT},
        [S_VERS_RUNNING][E_STOP] = {S_IDLE, A_STOP},
};

/**
 * @brief Checks if there is a contact on the sensors
 *
 * @return TRUE if there is a contact, otherwise FALSE
 */
static bool_e hasBumped();

/**
 * @brief Convert the velocity vector into a cosign for the robot
 *
 * @param vector Speed vector
 */
static void sendMvt(VelocityVector_s vector);

/**
 * @brief Check if our vector allows the robot to move forward
 *
 * @param vector Speed vector
 */
static void checkVector(VelocityVector_s vector);

/**
 * @brief State machine of our pilot
 *
 * @param vector Speed vector
 */
static void run(Event_e event, VelocityVector_s vector);

static PilotState_s *p_pilot = 0;
static State_e currentState;
static VelocityVector_s vectorDefault = {D_STOP, 0};

typedef void (*action_p)();
static const action_p a_actionTab[A_NB_ACTION] = {&Pilot_check, &checkVector, &Pilot_stop, &sendMvt};

extern void Pilot_new()
{
    p_pilot = (PilotState_s *)malloc(sizeof(PilotState_s));
}

extern void Pilot_free()
{
    Robot_free();
}

extern void Pilot_start()
{
    Robot_start();
    currentState = S_IDLE;
}

extern void Pilot_stop(VelocityVector_s vector)
{
    Robot_stop();
    currentState = S_IDLE;
}

extern void Pilot_setVelocity(VelocityVector_s vector)
{
    run(E_CHANGE_MVT, vector);
}

extern PilotState_s Pilot_getState()
{
    p_pilot->collision = hasBumped();
    p_pilot->luminosity = Robot_getSensorState().luminosity;
    p_pilot->speed = Robot_getRobotSpeed();
    run(E_ASK_LOG, vectorDefault);

    return *p_pilot;
}

extern void Pilot_check(VelocityVector_s vector)
{
    if (!hasBumped())
    {
        currentState = S_RUNNING;
    }
    else
    {
        run(E_BUMPED, vectorDefault);
    }
}

static bool_e hasBumped()
{
    const bool_e bumped = (Robot_getSensorState().collision == BUMPED);
    return bumped;
}

static void sendMvt(VelocityVector_s vector)
{
    switch (vector.dir)
    {
    case D_FORWARD:
        Robot_setWheelsVelocity(vector.power, vector.power);
        break;
    case D_BACKWARD:
        Robot_setWheelsVelocity(-vector.power, -vector.power);
        break;
    case D_LEFT:
        Robot_setWheelsVelocity(-vector.power, vector.power);
        break;
    case D_RIGHT:
        Robot_setWheelsVelocity(vector.power, -vector.power);
        break;
    default:
        run(E_STOP, vectorDefault);
        break;
    }
}

static void checkVector(VelocityVector_s vector)
{
    if (vector.dir == D_STOP)
    {
        run(E_STOP, vectorDefault);
    }
    else
    {
        run(E_CHANGE_MVT, vector);
    }
}

static void run(Event_e event, VelocityVector_s vector)
{
    const Action_e action = a_stateMachine[currentState][event].action;
    const State_e stateNext = a_stateMachine[currentState][event].stateNext;

    if (stateNext != S_NONE)
    {
        currentState = stateNext;
        a_actionTab[action](vector);
    }
}