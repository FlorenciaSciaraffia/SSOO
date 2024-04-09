//I have to do the struct group thar contains proccesses , each group has procceses that have a father and childs, the childs can also have childs.
//I have to do the functions that are in the group.h file

#include "group.h"
#include <stdlib.h> // Necesario para malloc
#include <stdio.h> // Necesario para printf

//create group build it correctly with all the parameters

Group* create_group( int time_arrival, int qstart, int qdelta, int qmin) {
    Group* new_group = (Group*)malloc(sizeof(Group)); // Reservamos memoria para el nuevo grupo
    if (new_group == NULL) {
        // Manejo de error, en caso de que malloc falle
        return NULL;
    }

    new_group->time_arrival = time_arrival;
    new_group->qstart = qstart;
    new_group->qdelta = qdelta;
    new_group->qmin = qmin;
    new_group->father = NULL; //inicializamos el padre en NULL no lo tiene asgnado aún
    new_group->cantidad_procesos = 0;

    return new_group;
}

//contar procesos
void count_processes(Group* group) {
    //busco el padre
    Process* father = group->father;
    //Si el padre no es nulo
    group->cantidad_procesos = 1;
    if (father != NULL) {
        //si el padre tiene hijos
        if (father->nh > 0) {
            //recorro los hijos
            for (int i = 0; i < father->nh; i++) {
                //si el hijo no es nulo
                if (father->children[i] != NULL) {
                    //incremento la cantidad de procesos
                    group->cantidad_procesos++;
                }
            }
        }
    }
}
// Función auxiliar para convertir el estado a string
const char* state_to_string(State state) {
    switch(state) {
        case RUNNING: return "RUNNING";
        case READY: return "READY";
        case WAITING: return "WAITING";
        case FINISHED: return "FINISHED";
        default: return "UNKNOWN";
    }
}

//report processes

void report_processes_not_finished(Process* process, int depth) {
    if (process == NULL) return;
    
    
    //Reporto solol los que no han terminado
    if (process->state != FINISHED) {
        // Imprimir espacios para indentar según el nivel en la jerarquía
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
        
        //PROGRAM <PID> <PPID> <GID> <STATUS_PROCESO> <CPU_TOTAL_USADA>
        printf("PROGRAM %d %d %d %s %d\n",
               process->pid, process-> ppid, process->gid, state_to_string(process->state), process->time_in_cpu);
    }
    // Recorrer y reportar recursivamente cada hijo
    for (int i = 0; i < process->nh; i++) {
        report_processes_not_finished(process->children[i], depth + 1);
    }
}


void report_processes_finished(Process* process, int depth) {
    if (process == NULL) return;
    
    if (process->state == FINISHED) {
        // Imprimir espacios para indentar según el nivel en la jerarquía
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
        
        // Imprimir información del proceso actual
        //PROGRAM <PID> <PPID> <GID> <STATUS_PROCESO> <CPU_TOTAL_USADA>
        printf("PROGRAM %d %d %d %s %d\n",
               process->pid, 0, 0, state_to_string(process->state), process->time_in_cpu);
    }
}

