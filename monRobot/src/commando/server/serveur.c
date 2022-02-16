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

#define MAX_PENDING (1)
#define MAX_CONNECTION_ATTEMPT (60)

/**
 * @brief Vitesse par défaut du robot
 */
#define POWER 100

static int s_socket_ecoute;
static int s_socket_donnees;
static struct sockaddr_in s_adresse;

static void sendMsg(PilotState_s pilot);
static void readMsg();

/**
 * @brief Convertie de la direction choisie par l'utilisateur en un vecteur vitesse
 *
 * @param direction la direction dans laquelle le robot doit aller.
 * @return VelocityVector_s le vector vitesse correspond à la direction où aller.
 */
static VelocityVector_s translate(const Direction_e direction);
static Data_s convertData(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity);
static void run();

static bool_e works;
static VelocityVector_s vectorDefault = {D_STOP, 0};
struct timeval timeout = {1, 0};

extern void
Server_new()
{
    TRACE("Le serveur est crée\n");
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
    printf("%sLe serveur est sur le port %d à l'adresse %s%s\n\n",
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
    TRACE("Le serveur est arrété\n");
    close(s_socket_ecoute);
}

static void sendMsg(const PilotState_s pilot)
{
    Data_s data = convertData(0, 0, pilot.speed, pilot.collision, pilot.luminosity);
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
            quantityToWrite -= quantityWritten;
        }
    }
    TRACE("Server send message\n");
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
            quantityToRead -= quantityReaddean;
        }
    }

    if (quantityToRead == 0)
    {
        data = convertData(data.order, data.direction, 0, 0, 0);
        TRACE("Receive data:\n\tDirection: %d\n\tEvent:%d\n", data.direction, data.order);
        if (data.order == O_ASK_LOG)
        {
            sendMsg(Pilot_getState());
        }
        else if (data.order == O_CHANGE_MVT)
        {
            Pilot_setVelocity(translate(data.direction));
        }
        else
        {
            works = FALSE;
            Pilot_stop(vectorDefault);
            Pilot_free();
        }
    }
}

static VelocityVector_s translate(const Direction_e direction)
{
    VelocityVector_s velocityVector = {direction, POWER};
    return velocityVector;
}

static Data_s convertData(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity)
{
    Data_s data;
    data.direction = ntohl(direction);
    data.order = ntohl(order);
    data.collision = ntohl(collision);
    data.luminosity = ntohl(luminosity);
    data.speed = ntohl(speed);
    return data;
}

static void run()
{
    bool_e clientConnect = FALSE;
    int timeoutcount = 0;
    fd_set readFd;
    FD_ZERO(&readFd); // Initialisation

    while (works == TRUE && timeoutcount < MAX_CONNECTION_ATTEMPT)
    {
        FD_SET(STDIN_FILENO, &readFd);    // Option standar d'écoute
        FD_SET(s_socket_ecoute, &readFd); // Lié au socket serveur
        struct termios oldt, newt;
        // Ecrit les paramètres de stdin sur old
        tcgetattr(STDIN_FILENO, &oldt);

        newt = oldt;

        newt.c_lflag &= ~(ICANON | ECHO); // Fait les flags de new comparé à l'opposé de ICANON et ECHO

        // Change les attributs immédiatement
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        if (clientConnect) // On vérifie s'il on est connécté
        {
            FD_SET(s_socket_donnees, &readFd); // Lié au socket client
        }
        else // On n'est pas connecté
        {
            printf("%s%s%sConnection failure, attempt %d / %d%s\n",
                   "\033[1A", "\033[K", "\033[33m", timeoutcount, MAX_CONNECTION_ATTEMPT, "\033[0m");
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
                }
                else
                {
                    clientConnect = TRUE;
                    printf("%s%s%sConnection réussite%s\n",
                           "\033[1A", "\033[K", "\033[33m", "\033[0m");
                }
            }
            else if (FD_ISSET(s_socket_donnees, &readFd))
            {
                readMsg();
            }
        }
        // On remet les anciens paramètres
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
    close(s_socket_donnees);
    printf("Client deconnecté");
}
