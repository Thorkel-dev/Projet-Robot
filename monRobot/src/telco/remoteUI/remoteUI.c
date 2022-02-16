#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "remoteUI.h"
#include "../../common.h"
#include "client/client.h"

/**
 * @brief La touche permettant de faire tourner à gauche le robot.
 */
#define LEFT_KEY 'q'

/**
 * @brief La touche permettant de faire tourner à droite le robot.
 */
#define RIGHT_KEY 'd'

/**
 * @brief La touche permettant de faire reculer le robot.
 */
#define FORWARD_KEY 'z'

/**
 * @brief La touche permettant de faire avancer le robot.
 */
#define BACK_KEY 's'

/**
 * @brief La touche permettant de stopper le robot.
 */
#define STOP_KEY ' '

/**
 * @brief La touche permettant de nettoyer le terminal.
 */
#define ERASE_LOG_KEY 'e'

/**
 * @brief La touche permettant d'afficher l'état du robot.
 */
#define DISPLAY_STATE_KEY 'r'

/**
 * @brief La touche permettant de quitter, équivalent à CTRL+C.
 */
#define QUIT_KEY 'a'

static void capturechoise(char p_char);
static void askMvt(Direction_e p_dir);
static void ask4Log();
static void askClearLog();
static void eraseLog();
static void quit();
static void run();
static void display();

static bool_e works;
static int s_socketClient;

extern void RemoteUI_new()
{
    Client_new();
}

extern void RemoteUI_start()
{
    s_socketClient = *Client_start();
    works = TRUE;
    run();
}

extern void RemoteUI_stop()
{
    works = FALSE;
}

static void askMvt(Direction_e p_dir)
{
    TRACE("Some Moves - Direction is %d\n", p_dir);

    Data_s l_data;
    l_data.direction = p_dir;
    l_data.order = O_CHANGE_MVT;
    Client_sendMsg(l_data);
}

static void ask4Log()
{
    askClearLog();

    printf("Speed calculation, please wait ...\n");

    Data_s l_data;
    l_data.order = O_ASK_LOG;
    Client_sendMsg(l_data);
}

static void askClearLog()
{
    eraseLog();
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

static void eraseLog()
{
    printf("\033c");
    display();
}

static void quit()
{
    printf("\n%sAsk for exit%s\n", "\033[31m", "\033[0m");
    RemoteUI_stop();
}

static void capturechoise(char p_char)
{
    switch (p_char)
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
        quit();
        break;
    default:
        break;
    }
}

static void run()
{

    if (works == TRUE)
    {
        askClearLog();
    }

    fd_set l_env;
    FD_ZERO(&l_env);
    FD_SET(STDIN_FILENO, &l_env);
    FD_SET(s_socketClient, &l_env);

    while (works == TRUE)
    {
        struct termios oldt, newt;

        // Ecrit les paramètres de stdin sur old
        tcgetattr(STDIN_FILENO, &oldt);

        newt = oldt;

        newt.c_lflag &= ~(ICANON | ECHO); // Fait les flags de new comparé à l'opposé de ICANON et ECHO

        // Change les attributs immédiatement
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        if (select(FD_SETSIZE, &l_env, NULL, NULL, NULL) == -1)
        {
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            RemoteUI_stop();
            break;
        }
        // On remet les anciens paramètres
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        if (FD_ISSET(STDIN_FILENO, &l_env))
        {
            char l_charInput[2]; // Size 2 for the null character

            if (read(STDIN_FILENO, l_charInput, sizeof(l_charInput)) < 0)
            {
                printf("%sError when readind the input%s\n", "\033[41m", "\033[0m");
            }
            else
            {
                capturechoise(l_charInput[0]);
            }
        }
        else if (FD_ISSET(s_socketClient, &l_env))
        {
            const Data_s pilotState = Client_readMsg();

            printf("\033[1A\033[K"); // Positionnement du curseur 1 ligne au-dessus et effacement de celle ci
            printf("\nVitesse du robot : %d cm/s\n", pilotState.speed);
            printf("Collision : %s\033[0m\n", pilotState.collision ? "\033[31mOui" : "\033[32mNon"); // Oui en rouge et Non en vert
            printf("Lumière : %d mV\n", pilotState.luminosity);
        }
    }
}
