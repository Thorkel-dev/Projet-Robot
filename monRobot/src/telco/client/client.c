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

#include "client.h"
#include "../../common.h"
#include "../util.h"

#define MAX_CONNECTION_ATTEMPT 60

int s_socket;
struct sockaddr_in s_server_address;
Data_s data;

static Data_s convertData(const Order_e order, const Direction_e direction, const int speed, const bool_e collision, const int luminosity);

extern void Client_new()
{
    const struct hostent *l_hostInfo = gethostbyname(IP_SERVER);

    if (l_hostInfo == NULL)
    {
        printf("%sUnknown host%s\n", "\033[41m", "\033[0m");
        Client_stop();
        return;
    }
    s_server_address.sin_port = htons(PORT_SERVER);
    s_server_address.sin_family = AF_INET;
    s_server_address.sin_addr = *((struct in_addr *)l_hostInfo->h_addr_list[0]);

    s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (s_socket < 0)
    {
        printf("%sError socket%s\n", "\033[41m", "\033[0m");
        Client_stop();
        return;
    }
    TRACE("The client is created\n");
}

extern int *Client_start()
{
    int *l_ret = 0;
    int l_attemptCounter = 0;

    printf("%sConnection tent to the server%s\n\n", "\033[34m", "\033[0m");

    while (l_attemptCounter < MAX_CONNECTION_ATTEMPT)
    {
        l_attemptCounter++;
        if (connect(s_socket, (struct sockaddr *)&s_server_address, sizeof(s_server_address)) < 0)
        {
            printf("%s%s%sConnection failure, attempt %d / %d%s\n",
                   "\033[1A", "\033[K", "\033[33m", l_attemptCounter, MAX_CONNECTION_ATTEMPT, "\033[0m");
            sleep(1); // Sleep and retry after
        }
        else
        {
            break;
        }
    }

    l_ret = &s_socket;
    return l_ret;
}

extern void Client_stop()
{
    TRACE("The client is OFF\n");
    close(s_socket);
}

extern void Client_sendMsg(Data_s data)
{
    data = convertData(data.order, data.direction, data.speed, data.collision, data.luminosity);

    int l_quantityWritten = 0;
    int l_quantityToWrite = sizeof(data);

    while (l_quantityToWrite > 0)
    {
        l_quantityWritten += write(s_socket, &data, l_quantityToWrite);

        if (l_quantityWritten < 0)
        {
            printf("%sError when sending the message%s\n", "\033[41m", "\033[0m");
            break;
        }
        else
        {
            l_quantityToWrite -= l_quantityWritten;
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
        quantityReaddean = read(s_socket, &data + quantityReaddean, quantityToRead);

        if (quantityReaddean < 0)
        {
            printf("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
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
        TRACE("Receive data\n\tDirection: %d\n", data.direction);
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
