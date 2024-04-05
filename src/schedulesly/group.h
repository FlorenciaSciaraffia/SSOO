#pragma once
#include "process.h"
//I have to do the struct group thar contains proccesses , each group has procceses that have a father and childs, the childs can also have childs.
//I have to do the functions that are in the group.h file
//Each groups needs to have a group id, time of arrival and unit of works (q) (CPU time , burst)

//Struct of the group add qstart , qmin, q delta
typedef struct {
    int gid;
    int time_arrival;
    int qstart;
    int qmin;
    int qdelta;
    //PADRE que es un proceso
    Process* father;
} Group;



//create_group function
Group* create_group(int time_arrival, int qstart, int qmin, int qdelta);

