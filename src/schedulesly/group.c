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

    return new_group;
}