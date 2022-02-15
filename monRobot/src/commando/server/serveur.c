/**
 * @file serveur.c
 *
 * @see serveur.h
 *
 * @author Gautier Edouard
 */

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
#include "../pilot/pilot.h"
#include "../util.h"

#define MAX_PENDING (1)

/**
 * @brief Vitesse par défaut du robot
 */
#define POWER 100

static int s_socket_ecoute;
static int s_socket_donnees;
static struct sockaddr_in s_adresse;

static void sendMsg(PilotState_s data);
static void readMsg();

/**
 * @brief Convertie de la direction choisie par l'utilisateur en un vecteur vitesse
 *
 * @param direction la direction dans laquelle le robot doit aller.
 * @return VelocityVector_s le vector vitesse correspond à la direction où aller.
 */
static VelocityVector_s translate(const Direction_e direction);

static void run();

static bool_e works;
static VelocityVector_s vectorDefault = {STOP, 0};
struct timeval timeout = {1, 0};

extern void
Server_new()
{
    TRACE("%s%sLe serveur est crée%s\n\n", "\033[44m", "\033[37m", "\033[0m");
    s_socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if (s_socket_ecoute == -1)
    {
        perror("Erreur dans la création du socket");
        exit(s_socket_ecoute);
    }
    s_adresse.sin_family = AF_INET;
    s_adresse.sin_port = htons(PORT_SERVER);
    s_adresse.sin_addr.s_addr = htonl(INADDR_ANY);
    Pilot_new();
}

extern void Server_start()
{
    printf("%sLe serveur est sur le port %d à l'adresse %s%s\n",
           "\033[32m", PORT_SERVER, IP_SERVER, "\033[0m");

    bind(s_socket_ecoute, (struct sockaddr *)&s_adresse, sizeof(s_adresse));

    if (listen(s_socket_ecoute, MAX_PENDING) != 0)
    {
        printf("%sErreur durant l'écoute du port%s\n", "\033[41m", "\033[0m");
        Server_stop();
        return;
    }
    Pilot_start();
    works = TRUE;
    run();
}

extern void Server_stop()
{
    TRACE("%sLe serveur est arrété%s\n", "\033[31m", "\033[0m");
    Pilot_setVelocity(vectorDefault);
    works = FALSE;
    close(s_socket_ecoute);
    Pilot_free();
}

static void sendMsg(PilotState_s data)
{
    data.collision = htonl(data.collision);
    data.luminosity = htonl(data.luminosity);
    data.speed = htonl(data.speed);
    int quantityWritten = 0;
    int quantityToWrite = sizeof(data);

    while (quantityToWrite > 0)
    {
        quantityWritten = write(s_socket_donnees, &data + quantityWritten, quantityToWrite);

        if (quantityWritten < 0)
        {
            printf("%sError when sending the message%s\n", "\033[41m", "\033[0m");
            break;
        }
        else
        {
            TRACE("Not all the message has been sent%s", "\n");
            quantityToWrite -= quantityWritten;
        }
    }
    TRACE("%sServer send message%s\n", "\033[36m", "\033[0m");
}

static void readMsg()
{
    Data_s data;
    int quantityToRead = sizeof(Data_s);
    int quantityReaddean = 0;

    while (quantityToRead > 0)
    {
        quantityReaddean = read(s_socket_donnees, &data + quantityReaddean, quantityToRead);

        if (quantityReaddean < 0)
        {
            printf("%sError when receiving the message%s\n", "\033[41m", "\033[0m");
            break;
        }
        else
        {
            TRACE("Not all the message has been read\n");
            quantityToRead -= quantityReaddean;
        }
    }
    if (quantityToRead == 0)
    {
        data.direction = ntohl(data.direction);
        data.event = ntohl(data.event);
        TRACE("Receive data:%d\n\tDirection: %d\n\tEvent: %s\n\n", data.direction, data.event);
        if (data.event == O_ASK_LOG)
        {
            sendMsg(Pilot_getState());
        }
        else
        {
            Pilot_setVelocity(translate(data.direction));
        }
    }
}

VelocityVector_s translate(const Direction_e direction)
{
    VelocityVector_s velocityVector = {direction, POWER};
    return velocityVector;
}

static void run()
{
    bool_e clientConnect = FALSE;
    int timeoutcount = 0;
    fd_set readFd;
    FD_ZERO(&readFd);                 // Initialisation
    FD_SET(STDIN_FILENO, &readFd);    // Option standar d'écoute
    FD_SET(s_socket_ecoute, &readFd); // Lié au socket serveur

    while (works == TRUE && timeoutcount < 6)
    {
        if (clientConnect) // On vérifie s'il on est connécté
        {
            FD_SET(s_socket_donnees, &readFd); // Lié au socket client
        }
        else // On est connecté
        {
            printf("wait\n");
        }

        int rc = select(FD_SETSIZE, &readFd, NULL, NULL, &timeout);
        // On surveille un descripteur
        if (rc == -1)
        {
            break; // erreur
        }
        else if (rc == 0)
        {
            timeoutcount++;
        }
        else
        {
            timeoutcount = 0;
            // On vérifie si les descripteurs sont présent
            if (FD_ISSET(s_socket_ecoute, &readFd))
            {
                s_socket_donnees = accept(s_socket_ecoute, NULL, 0); // Connection

                if (s_socket_donnees < 0) // Vérification
                {
                    clientConnect = FALSE;
                    Server_stop();
                }
                else
                {
                    clientConnect = TRUE;
                }
            }
            else if (FD_ISSET(s_socket_donnees, &readFd))
            {
                readMsg();
            }
        }
    }
    close(s_socket_donnees);
    printf("Client deconnecté");
}
