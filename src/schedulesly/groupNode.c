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

