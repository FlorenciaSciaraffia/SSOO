#include "senales.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


extern pid_t PIDs[];
extern int hijosEnEjecucion;

void manejarSIGTSTP(int signum) {
    //envía SIGSTOP a todos los procesos hijos para pausarlos inmediatamente

    for (int i = 0; i < hijosEnEjecucion; ++i){
        if (PIDs[i]>0){
            kill(PIDs[i], SIGSTOP); //enviar señal a cada proceso hijo activo
        }
    }
    printf("Manejador SIGTSTP: Todos los procesos hijos han recibido SIGTSTP.\n");

    sleep(10);
    //Verifica y envia SIGTERM a los procesos que no han terminado
    for (int i = 0; i < hijosEnEjecucion; ++i){
        int status;
        pid_t result = waitpid(PIDs[i], &status, WNOHANG);
        if(result == 0){ //proceso hijo no ha terminado
            kill(PIDs[i], SIGTERM);
            printf("SIGTERM enviado al proceso hijo PID=%d\n", PIDs[i]);
        }
    }
    
}

void manejarSIGINT(int signum) {
    for (int i = 0; i < hijosEnEjecucion; ++i){
        if (PIDs[i]>0){
            kill(PIDs[i], SIGINT);
        }
    }
    printf("Manejador SIGINT: Todos los procesos hijos han recibido SIGINT.\n");
}

void manejarSenales() {
    struct sigaction sa;

    sa.sa_handler = manejarSIGINT;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = manejarSIGTSTP;
    sigaction(SIGTSTP, &sa, NULL);
}
