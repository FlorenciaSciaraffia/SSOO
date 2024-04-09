#pragma once //corrrerlo una sola vez
#include <stdbool.h>


typedef enum {
    RUNNING,
    READY,
    WAITING,
    FINISHED
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
    struct Process** children;
} Process;

//Función que crea un proceso create_process
//el pid, ppig y gid los agrego despues
Process* create_process( int ci, int nh, int cf, bool is_father_max);

void assign_gid_recursively(Process* process, int gid);