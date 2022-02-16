#ifndef _CONFIG_
#define _CONFIG_

#define IP_SERVER "127.0.0.1"
#define PORT_SERVER 2345
#define SIZE_MESSAGE 128

#include "../../infox_prose-x86_64-v0.3/include/infox/prose/prose.h"
#include "util.h"
typedef enum
{
    D_STOP = 0,
    D_FORWARD,
    D_BACKWARD,
    D_LEFT,
    D_RIGHT,
    D_NB_DIRECTION
} Direction_e;

/**
 * @brief Structure de l'état du robot
 */
typedef struct
{
    int speed;
    bool_e collision;
    int luminosity;
} PilotState_s;

typedef enum
{
    O_CHANGE_MVT = 0,
    O_ASK_LOG,
    O_NB_ORDER
} Order_e;

typedef struct
{
    Order_e order;
    Direction_e direction;
    int speed;
    bool_e collision;
    int luminosity;
} Data_s;

#endif // _CONFIG_
