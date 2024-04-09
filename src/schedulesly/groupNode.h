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

//Sort
void sort_group_list(GroupNode* head);

//Assing gid
void assign_gid(GroupNode* current);

//Add group to end
void add_group_to_end(GroupNode* head, Group* group);

//move first
void move_first_pending_to_active_end(GroupNode* pending, GroupNode* active);

//check pending groups
void check_and_move_groups(GroupNode* pending, GroupNode* active, int currentTime);