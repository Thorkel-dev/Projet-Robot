#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../common.h"
#include "server/server.h"
#include "pilot/pilot.h"

bool_e g_keepGoing = TRUE;

static void intHandler(int dummy)
{
    TRACE("%sCtrl+C pressed\n","\033[0m");

    g_keepGoing = FALSE;
}

int main(int argc, char *argv[])
{
#ifdef INTOX
    if (ProSE_Intox_init("127.0.0.1", 12345) == -1)
    {
        PProseError("Problem initializing the Intox simulator");
        return EXIT_FAILURE;
    }
#endif
    printf("\033[2J\033[;H");
    signal(SIGINT, intHandler);

    Pilot_new();
    Server_new();

    Pilot_start();
    Server_start();

    Pilot_stop();
    Server_stop();

    Pilot_free();
    Server_free();

#ifdef INTOX
    ProSE_Intox_close();
#endif

    return EXIT_SUCCESS;
}
