#ifndef _CONFIG_
#define _CONFIG_

#define IP_SERVER "127.0.0.1"
#define PORT_SERVER 2345
#define SIZE_MESSAGE 128

#include "../../infox_prose-x86_64-v0.3/include/infox/prose/prose.h"
typedef enum
{
    STOP = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
} Direction_e;

typedef enum
{
    O_CHANGE_MVT = 0,
    O_ASK_LOG,
    O_NB_EVENT
} Order_e;

typedef struct
{
    Order_e event;
    Direction_e direction;
} Data_s;

#endif // _CONFIG_
