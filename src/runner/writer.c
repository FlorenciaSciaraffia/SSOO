#include "writer.h"
#include <stdio.h>
#include "estadistica.h"

extern Estadistica estadisticas[100];
extern int numEstadisticas;

void escribirStatus(const char *outputPath){
    FILE *file = fopen(outputPath, "w");
    if (!file){
        perror("No se pudo abrir output file");
        return;
    }

    for (int i = 0; i < numEstadisticas; ++i){
        printf("Escribiendo: %.0f, %d\n",  estadisticas[i].tiempoEjecucion, estadisticas[i].estado); 
        fprintf(file, "%s,%.0f,%d\n", estadisticas[i].nombreEjecutable, estadisticas[i].tiempoEjecucion, estadisticas[i].estado);
    }

    fclose(file);
}