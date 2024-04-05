#ifndef PRINTING_H
#define PRINTING_H

#include "process.h"
#include "groupNode.h"
#include "group.h"   

// Declaración de las funciones
void print_ce_values(Process* process);
void printProcessStructure(Process* process, int level);
void printAllGroups(GroupNode* groupList);

#endif