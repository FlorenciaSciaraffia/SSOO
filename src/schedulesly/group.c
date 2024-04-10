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

void report_processes(Process* process, FILE* output_file) {
    if (process == NULL) return;
    

    if (process->state == RUNNING || process->state == READY || process->state == WAITING) {
        printf("PROGRAM %d %d %d %s %d\n",
               process->pid, process-> ppid, process->pid, state_to_string(process->state), process->time_in_cpu);
        fprintf(output_file, "PROGRAM %d %d %d %s %d\n",
               process->pid, process-> ppid, process->pid, state_to_string(process->state), process->time_in_cpu);
               
    }
    // Recorrer y reportar recursivamente cada hijo
    for (int i = 0; i < process->nh; i++) {
        report_processes(process->children[i], output_file);
    }
}

void collect_finished_processes(Process* process, Process** finished_processes, int* n_finished) {
    if (process == NULL) return;

    if (process->state == FINISHED) {
        // Agregar el proceso actual al arreglo de procesos terminados
        finished_processes[*n_finished] = process;
        (*n_finished)++;
    }

    // Recorrer y reportar recursivamente cada hijo
    for (int i = 0; i < process->nh; i++) {
        collect_finished_processes(process->children[i], finished_processes, n_finished);
    }
}


//sort finished processes
void sort_finished_processes(Process* processes[], int count) {
    bool swapped;
    do {
        swapped = false;
        for (int i = 0; i < count - 1; i++) {
            if (processes[i]->time_in_cpu > processes[i + 1]->time_in_cpu) {
                Process* temp = processes[i];
                processes[i] = processes[i + 1];
                processes[i + 1] = temp;
                swapped = true;
            }
        }
    } while (swapped);
}

void report_processes_finished(Process* process, int depth, FILE* output_file) {

    //Tengo que encontrar todos los procesos terminados e imprimirlos en orden del time in cpu
    
    if (process == NULL) return;
    printf("EStoy entradando a imprimir los finished");
    
    if (process->state == FINISHED) {
        // Imprimir información del proceso actual
        //PROGRAM <PID> <PPID> <GID> <STATUS_PROCESO> <CPU_TOTAL_USADA>
        printf("PROGRAM %d %d %d %s %d\n",
               process->pid, 0, 0, state_to_string(process->state), process->time_in_cpu);

        fprintf(output_file, "PROGRAM %d %d %d %s %d\n",
               process->pid, 0, 0, state_to_string(process->state), process->time_in_cpu);
    }

    for (int i = 0; i < process->nh; i++) {
        report_processes_finished(process->children[i], depth + 1, output_file);
    }
}

