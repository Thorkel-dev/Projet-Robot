/**
 * @file adminui.c
 *
 * @see adminui.h
 *
 * @author Gautier Edouard
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "adminui.h"

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

/**
 * @brief Vitesse par défaut du robot
 */
#define POWER 100
/**
 * @brief Affiche les touches de la télécommande
 */
static void display(void);

/**
 * @brief Récupère la touche entrée par l'utilisateur
 */
static void captureChoice(void);

/**
 * @brief Convertie de la direction choisie par l'utilisateur en un vecteur vitesse
 *
 * @param direction la direction dans laquelle le robot doit aller.
 * @return VelocityVector le vector vitesse correspond à la direction où aller.
 */
static VelocityVector translate(const Direction direction);

/**
 * @brief Transmet la direction donnée à Pilot.
 *
 * @param direction la direction demandée.
 */
static void askMvt(const Direction direction);

/**
 * @brief Affiche l'état du robot
 */
static void ask4Log(void);

/**
 * @brief Efface le texte du terminal
 */
static void askClearLog(void);

/**
 * @brief Sort du programme
 */
static void quit(void);

extern void AdminUI_new()
{
    Pilot_new();
}

extern void AdminUI_free()
{
    Pilot_free();
}

extern void AdminUI_start()
{
    Pilot_start();
    display();
    captureChoice();
}

extern void AdminUI_stop()
{
    Pilot_stop();
}

static void display()
{
    printf("\033c");
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

static void captureChoice()
{
    int caractere = ERASE_LOG_KEY;
    do
    {
        struct termios oldt, newt;

        // Ecrit les paramètres de stdin sur old
        tcgetattr(STDIN_FILENO, &oldt);

        newt = oldt;

        newt.c_lflag &= ~(ICANON | ECHO); // Fait les flags de new comparé à l'opposé de ICANON et ECHO

        // Change les attributs immédiatement
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        caractere = getchar();

        // On remet les anciens paramètres
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        switch (caractere)
        {
        case LEFT_KEY:
            askMvt(LEFT);
            break;
        case RIGHT_KEY:
            askMvt(RIGHT);
            break;
        case FORWARD_KEY:
            askMvt(FORWARD);
            break;
        case BACK_KEY:
            askMvt(BACKWARD);
            break;
        case STOP_KEY:
            askMvt(STOP);
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
    } while (caractere != QUIT_KEY);
}

VelocityVector translate(const Direction direction)
{
    VelocityVector velocityVector = {direction, POWER};
    return velocityVector;
}

static void askMvt(const Direction direction)
{
    Pilot_setVelocity(translate(direction));
}

static void ask4Log()
{
    askClearLog();

    const PilotState pilotState = Pilot_getState();
    printf("\033[1A\033[K"); // Positionnement du curseur 1 ligne au-dessus et effacement de celle ci
    printf("\nVitesse du robot : %d cm/s\n", pilotState.speed);
    printf("Collision : %s\033[0m\n", pilotState.collision ? "\033[31mOui" : "\033[32mNon"); // Oui en rouge et Non en vert
    printf("Lumière : %d mV\n", pilotState.luminosity);
}

static void askClearLog()
{
    printf("\033[2J\033[;H");
    display();
}

static void quit()
{
    Pilot_stop();
}