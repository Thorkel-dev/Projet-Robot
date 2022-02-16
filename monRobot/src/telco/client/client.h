#include "../../common.h"

#ifndef _CLIENT_
#define _CLIENT_

typedef struct client_t Client;

extern void Client_new();
extern int *Client_start();
extern void Client_stop();
extern void Client_sendMsg(Data_s p_data);
extern Data_s Client_readMsg();

#endif // _CLIENT_
