#include "process.h"
#include <stdlib.h> // Necesario para malloc
#include <stdio.h> // Necesario para printf

//create process build it correctly with all the parameters
// Process* create_process(int ti, int ci, int nh, int ce, int cf);

Process* create_process(int ci, int nh, int cf, bool is_father_max) {
    Process* new_process = (Process*)malloc(sizeof(Process)); // Reservamos memoria para el nuevo proceso
    if (new_process == NULL) {
        // Manejo de error, en caso de que malloc falle
        return NULL;
    }
    new_process->ci = ci;
    new_process->nh = nh;
    new_process->is_father_max = is_father_max;
    //construir array ce que es del tamaño nh - 1 
    //if is father max la lista es de nh-1 si no el atributo ce va a ser una lista vacia
    if (is_father_max && nh > 0) {
        new_process->ce = malloc(sizeof(int) * (nh - 1));
        for (int i = 0; i < nh - 1; i++) {
            new_process->ce[i] = 0; //inicializamos la lista en NULL
        }
    } else {
        new_process->ce = NULL; //no tiene hijos
    }

    new_process->cf = cf;
    if (nh >0)  {
        new_process->children = malloc(sizeof(Process*) * nh);
        for (int i = 0; i < nh; i++) {
            new_process->children[i] = NULL; //inicializamos los hijos en NULL
        }
    } else {
        new_process->children = NULL; //no tiene hijos
    }
    return new_process;
}

void assign_gid_recursively(Process* process, int gid) {
    if (process == NULL) return; // Caso base: no hay proceso

    process->gid = gid; // Asigna el gid al proceso actual

    
    for (int i = 0; i < process->nh; i++) {
        assign_gid_recursively(process->children[i], gid);
    }
}


