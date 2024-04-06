#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "process.h"
#include "instruccion.h"
#include "senales.h"
#include "writer.h"


int main(int argc, char const *argv[])
{

	if (argc < 4 || argc > 5) {
        fprintf(stderr, "Uso correcto: %s <input_path> <output_path> <amount> [<max_time>]\n", argv[0]);
        return 1;
    }

	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);
  
	const char* output_path = argv[2];
	int amount = atoi(argv[3]);
	int max = (argc == 5) ? atoi(argv[4]) : -1;


	Instruccion* instrucciones = malloc(sizeof(Instruccion)* (input_file->len));

	/*Mostramos el archivo de input en consola*/
	printf("Cantidad de lineas: %d\n", input_file->len);

	for (int i = 0; i < input_file->len; ++i)
	{
		
		int argc = atoi(input_file->lines[i][0]);
		printf("%d ", atoi(input_file->lines[i][0]));
		printf("%s ", input_file->lines[i][1]);
		for (int j = 2; j < argc + 2; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}
		
		printf("\n");
	}

	for (int i = 0; i < input_file->len; ++i)
	{
		int numArgumentos = atoi(input_file->lines[i][0]);
		char* nombreEjecutable = input_file->lines[i][1];
		instrucciones[i].numArgumentos = numArgumentos;
		instrucciones[i].nombreEjecutable = strdup(nombreEjecutable);

		if (numArgumentos == -1){ //wait all
			instrucciones[i].timeout = atoi(input_file->lines[i][2]);
			instrucciones[i].args = NULL;
		} else{
			instrucciones[i].args = malloc(sizeof(char*)*(numArgumentos + 1));
			for (int j = 0; j < numArgumentos; ++j){
				instrucciones[i].args[j] = strdup(input_file->lines[i][j + 2]);
			}
			instrucciones[i].args[numArgumentos] = NULL;
		}

		if (instrucciones[i].numArgumentos == -1) {
            printf("wait all %d\n", instrucciones[i].timeout);
        } else {
            for (int j = 0; j < numArgumentos + 1; ++j) {
                printf("%s ", instrucciones[i].args[j]);
            }
            printf("\n");
        }
	}

	ejecutarComandos(input_file, output_path, amount, max, instrucciones);
	printf("aquiii");


	manejarSenales();
	escribirStatus(output_path);


	//free instrucciones
	for (int i = 0; i < input_file->len; ++i) {
        if (instrucciones[i].numArgumentos != -1) {
            for (int j = 0; j < instrucciones[i].numArgumentos + 1; ++j) {
                free(instrucciones[i].args[j]);
            }
            free(instrucciones[i].args);
        }
    }
    free(instrucciones);

	input_file_destroy(input_file);
}