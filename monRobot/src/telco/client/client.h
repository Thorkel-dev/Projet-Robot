/**
 * @file  client.h
 *
 * @brief  description
 *
 * @author Thorkel-dev
 * @date 16-02-2022
 * @version version 1
 * @section License
 *
 * The MIT License
 *
 * Copyright (c) 2022, Thorkel-dev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "../../common.h"

#ifndef _CLIENT_
#define _CLIENT_

/**
 * @brief Initializes the client and the connection
 */
extern void Client_new();

/**
 * @brief Starts the client
 *
 * @return int* socket for connexion
 */
extern int *Client_start();

/**
 * @brief Stopping client
 */
extern void Client_stop();

/**
 * @brief Send the data for the pilot
 *
 * @param data Data to be sent
 */
extern void Client_sendMsg(Data_s data);

/**
 * @brief Read messages received from the server
 *
 * @return Data_s Data sent
 */
extern Data_s Client_readMsg();

#endif // _CLIENT_
