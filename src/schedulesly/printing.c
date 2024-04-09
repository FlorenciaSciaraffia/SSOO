#include "printing.h"
#include <stdio.h>


void print_ce_values(Process* process) {
    if (process->is_father_max && process->nh > 1 && process->ce != NULL) {
        printf("Valores de CE para el proceso padre máximo (PID: %d) con %d hijos:\n", process->pid, process->nh);
        for (int i = 0; i < process->nh - 1; i++) {
            printf("%d ", process->ce[i]);
        }
        printf("\n");
    }
}



void printProcessStructure(Process* process, int level) {
    // Imprimir espacios para indentar según el nivel en la jerarquía
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Imprimir información del proceso actual
    printf("Proceso: PID: %d, PPID: %d, GID: %d, CI: %d, NH: %d, CF: %d\n", process->pid, process->ppid, process-> gid, process->ci, process->nh, process->cf);
    
    // Si el proceso es padre máximo y tiene hijos, imprimir CE
    if (process->is_father_max && process->nh > 1 && process->ce != NULL) {
        for (int i = 0; i < process->nh - 1; i++) {
            for (int j = 0; j < level + 1; j++) {
                printf("  ");
            }
            printf("CE entre hijo %d y %d: %d\n", i+1, i+2, process->ce[i]);
        }
    }

    // Llamar recursivamente para cada hijo
    for (int i = 0; i < process->nh; i++) {
        printProcessStructure(process->children[i], level + 1);
    }
}


void printAllGroups(GroupNode* groupList) {
    GroupNode* current = groupList->next; // Asumiendo un nodo cabeza ficticio
    while (current != NULL) {
        printf("Grupo: TI: %d, QStart: %d, QDelta: %d, QMin: %d\n",
               current->group->time_arrival, current->group->qstart,
               current->group->qdelta, current->group->qmin);

        // Imprimir el proceso padre y su jerarquía de hijos
        if (current->group->father != NULL) {
            printProcessStructure(current->group->father, 0);
        } else {
            printf("  Este grupo no tiene un proceso padre.\n");
        }

        current = current->next;
    }
}
