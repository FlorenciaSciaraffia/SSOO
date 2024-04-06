#ifndef ESTADISTICA_H
#define ESTADISTICA_H

#include <stdio.h>
#include <sys/types.h>

typedef struct{
    char* nombreEjecutable;
    double tiempoEjecucion;
    time_t tiempoInicio;
    int estado;
    pid_t pid;
} Estadistica;

#endif
