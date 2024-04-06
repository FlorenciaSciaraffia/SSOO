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
    //Array con los ce que es el tiempo que correrá entre creación de hijos solo si es padre máximo y tiene el tamaño de nh -1 
    int* ce;
    int cf; //Tiempo que correra despues de crear todos sus hijos
    //Cada proceso tiene un array que tiene punteros a todos sus hijos y cm es array es del tamaño de nh 
    //ARRAY DE PUNTEROS A PROCESOS NO UNA LISTA LIGADA
    struct Process** children;
} Process;

//Función que crea un proceso create_process
//el pid, ppig y gid los agrego despues
Process* create_process( int ci, int nh, int cf, bool is_father_max);

void assign_gid_recursively(Process* process, int gid);