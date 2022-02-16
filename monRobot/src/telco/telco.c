#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../common.h"
#include "client/client.h"
#include "remoteUI/remoteUI.h"

int main(int argc, char *argv[])
{
    printf("\033c");
    RemoteUI_new();
    RemoteUI_start();
    RemoteUI_stop();

    return EXIT_SUCCESS;
}
