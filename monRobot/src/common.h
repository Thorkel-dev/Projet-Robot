#ifndef _CONFIG_
#define _CONFIG_

#define IP_SERVER "127.0.0.1"
#define PORT_SERVER 8080
#define SIZE_MESSAGE 128
#define SIZE_ID 8

typedef enum
{
    E_CHANGE_MVT = 0,
    E_ASK_LOG,
    E_DISCONNECT,
    E_NB_EVENT
} Event;

typedef enum
{
    D_STOP = 0,
    D_FORWARD,
    D_BACK,
    D_LEFT,
    D_RIGHT,
} Direction;

typedef struct
{
    Event event;
    Direction direction;
    char id[SIZE_ID];
    char message[SIZE_MESSAGE];
} Data;

typedef unsigned int sizeData;

extern bool_e g_keepGoing;


#endif // _CONFIG_
