#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../common.h"
#include "server/server.h"

int main(int argc, char *argv[])
{
    printf("\033c");
    Server_new();
    Server_start();
    Server_stop();
    return EXIT_SUCCESS;
}
