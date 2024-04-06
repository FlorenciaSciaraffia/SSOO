#ifndef INSTRUCCION_H
#define INSTRUCCION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
    int numArgumentos;
    char* nombreEjecutable;
    char** args;
    int timeout;
} Instruccion;



#endif

