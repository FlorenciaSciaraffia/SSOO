//Lista ligada de grupos 
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "group.h"


//Estructura del nodo de la lista ligada
typedef struct GroupNode {
    Group* group;
    struct GroupNode* next;
} GroupNode;

//funcion incializa la lista 
GroupNode* init_group_list();

//Agragar grupos a la lista
void add_group(GroupNode* head, Group* group);

//Borrar grupo de la lista
void delete_group(GroupNode* head, int gid);
//Sort
void sort_group_list(GroupNode* head);

//Assing gid
void assign_gid(GroupNode* current);