#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include "../../common.h"
#include "../util.h"
#include "client.h"

#define MAX_CONNECTION_ATTEMPT (60)

static int socket_ecoute;
static struct sockaddr_in server_address;

/**
 * @brief Convert data to Byte order
 *
 * @param order The type of order
 * @param direction The direction in which the robot should go.
 * @param speed The speed
 * @param collision Collision sensor status
 * @param luminosity The luminosity measured
 *
 * @return Data_s convert data to Byte order
 */
static Data_s convertData(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity);

extern void Client_new()
{
    const struct hostent *host = gethostbyname(IP_SERVER);

    if (host == NULL)
    {
        printf("%sHôte inconnu%s\n", "\033[41m", "\033[0m");
        Client_stop();
        return;
    }
    server_address.sin_port = htons(PORT_SERVER);
    server_address.sin_family = AF_INET;
    server_address.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ecoute < 0)
    {
        printf("%sErreur dans le socket%s\n", "\033[41m", "\033[0m");
        Client_stop();
        return;
    }
    TRACE("The client is created\n");
}

extern int *Client_start()
{
    int timeoutCounter = 0;

    printf("%sConnection tent to the server%s\n\n", "\033[34m", "\033[0m");

    while (timeoutCounter < MAX_CONNECTION_ATTEMPT)
    {
        timeoutCounter++;
        if (connect(socket_ecoute, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        {
            printf("%s%s%sÉchec de la connexion, tentative n°%d / %d%s\n", "\033[1A", "\033[K", "\033[33m", timeoutCounter, MAX_CONNECTION_ATTEMPT, "\033[0m");
            sleep(1); // Sleep and retry after
        }
        else
        {
            printf("%s%s%sConnexion réussite%s\n", "\033[1A", "\033[K", "\033[33m", "\033[0m");
            break;
        }
    }
    return &socket_ecoute;
}

extern void Client_stop()
{
    TRACE("The client is OFF\n");
    close(socket_ecoute);
}

extern void Client_sendMsg(Data_s data)
{
    data = convertData(data.order, data.direction, data.speed, data.collision, data.luminosity);

    int quantityWritten = 0;
    int quantityToWrite = sizeof(data);

    while (quantityToWrite > 0)
    {
        quantityWritten += write(socket_ecoute, &data, quantityToWrite);

        if (quantityWritten < 0)
        {
            printf("%sErreur lors de l'envoi du message%s\n", "\033[41m", "\033[0m");
            break;
        }
        else
        {
            quantityToWrite -= quantityWritten;
        }
    }
}

extern Data_s Client_readMsg()
{
    Data_s data;
    int quantityToRead = sizeof(Data_s);
    int quantityReaddean = 0;

    while (quantityToRead > 0)
    {
        quantityReaddean = read(socket_ecoute, &data + quantityReaddean, quantityToRead);

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
        data = convertData(data.order, data.direction, data.speed, data.collision, data.luminosity);
        TRACE("Receive data:\tDirection: %d - Event: %d - Speed: %d - Collision: %d - Luminosity: %d\n\n", data.direction, data.order, data.speed, data.collision, data.luminosity);
    }

    return data;
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
