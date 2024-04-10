#pragma once //corrrerlo una sola vez
#include <stdbool.h>


typedef enum {
    RUNNING,
    READY,
    WAITING,
    FINISHED, 
    NOT_STARTED
} State;

//Estructura del proceso
typedef struct Process {
    int pid;
    int ppid;
    int gid;
    State state;
    int ti; //Tiempo de llegada a la cola
    int ci; //Tiempo de CPU antes de crear un hijo
    int nh; //Numero de hijos que creara
    bool is_father_max; //Si es el padre máximo
    int* ce; //tiempo que corre entre sus hijos
    int cf;  //tiempo que corre despues dde crear y correr todos los hijos
    int cf_original;
    int time_in_cpu;
    int line_ci;
    int arg_ci;
    int time_finished;
    struct Process** children;
} Process;

//Función que crea un proceso create_process
//el pid, ppig y gid los agrego despues
Process* create_process( int ci, int nh, int cf, bool is_father_max, int line_ci, int arg_ci);

void assign_gid_recursively(Process* process, int gid);