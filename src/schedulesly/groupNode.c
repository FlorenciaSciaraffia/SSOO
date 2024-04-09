#include "groupNode.h"
#include <stdlib.h> // Necesario para malloc
#include <stdio.h> // Necesario para printf



//Init group node
GroupNode* init_group_list() {
    GroupNode* head = (GroupNode*)malloc(sizeof(GroupNode)); // Reservamos memoria para el nodo de grupo inicial
    if (head == NULL) {
        // Manejo de error, en caso de que malloc falle
        printf("Error al crear el nodo de grupo\n");
        return NULL;
    }
    head->group = NULL;
    head->next = NULL;
    return head;
}


//Add group considera que ya tendré la lista incializada
void add_group(GroupNode* head, Group* group) {
    GroupNode* new_node = (GroupNode*)malloc(sizeof(GroupNode)); // Reservamos memoria para el nuevo nodo de grupo
    if (new_node == NULL) {
        // Manejo de error, en caso de que malloc falle
        printf("Error al crear el nodo de grupo\n");
        return;
    }
    new_node->group = group;
    new_node->next = NULL;

    //Buscar el último nodo
    GroupNode* current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    //Asignar el nuevo nodo al final
    current->next = new_node;
}

//Sort the groups by arriving time
void sort_group_list(GroupNode* head) {
    GroupNode* current = head->next; // Si tienes un nodo cabeza ficticio, empieza con el siguiente
    while (current != NULL) {
        GroupNode* next = current->next;
        while (next != NULL) {
            if (current->group->time_arrival > next->group->time_arrival) {
                // Intercambiar los grupos
                Group* temp = current->group;
                current->group = next->group;
                next->group = temp;
            }
            next = next->next;
        }
        current = current->next;
    }
}

//Asign gid
void assign_gid(GroupNode* current) {
    int gid = 1;
    while (current != NULL) {
        assign_gid_recursively(current->group->father, gid);
		current = current->next;
		gid++;
    }
}



void add_group_to_end(GroupNode* head, Group* group) {
    GroupNode* newNode = (GroupNode*)malloc(sizeof(GroupNode));
    newNode->group = group;
    newNode->next = NULL;

    if (head->next == NULL) { // Si la lista está vacía
        head->next = newNode;
    } else {
        GroupNode* current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void move_first_pending_to_active_end(GroupNode* pending, GroupNode* active) {
    if (!pending->next) {
        printf("No hay grupos pendientes para mover.\n");
        return;
    }

    // Encuentra el último nodo en la lista active
    GroupNode* lastActive = active;
    while (lastActive->next != NULL) {
        lastActive = lastActive->next;
    }

    // Mueve el primer grupo de pending al final de active
    GroupNode* firstPending = pending->next; // Primer grupo pendiente
    pending->next = firstPending->next; // Ajustar el inicio de pending al segundo grupo

    firstPending->next = NULL; // Este será el nuevo último nodo en active
    lastActive->next = firstPending; // Conecta el último nodo de active con el primero de pending
}


void check_and_move_groups(GroupNode* pending, GroupNode* active, int currentTime) {
    bool movedGroup;
    do {
        movedGroup = false; // Reiniciar el indicador para cada iteración.
        GroupNode* current = pending->next; // Comenzar con el primer grupo pendiente.
        while (current != NULL && current->group->time_arrival <= currentTime) {
            if (current->group->time_arrival == currentTime) {
                move_first_pending_to_active_end(pending, active);
                movedGroup = true; // Indicar que un grupo fue movido.
                printf("Grupo %d agregado a la lista de grupos activos\n", current->group->father->gid);
                break; // Salir del bucle para reevaluar desde el principio de pending.
            }
            current = current->next;
        }
    } while (movedGroup); // Continuar mientras se muevan grupos.
}
