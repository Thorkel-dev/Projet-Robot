/**
 * @file  remoteUI.h
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

#ifndef _REMOTE_UI_
#define _REMOTE_UI_

/**
 * @brief Initializes the interface and the client
 */
extern void RemoteUI_new();

/**
 * @brief Start the interface and the client
 */
extern void RemoteUI_start();

/**
 * @brief Stopping interface et client
 */
extern void RemoteUI_stop();

#endif // _REMOTE_UI_
