/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/main.c to edit this template
 */

/* 
 * File:   main.c
 * Author: tcorbillon
 *
 * Created on 6 octobre 2022, 09:07
 */

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
/*
 * 
 */
#define NB_ENABLE 0
#define NB_DISABLE 1
#define NBLETTRES 26
#define TAILLEMAX 250

FILE *fic;

void traitement(int sig) {
/****************************************************************/
    (void) signal(SIGTERM, traitement);
/****************************************************************/
    printf("signal SIGTERM dans le process ayant pour pid : %d \n", getpid());
    if (fic != NULL) {
        fclose(fic);
        fic == NULL;
    }

    // envoyer le signal SIGINT au processus courant
/****************************************************************/
    kill(sig, SIGINT);
/****************************************************************/
   



}

int kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state) {
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state == NB_ENABLE) {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    if (state == NB_DISABLE) {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

}

char *transpose(char c) {
    char *retour = NULL;
     int indice;
    char *correspondance[NBLETTRES] = {
        "dead ",
        "ca ",
        "djaja ",
        "yo ",
        "catin ",
        "que ",
        "pasa ",
        "catchana ",
        "baby ",
        "crache ",
        "nakamura ",
        "pas ",
        "moyen ",
        "oh ",
        "genre ",
        "yeah ",
        "ouais ",
        "moliere ",
        "damned ",
        "diantre ",
        "fichtre ",
        "bigre ",
        "palsambleu ",
        "sacrebleu ",
        "sapristi ",
        "mazette "
    };
   
    if (c >= 'A' && c <= 'Z') {
        c = c + 'a' - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        indice = c - 'a';
        retour = (char *) malloc(strlen(correspondance[indice]) + 1);
        strcpy(retour, correspondance[indice]);
    } else {
        retour = (char *) malloc(2);
        strcpy(retour, ",");
    }

    return retour;


}

void crypteur(int fdLectureClavier, int fdEnregistreur) {
    char c;
    char *chaine;
    while (1 == 1) {
        // lecture du caractere present dans le tube en provenance de P1
        // et sauvegarde dans la variable c
/****************************************************************/
        read(fdLectureClavier, c, strlen(c));
/****************************************************************/
        // génération de la chaine de caractère correspondant à la lettre
        chaine = transpose(c);
        // ecriture de la chaine de caractere dans le tube vers P3
/****************************************************************/
        write(fdEnregistreur, &chaine, strlen(chaine));
/****************************************************************/
    }

}

void sauvegarde(int fd) {
    char buffer[TAILLEMAX];
    fic = fopen("cryptofile.txt", "a");

    if (fic == NULL) {
        printf("pb ouverture/creation fichier : %s\n", strerror(errno));
        exit(errno);
    }
    while (1 == 1) {
        memset(buffer, '\0', TAILLEMAX);
        // lecture de la chaine de caractere presente dans le tube en provenance de P2
        // et sauvegarde de la chaine dans la variable buffer
/****************************************************************/
        read(fd, buffer, TAILLEMAX);
/****************************************************************/
        //sauvegarde de la chaine dans le fichier cryptofile.txt
        fwrite(buffer, strlen(buffer), 1, fic);
    }

}

void saisie(int fd, int pidP2, int pidP3) {
    char c;
    int i = 0;
    int choix;
    do {
        nonblock(NB_ENABLE);

        printf("Saisissez la phrase à crypter (finir par .) :\n");
        do {
            c = getchar_unlocked();
            //ecrire c dans le tube vers P2
/****************************************************************/
            write(fd, c, strlen(c));
/****************************************************************/

        } while (c != '.');

        nonblock(NB_ENABLE);
        printf("\nQue souhaitez vous faire ?");
        printf("1 - Continuer\n");
        printf("2 ou autre - Quitter\n");
        scanf("%d", &choix);
    } while (choix == 1);
    // envoyer le signal SIGTERM vers P3
    
/****************************************************************/
    kill(pidP3, SIGTERM);
/****************************************************************/
    sleep(1);
    // envoyer le signal SIGINT vers P2
/****************************************************************/
    kill(pidP2, SIGINT);
/****************************************************************/

}

int main(int argc, char** argv) {
    
/****************************************************************/
    printf("test\n");
    int pid, pidP1, pidP2, pidP3;
    int tube[2];
    int nbOctets;
    (void) signal(SIGTERM, traitement);
    
    // P1
    pidP1 = getpid();
    if (pipe(tube) == 0) {
        pid = fork();
        if (pid == 0) // P2
        {
            pidP2 = getpid();
            pid = fork();
            if (pid == 0) // P3
            {
               sauvegarde(int tube[2]);
                

            } else // P2
            {
              crypteur(int tube[0], int tube[1]);

            }

        } else // P1
        {
           saisie(int pidP2, int pidP3);

        }
    }
    
/****************************************************************/
    return (EXIT_SUCCESS);
}
