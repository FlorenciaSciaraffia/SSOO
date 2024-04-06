#include "process.h"
#include <signal.h>
#include "estadistica.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruccion.h"
#include <stdbool.h>
#include <math.h>

//hay que hacer variables globales para rastrear los PIDs
pid_t PIDs[100]; //Asumiendo un max de 100 procesos simultaneos
int hijosEnEjecucion = 0;

Estadistica estadisticas[100]; //Depende los hijos, modificable
int numEstadisticas = 0;

void manejadorTiempoMax(int signum){
    printf("Tiempo max alcanzado. Enviando SIGINT a todos los procesos hijos.\n");
    for (int i = 0; i < hijosEnEjecucion; ++i){
        if (PIDs[i] > 0){
            kill(PIDs[i], SIGINT);
        }
    }
    sleep(10); //espera 10 seg para dar tiempo a los procesos hijos de terminar
    //Verifica y envia SIGTERM a los procesos hijos que no hayan terminado

    for (int i = 0; i < hijosEnEjecucion; ++i){
        if (PIDs[i] > 0){
            int status;
            if (waitpid(PIDs[i], &status, WNOHANG) == 0){
                //el proceso aun existe
                printf("Proceso hijo %d no terminó. Enviando SIGTERM.\n", PIDs[i]);
                kill(PIDs[i], SIGTERM);

            }
        }
    }

}

void ejecutarComando(char **args, char* nombreEjecutable, int max_time){
    pid_t pid = fork();
   
    if (pid == 0){ //el proceso hijo ejecuta el comando
        // Prepara los argumentos para execvp. Recuerda que el primer argumento debe ser el nombre del programa
        if (execvp(nombreEjecutable, args) == -1) {
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }
        
    } else if (pid > 0){ //proceso padre
        //time_t inicio, fin;
        PIDs[hijosEnEjecucion++] = pid; //se agrega el PID del hijo al arreglo
        //printf("hijos actuales 3 %d\n", hijosEnEjecucion);
        time_t inicio = time(NULL);
        estadisticas[numEstadisticas].pid = pid;
        estadisticas[numEstadisticas].nombreEjecutable = strdup(nombreEjecutable); // Copiar el nombre para uso futuro
        estadisticas[numEstadisticas].tiempoInicio = inicio; // Registrar tiempo de inicio
        numEstadisticas++;



    } else{
        perror("Fallo en fork");
        exit(EXIT_FAILURE);
    }
}

void recolectarEstadisticas(pid_t pid, int status, time_t tiempoFin) {
    //printf("aqui en recolectar estadisticas\n");
    for (int i = 0; i < numEstadisticas; i++) {
        if (estadisticas[i].pid == pid) {
            estadisticas[i].tiempoEjecucion = difftime(tiempoFin, estadisticas[i].tiempoInicio);

            if (WIFSIGNALED(status)) {
                estadisticas[i].estado = WTERMSIG(status);
            } else if (WIFEXITED(status)) {
                estadisticas[i].estado = WEXITSTATUS(status);
            } else {
                estadisticas[i].estado = -1; // Indica un error o terminación anormal
            }
            break;// Salir del bucle una vez encontrado el pid correspondiente
        }
    }
}

void revisarEstadisticasNoBloqueante() {
    int status;
    pid_t pidTerminado = waitpid(-1, &status, WNOHANG);
    if (pidTerminado > 0) {
        recolectarEstadisticas(pidTerminado, status, time(NULL));
        hijosEnEjecucion--;
    }
}


// void revisarEstadisticasNoBloqueante() {
//     int status;
//     pid_t pidTerminado;
//     while ((pidTerminado = waitpid(-1, &status, WNOHANG)) > 0) {
//         recolectarEstadisticas(pidTerminado, status, time(NULL));
//         for (int i = 0; i < hijosEnEjecucion; i++) {
//             if (PIDs[i] == pidTerminado) {
//                 PIDs[i] = PIDs[hijosEnEjecucion - 1]; // Reemplaza el PID terminado con el último activo
//                 hijosEnEjecucion--; // Decrementa el contador de procesos activos
//                 break;
//             }
//         }
//     }
// }


void revisarEstadisticasBloqueante() {
    int status;
    pid_t pidTerminado;

    if (hijosEnEjecucion > 0) {
        // Espera bloqueante por la terminación de cualquier proceso hijo
        while ((pidTerminado = waitpid(-1, &status, 0)) > 0) {
            recolectarEstadisticas(pidTerminado, status, time(NULL));
            hijosEnEjecucion--;
        }
    }
}

void manejarWaitAll(int timeout) {
    time_t tiempo_inicio = time(NULL);
    printf("aqui WAIT ALL\n");

    // Esperar hasta que todos los procesos terminen o hasta que se alcance el timeout.
    while (hijosEnEjecucion > 0 && difftime(time(NULL), tiempo_inicio) <= timeout) {
        // Intentar recolectar procesos que hayan terminado.
        int status;
        printf("aqui en el while\n");
        pid_t pidTerminado;
        while ((pidTerminado = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("aqui en el while 2\n");
            //recolectarEstadisticas(pidTerminado, WIFEXITED(status) ? WEXITSTATUS(status) : -1, time(NULL));
            hijosEnEjecucion--;
        }
    }

    // Si aún hay procesos corriendo después del timeout, enviar SIGKILL.
    if (hijosEnEjecucion > 0) {
        printf("Timeout alcanzado. Enviando SIGKILL a procesos restantes.\n");
        for (int i = 0; i < hijosEnEjecucion; i++) {
            if (PIDs[i] > 0) {
                kill(PIDs[i], SIGKILL);
            }
        }
    }

    // Esperar a que todos los procesos terminados por SIGKILL sean recolectados.
    int status;
    pid_t pidTerminado;
    while (hijosEnEjecucion > 0 && (pidTerminado = waitpid(-1, &status, 0)) > 0) {
        recolectarEstadisticas(pidTerminado, 9, time(NULL));  // Estado 9 para SIGKILL.
        hijosEnEjecucion--;
        // Eliminar el PID del arreglo PIDs aquí.
    }
}




void ejecutarComandos(InputFile *input_file, const char* output_path, int amount, int max_time, Instruccion *instrucciones){
 
    hijosEnEjecucion = 0; //contador de los procesos hijos activos
    memset(PIDs, 0, sizeof(PIDs)); //inicializarlo con 0`s
    numEstadisticas = 0; //reinicia contador

    int i = 0;

    while (true){
        revisarEstadisticasNoBloqueante();

        while (hijosEnEjecucion >= amount && i < input_file->len) {
            revisarEstadisticasBloqueante();
        }

        if (i < input_file->len && hijosEnEjecucion < amount){
            if (instrucciones[i].numArgumentos != -1){ 
                ejecutarComando(instrucciones[i].args, instrucciones[i].nombreEjecutable,  max_time);
                i++;
            } else{
                manejarWaitAll(instrucciones[i].timeout);
                i++;
            }
        }
        
        if (i >= input_file->len && hijosEnEjecucion == 0) {
            break;
        }
    }
    revisarEstadisticasBloqueante();
    
}
