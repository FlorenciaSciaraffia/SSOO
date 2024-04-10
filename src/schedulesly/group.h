#pragma once
#include "process.h"
#include <stdbool.h>
#include <stdio.h>
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
    int cantidad_procesos;
    Process* father;
} Group;



//create_group function
Group* create_group(int time_arrival, int qstart, int qmin, int qdelta);

//contar procesos
void count_processes(Group* group);

//Report processes
void report_processes(Process* process, FILE* output_file);

//COllect finished processes

void collect_finished_processes(Process* process, Process** finished_processes, int* n_finished) ;

//Report processes findihes
void report_processes_finished(Process* process, int depth, FILE* output_file);


void sort_finished_processes(Process* processes[], int count) ;