#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "../file_manager/manager.h"
#include "instruccion.h"

void ejecutarComandos(InputFile *input_file, const char* output_path, int amount, int max_time, Instruccion *instrucciones);


#endif

