#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "server.h"
#include "../../common.h"
#include "pilot/pilot.h"

#define MAX_PENDING (1)

static int s_socket_ecoute;
static int s_socket_donnees;
static struct sockaddr_in s_adresse;
static bool_e s_clientIsConnected = FALSE;

static void sendMsg(Data *p_data);
static void readMsg(void);
static void run(void);

extern void Server_new(void)
{
    TRACE("%s%sThe server is created%s\n\n", "\033[44m", "\033[37m", "\033[0m");

    s_socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    s_adresse.sin_family = AF_INET;
    s_adresse.sin_port = htons(PORT_SERVER);
    s_adresse.sin_addr.s_addr = htonl(INADDR_ANY);
}

extern void Server_start(void)
{
    printf("%sThe server is serving on port %d at the address %s%s\n",
           "\033[32m", PORT_SERVER, IP_SERVER, "\033[0m");
    printf("%sTo shutdown the server press enter%s\n\n", "\033[36m", "\033[0m");

    bind(s_socket_ecoute, (struct sockaddr *)&s_adresse, sizeof(s_adresse));

    if (listen(s_socket_ecoute, MAX_PENDING) != 0)
    {
        printf("%sError while listenning the port%s\n", "\033[41m", "\033[0m");
        Server_stop();
        return;
    }

    run();
}

extern void Server_stop(void)
{
    TRACE("%s%sThe socket is closed%s\n", "\033[43m", "\033[38m", "\033[0m");

    close(s_socket_ecoute);
    g_keepGoing = FALSE;
}

static void sendMsg(Data *p_data)
{
    static int l_count = 0;
    l_count++;

    sprintf(p_data->id, "S-%d", l_count);

    p_data->event = htonl(p_data->event);
    p_data->direction = htonl(p_data->direction);

    sizeData l_quantityWritten = 0;
    sizeData l_quantityToWrite = sizeof(*p_data);

    while (l_quantityToWrite > 0)
    {
        l_quantityWritten = write(s_socket_donnees, p_data + l_quantityWritten, l_quantityToWrite);

        if (l_quantityWritten < 0)
        {
            printf("%sError when sending the message %s%s\n", "\033[41m", p_data->id, "\033[0m");
            break; // l_quantityWritten
        }
        else
        {
            TRACE("Not all the message has been sent%s", "\n");

            l_quantityToWrite -= l_quantityWritten;
        }
    }

    TRACE("%sServer send message %s%s\n", "\033[36m", p_data->id, "\033[0m");
}

static void readMsg(void)
{
    Data l_data;

    sizeData l_quantityReaddean = 0;
    sizeData l_quantityToRead = sizeof(Data);

    while (l_quantityToRead > 0)
    {
        l_quantityReaddean = read(s_socket_donnees, &l_data + l_quantityReaddean, l_quantityToRead);

        if (l_quantityReaddean < 0)
        {
            printf("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
            break; // l_quantityWritten
        }
        else if (l_quantityReaddean == 0)
        {
            Pilot_update(E_DISCONNECT, D_STOP);

            disconnectClient();
            break; // l_quantityWritten
        }
        else
        {
            TRACE("Not all the message has been read%s", "\n");
            l_quantityToRead -= l_quantityReaddean;
        }
    }

    if (l_quantityToRead == 0)
    {
        l_data.event = ntohl(l_data.event);
        l_data.direction = ntohl(l_data.direction);

        TRACE("Receive data:\n\tId: %s\n\tEvent: %d\n\tDirection: %d\n\tMessage: %s\n\n",
              l_data.id, l_data.event, l_data.direction, l_data.message);

        Pilot_update(l_data.event, l_data.direction);
    }
}

static void run(void)
{
    fd_set l_env;

    while (g_keepGoing == TRUE)
    {
        FD_ZERO(&l_env);
        FD_SET(STDIN_FILENO, &l_env);
        FD_SET(s_socket_ecoute, &l_env);

        if (s_clientIsConnected == TRUE)
        {
            FD_SET(s_socket_donnees, &l_env);
        }
        else
        {
            printf("Waiting for a client...\n");
        }

        if (select(FD_SETSIZE, &l_env, NULL, NULL, NULL) == -1)
        {
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break; // g_keepGoing
        }

        if (FD_ISSET(STDIN_FILENO, &l_env))
        {
            printf("\n%sAsk for exit%s\n", "\033[41m", "\033[0m");

            Server_stop();
            break; // g_keepGoing
        }
        else if (FD_ISSET(s_socket_ecoute, &l_env))
        {
            connectClient();
        }
        else if (FD_ISSET(s_socket_donnees, &l_env))
        {
            readMsg();
        }
    }

    TRACE("%s%sThe server is no more serving%s\n", "\033[43m", "\033[38m", "\033[0m");
}
