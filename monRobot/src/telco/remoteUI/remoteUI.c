/**
 * @file remoteUI.c
 *
 * @see remoteUI.h
 *
 * @author Thorkel-dev
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "../../common.h"
#include "../client/client.h"
#include "remoteUI.h"

/**
 * @brief The key for turning the robot to the left
 */
#define LEFT_KEY 'q'

/**
 * @brief The button for turning the robot to the right
 */
#define RIGHT_KEY 'd'

/**
 * @brief The button to move the robot backwards
 */
#define FORWARD_KEY 'z'

/**
 * @brief The button for moving the robot forward
 */
#define BACK_KEY 's'

/**
 * @brief The button to stop the robot
 */
#define STOP_KEY ' '

/**
 * @brief The button for cleaning the terminal
 */
#define ERASE_LOG_KEY 'e'

/**
 * @brief The button for displaying the status of the robot
 */
#define DISPLAY_STATE_KEY 'r'

/**
 * @brief The exit key
 */
#define QUIT_KEY 'a'

/**
 * @brief Displays the keys of the remote control
 */
static void display();

/**
 * @brief Retrieves the key entered by the user
 */
static void capturechoise(char carractere);

/**
 * @brief Launch the application
 */
static void run();

/**
 * @brief Transmits the direction given to Pilot.
 *
 * @param direction the requested direction.
 */
static void askMvt(Direction_e p_dir);

/**
 * @brief Displays the status of the robot
 */
static void ask4Log();

/**
 * @brief Clears text from terminal
 */
static void askClearLog();

static bool_e work;
static int socket_donnees;

extern void RemoteUI_new()
{
    Client_new();
}

extern void RemoteUI_start()
{
    socket_donnees = *Client_start();
    work = TRUE;
    run();
}

extern void RemoteUI_stop()
{
    printf("\n%sStop%s\n", "\033[31m", "\033[0m");
    Data_s data = {0, 0, 0, 0, 0};

    data.order = O_STOP;
    Client_sendMsg(data);
    work = FALSE;
}

static void askMvt(Direction_e p_dir)
{
    TRACE("Some Moves - Direction is %d\n", p_dir);

    Data_s data = {0, 0, 0, 0, 0};
    data.direction = p_dir;
    data.order = O_CHANGE_MVT;
    Client_sendMsg(data);
}

static void ask4Log()
{
    askClearLog();

    Data_s data = {0, 0, 0, 0, 0};
    data.order = O_ASK_LOG;
    Client_sendMsg(data);
}

static void askClearLog()
{
    printf("\033c");
    display();
}

static void display()
{
    printf("Vous pouvez faire les actions suivantes :\n");
    printf("%c : Gauche\n", LEFT_KEY);
    printf("%c : Droite\n", RIGHT_KEY);
    printf("%c : Avancer\n", FORWARD_KEY);
    printf("%c : Reculer\n", BACK_KEY);
    printf("%c : Stopper\n", STOP_KEY);
    printf("%c : Effacer\n", ERASE_LOG_KEY);
    printf("%c : Afficher l'état du robot\n\n", DISPLAY_STATE_KEY);
    printf("%c : \033[31mQuitter\033[00m\n\n", QUIT_KEY);
}

static void capturechoise(char carractere)
{
    switch (carractere)
    {
    case LEFT_KEY:
        askMvt(D_LEFT);
        break;
    case RIGHT_KEY:
        askMvt(D_RIGHT);
        break;
    case FORWARD_KEY:
        askMvt(D_FORWARD);
        break;
    case BACK_KEY:
        askMvt(D_BACKWARD);
        break;
    case STOP_KEY:
        askMvt(D_STOP);
        break;
    case ERASE_LOG_KEY:
        askClearLog();
        break;
    case DISPLAY_STATE_KEY:
        ask4Log();
        break;
    case QUIT_KEY:
        work = FALSE;
        break;
    default:
        break;
    }
}

static void run()
{

    if (work == TRUE)
    {
        askClearLog();
    }

    fd_set writeFd;
    FD_ZERO(&writeFd); // Initialization of the file descriptor

    while (work == TRUE)
    {
        struct termios oldt, newt;
        FD_SET(socket_donnees, &writeFd); // Client Socket
        FD_SET(STDIN_FILENO, &writeFd);   // The terminal

        // Write stdin parameters to old
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Makes the flags of new compared to ICANON and ECHO

        // Change the attributes immediately
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        if (select(FD_SETSIZE, &writeFd, NULL, NULL, NULL) == -1)
        {
            break; // Error
        }
        // We put back the old parameters
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        if (FD_ISSET(STDIN_FILENO, &writeFd))
        {
            char charInput[2]; // Size 2 for the null character

            if (read(STDIN_FILENO, charInput, sizeof(charInput)) < 0)
            {
                printf("%sErreur lors de la lecture%s\n", "\033[41m", "\033[0m");
            }
            else
            {
                capturechoise(charInput[0]);
            }
        }
        else if (FD_ISSET(socket_donnees, &writeFd))
        {
            const Data_s pilotState = Client_readMsg();

            printf("\033[1A\033[K"); // Position the cursor 1 line above and delete it
            printf("\nVitesse du robot : %d cm/s\n", pilotState.speed);
            printf("Collision : %s\033[0m\n", pilotState.collision ? "\033[31mOui" : "\033[32mNon"); // Oui in red and Non in green
            printf("Lumière : %d mV\n", pilotState.luminosity);
        }
    }
}
