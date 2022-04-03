/**
 * @file serveur.c
 *
 * @see serveur.h
 *
 * @author Thorkel-dev
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

#include "../../common.h"
#include "../pilot/pilot.h"
#include "server.h"

#define MAX_PENDING (1)
#define MAX_CONNECTION_ATTEMPT (60)

/**
 * @brief Default speed of the robot
 */
#define POWER (100)

/**
 * @brief Returns the status of the pilot to the client
 *
 * @param pilot Status of the pilot
 */
static void sendMsg(PilotState_s pilot);

/**
 * @brief Read messages received from the client
 */
static void readMsg();

static int socket_ecoute;
static int socket_donnees;
static struct sockaddr_in adresse;

/**
 * @brief Convert the direction chosen by the user into a velocity vector
 *
 * @param direction The direction in which the robot should go
 * @return VelocityVector_s corresponds to the direction and speed where to go
 */
static VelocityVector_s translate(const Direction_e direction);

/**
 * @brief Convert data to Byte order for host
 *
 * @param order The type of order
 * @param direction The direction in which the robot should go.
 * @param speed The speed
 * @param collision Collision sensor status
 * @param luminosity The luminosity measured
 *
 * @return Data_s convert data to Byte order
 */
static Data_s convertDataReception(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity);

/**
 * @brief Convert data to Byte order for network
 *
 * @param order The type of order
 * @param direction The direction in which the robot should go.
 * @param speed The speed
 * @param collision Collision sensor status
 * @param luminosity The luminosity measured
 *
 * @return Data_s convert data to Byte order
 */
static Data_s convertDataSend(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity);

/**
 * @brief Allows the server to run after it is launched
 */
static void run();

static bool_e work;
static VelocityVector_s vectorDefault = {D_STOP, 0};

extern void Server_new()
{
    TRACE("The server is created\n");
    socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ecoute == -1)
    {
        perror("Erreur dans la création du socket");
        exit(socket_ecoute);
    }
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(PORT_SERVER);
    adresse.sin_addr.s_addr = htonl(INADDR_ANY);
    Pilot_new();
}

extern void Server_start()
{
    printf("%sLe serveur est sur le port %d à l'adresse %s%s\n\n", "\033[32m", PORT_SERVER, IP_SERVER, "\033[0m");
    bind(socket_ecoute, (struct sockaddr *)&adresse, sizeof(adresse));

    if (listen(socket_ecoute, MAX_PENDING) != 0)
    {
        printf("%sErreur durant l'écoute du port%s\n", "\033[41m", "\033[0m");
        Server_stop();
        return;
    }
    Pilot_start();
    work = TRUE;
    run();
}

extern void Server_stop()
{
    printf("%sLe serveur est arrêté%s\n", "\033[31m", "\033[0m");
    close(socket_ecoute);
}

static void sendMsg(const PilotState_s pilot)
{
    Data_s data = convertDataSend(0, 0, pilot.speed, pilot.collision, pilot.luminosity);
    int quantityWritten = 0;
    int quantityToWrite = sizeof(data);

    quantityWritten = write(socket_donnees, &data + quantityWritten, quantityToWrite);

    if (quantityWritten < 0)
    {
        printf("%sErreur lors de l'envoi du message%s\n", "\033[41m", "\033[0m");
    }
    TRACE("Send data:\tDirection: %d - Event: %d - Speed: %d - Collision: %d - Luminosity: %d\n", data.direction, data.order, data.speed, data.collision, data.luminosity);
}

static void readMsg()
{
    Data_s data = {0, 0, 0, 0, 0};
    int quantityToRead = sizeof(Data_s);
    int quantityReaddean = 0;

    while (quantityToRead > 0)
    {
        quantityReaddean = read(socket_donnees, &data + quantityReaddean, quantityToRead);

        if (quantityReaddean < 0)
        {
            printf("%sErreur lors de la réception du message%s\n", "\033[41m", "\033[0m");
            break;
        }
        else
        {
            quantityToRead -= quantityReaddean;
        }
    }

    if (quantityToRead == 0)
    {
        data = convertDataReception(data.order, data.direction, 0, 0, 0);
        TRACE("Receive data:\tDirection: %d - Event: %d\n", data.direction, data.order);
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
            work = FALSE;
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

static Data_s convertDataReception(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity)
{
    Data_s data;
    data.direction = ntohl(direction);
    data.order = ntohl(order);
    data.collision = ntohl(collision);
    data.luminosity = ntohl(luminosity);
    data.speed = ntohl(speed);
    return data;
}

static Data_s convertDataSend(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity)
{
    Data_s data;
    data.direction = htonl(direction);
    data.order = htonl(order);
    data.collision = htonl(collision);
    data.luminosity = htonl(luminosity);
    data.speed = htonl(speed);
    return data;
}

static void run()
{
    bool_e clientConnect = FALSE;
    int timeoutCount = 0;
    fd_set readFd;
    FD_ZERO(&readFd); // Initialization of the file descriptor

    while (work == TRUE && timeoutCount < MAX_CONNECTION_ATTEMPT)
    {
        struct termios oldt, newt;
        // Write stdin parameters to old
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Makes the flags of new compared to ICANON and ECHO

        // Change the attributes immediately
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        FD_SET(STDIN_FILENO, &readFd);  // The terminal
        FD_SET(socket_ecoute, &readFd); // Server Socket
        struct timeval timeout = {60, 0}; //Time out for select()
        
        if (clientConnect) // We check if we are connected
        {
            FD_SET(socket_donnees, &readFd); // Client socket
        }
        else // We are not connected
        {
            printf("%s%s%sTentative de connexion...%s\n", "\033[1A", "\033[K", "\033[33m", "\033[0m");
        }

        int rc = select(FD_SETSIZE, &readFd, NULL, NULL, &timeout);
        // We monitor a descriptor
        if (rc == -1)
        {
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break; // Error
        }
        else if (rc == 0)
        {
            TRACE("Client not connected or inactive")
            work = FALSE;
        }
        else
        {
            // We check if the descriptors are present
            if (FD_ISSET(socket_ecoute, &readFd))
            {
                socket_donnees = accept(socket_ecoute, NULL, 0); // Connection

                if (socket_donnees < 0) // Verification
                {
                    clientConnect = FALSE;
                }
                else
                {
                    clientConnect = TRUE;
                    timeoutCount = 0;
                    printf("%s%s%sConnexion réussite%s\n", "\033[1A", "\033[K", "\033[33m", "\033[0m");
                }
            }
            else if (FD_ISSET(socket_donnees, &readFd))
            {
                readMsg();
            }
            else if (FD_ISSET(STDIN_FILENO, &readFd))
            {
                TRACE("Utilisation du terminal");
                break;
            }
        }
        // We put back the old parameters
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
    close(socket_donnees);
    printf("%sClient déconnecté%s\n", "\033[31m", "\033[0m");
}
