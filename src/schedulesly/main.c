#include <stdio.h>	
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h> 
#include "../file_manager/manager.h"
#include "group.h"
#include "process.h"
#include "groupNode.h"
#include "printing.h"

int currentPID = 1;

void readChilds(FILE *file, Process* father, int nh, int line_ci, int arg){
	 for (int i = 0; i < nh; i++) {
		printf("PROCESO EN CREACION\n");
		if (i > 0)
		{
			printf("Es el padre maximo y no es el primer hijo\n");
			int ce;
			fscanf(file, "%d", &ce);
			printf("CE: %d\n", ce);
			arg++;
			father->ce[i - 1] = ce;
		}

		int child_ci, child_nh, child_cf;
		fscanf(file, "%d %d", &child_ci, &child_nh);
		printf("CI: %d - NH: %d\n", child_ci, child_nh);
		arg++;
		arg++;
		Process* new_process_child = create_process(child_ci, child_nh, 0, false, line_ci, arg);
		father->children[i] = new_process_child;
		printf("Proceso hijo creado: %d - %d - %ls - %d\n", new_process_child->ci, new_process_child->nh, new_process_child->ce, new_process_child->cf);

		if (child_nh == 0)
		{
			fscanf(file, "%d", &child_cf);
			printf("CF: %d\n", child_cf);
			new_process_child->cf = child_cf;
			arg++;
			printf("No tiene hijos el proceso hijo\n");
		}
		else
		{
			readChilds(file, new_process_child, child_nh, line_ci, arg); 
			fscanf(file, "%d", &child_cf);
			printf("CF: %d\n", child_cf);
			arg++;
			new_process_child->cf = child_cf;
		}
		printf("Proceso aactualizado: %d - %d - %ls - %d\n", new_process_child->ci, new_process_child->nh, new_process_child->ce, new_process_child->cf);		
	}
	return;
}

GroupNode* readFileAndCreatStructures(char *file_name){
	FILE *file = fopen(file_name, "r");
	if (file == NULL)
	{
		printf("Error al abrir el archivo\n");
		return NULL;
	}

	int K, qstart, qdelta, qmin;
	fscanf(file, "%d", &K);
	fscanf(file, "%d %d %d", &qstart, &qdelta, &qmin);
	printf("K líneas: %d\n", K);
	printf("qstart: %d - qdelta: %d - qmin: %d\n", qstart, qdelta, qmin);
	
	//crear e inicializar la lista ligada de grupos GroupNode 
	GroupNode* group_list_pending = init_group_list();
	int arg = 1;
	//LEctura resto de las k-1 lineas
	for (int i = 0; i < K -1 ; i++)
	{
		int ti, ci, nh;
		fscanf(file, "%d %d %d", &ti, &ci, &nh);
		//printf("TI: %d - CI: %d - NH: %d\n", ti, ci, nh);
		//crear el grupo
		Group* new_group = create_group(ti, qstart, qdelta, qmin);
		//crear el proceso padre máximo
		Process* new_process = create_process(ci, nh, 0, true, i+1, arg);
		//agregar el proceso al grupo
		new_group-> father = new_process;
		//agregar el grupo a la lista de grupos
		add_group(group_list_pending, new_group);
		//imprimir el grupo creado y el padre
		printf("Grupo creado: TI:%d - Qstart:%d - Qdelta:%d - QMin:%d\n", new_group->time_arrival, new_group->qstart, new_group->qdelta, new_group->qmin);
		printf("Proceso padre creado: CI:%d - NH:%d - CE:%ls - CF:%d\n", new_process->ci, new_process->nh, new_process->ce, new_process->cf);
		//lo primero es que si el nh del padre maximo es 0 el numero que viene va a ser el cf del padre maximo y no tendrá ni un hijo
		if (nh == 0)
		{ //NO hay hijos leo solo el input que viene y lo guardo como el cf del padre
			int cf;
			fscanf(file, "%d", &cf);
			//printf("CF: %d\n", cf);
			new_process->cf = cf;
			// termina el for 
			printf("No tiene hijos el proceso padre\n");
		}
		else
		{	
			printf("Tiene %d hijos el proceso padre\n", nh);
			//llamar a la función recursiva que lee los hijos
			readChilds(file, new_process, nh, i+1, arg);
			//Lee el cf despues de construir todos los hijos
			int cf;
			fscanf(file, "%d", &cf);
			//printf("CF: %d\n", cf);
			//lo asingo al padre
			new_process->cf = cf;
			//imprimo la info del padre
			printf("Proceso padre actualizado valor CF: CI:%d - NH:%d - CE:%ls - CF:%d\n", new_process->ci, new_process->nh, new_process->ce, new_process->cf);
		}


	GroupNode* current = group_list_pending->next; // Si tienes un nodo cabeza ficticio, empieza con el siguiente
    while (current != NULL) {
        if (current->group->father->is_father_max) {
            printf("Valores de CE para el grupo %d, proceso padre máximo:\n", current->group->gid);
            print_ce_values(current->group->father);
        }
        current = current->next;
    }
}
	return group_list_pending;
}


int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
	return (a > b) ? a : b;
}

int process_process(Process* process, int qstart, int parentPID, int tiempo, Group* group, FILE* output_file){
	int tiempo_proceso= 0;
	int tiempo_avanzado_total= 0;
	printf("qstart: %d\n", qstart);
	printf("Proceso a procesar:CI %d -NH %d -CF %d - PID  %d\n", process->ci, process->nh, process->cf, process->pid);
	//Procesamos el tiempo antes de crear los hijos, osea el ci
	//Si es la primera vez que se corre el ci tengo que asigan el pid y ppid
	if (process->pid == -1)
	{	
		//cambio estado a READY
		process->state = READY;
		printf("Proceespid: %d\n", process->pid);
		process->pid = currentPID;
		currentPID++;
		group->cantidad_procesos++;
		//si no hay padre, el ppid es 0
		if (process->is_father_max==true)
		{
			process->ppid = 0;
		}
		else
		{
			process->ppid = parentPID;
		}
		//Reportar que entra por primer vez a SO
		//ENTER <PID> <PPID> <GID> TIME <TIEMPO_ACTUAL_SO> LINE <NUM_LINEA> ARG <NUM_ARGUMENTOS>
		printf("ENTER %d %d %d TIME %d LINE %d ARG %d\n", process->pid, process->ppid, process->gid, tiempo, process->line_ci, process->arg_ci);
		fprintf(output_file, "ENTER %d %d %d TIME %d LINE %d ARG %d\n", process->pid, process->ppid, process->gid, tiempo+tiempo_proceso, process->line_ci, process->arg_ci);
	}
	
	int timeToProcess = min(qstart, process->ci);
	printf("Time to process: %d\n", timeToProcess);
	process->ci -= timeToProcess;
	tiempo_proceso += timeToProcess;
	tiempo_avanzado_total += timeToProcess;
	qstart -= timeToProcess;
	printf("Tiempo proceso: %d\n", tiempo_proceso);
	//Actulizar estado a running
	
	//Reporto el tiempo que corrio
	//RUN <PID> <TIEMPO_TRABAJADO>
	if (tiempo_proceso > 0)
	{
		printf("RUN %d %d\n", process->pid, tiempo_proceso);
		fprintf(output_file, "RUN %d %d\n", process->pid, tiempo_proceso);
		process->state = RUNNING;
	}
	//si quedan unidades de trabajo por trabajar y hay hijos por recorrer: 
	if (qstart > 0 && process->nh > 0)
	{
		printf("Proceso con hijos\n");
		printf("Proceso actualizacion como va:CI %d -NH %d -CF %d - PID  %d\n", process->ci, process->nh, process->cf, process->pid	);
		//Procesamos los hijos
		for (int i = 0; i < process->nh && qstart>0 ; i++)
		{	
			int tiempo_hijo = 0;
			//Procesamos cada hijo
			//Imprimo wait solo si el estado anterior era running
			if (process->state == RUNNING)
			{
				printf("WAIT %d\n", process->pid);
				fprintf(output_file, "WAIT %d\n", process->pid);
			}
			process->state = WAITING;
			if (i>0){
				//Procesamos el tiempo antes de crear los hijos, osea el ce
				int ceToProcess = min(qstart, process->ce[i-1]);
				process->ce[i-1] -= ceToProcess;
				tiempo_proceso += ceToProcess;
				tiempo_avanzado_total += ceToProcess;
				qstart -= ceToProcess;
				printf("CE to process: %d\n", ceToProcess);
			}
			//Aca debe correr si quedan unidades de qstart
			if (qstart > 0)
			{
				//Procesamos el hijo
				tiempo_hijo += process_process(process->children[i], qstart, process->pid, tiempo+tiempo_avanzado_total, group, output_file);
				qstart-= tiempo_hijo;
				tiempo_avanzado_total += tiempo_hijo;
			}
		}
        
	}
	
	//Si aun quedan unidades de trabajo procesar el cf
	if (qstart > 0 || process->cf == 0)
	{	
		//Si el estado anterior era waitng tiene que imprimir resume <pid>
		if (process->state == WAITING)
		{
			printf("RESUME %d\n", process->pid);
			fprintf(output_file, "RESUME %d\n", process->pid);
		}
		
		//Procesamos el tiempo antes de crear los hijos, osea el cf
		int cfToProcess = min(qstart, process->cf);
		process->cf -= cfToProcess;
		tiempo_proceso += cfToProcess;
		tiempo_avanzado_total += cfToProcess;
		qstart -= cfToProcess;
		//impirimir runnig si anterior era waiting y el tiempo de proceso es mayor a 0
		if (tiempo_proceso > 0 && process->state == WAITING)
		{
			printf("RUN %d %d\n", process->pid, tiempo_proceso);
			fprintf(output_file, "RUN %d %d\n", process->pid, tiempo_proceso);
		}
		process->state = RUNNING;

		if (process->cf == 0 && process->ci == 0)
		{
			process->state = FINISHED;
			printf("Proceso terminado: %d\n", process->pid);
			printf("END %d TIME %d\n", process->pid, tiempo + tiempo_avanzado_total);
			fprintf(output_file, "END %d TIME %d\n", process->pid, tiempo + tiempo_avanzado_total);
		}
	}
	process->time_in_cpu += tiempo_proceso;

	return tiempo_avanzado_total;
}


int process_group(Group* group, int tiempo, FILE* output_file){
	//Proceso el grupo
	int tiempo_avanzado = 0;
	Process* currentProcess = group->father;
	printf("Esta en process_group\n");
	tiempo_avanzado = process_process(currentProcess, group->qstart, currentProcess-> pid, tiempo, group,output_file);
	//actualizar qstart del grupo min{qstart - qdelta, qmin}
	//imprimo el tiempo avanzado
	printf("Tiempo avanzado: %d\n", tiempo_avanzado);
	group->qstart = max((group->qstart - group->qdelta), group->qmin);
	return tiempo_avanzado;	
}



int main(int argc, char const *argv[])
{
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);
	
	

	GroupNode* group_list_pending;
	GroupNode* group_list_active;
	GroupNode* group_list_ready;
	

	group_list_pending = readFileAndCreatStructures(file_name);
	input_file_destroy(input_file);
	//Ordeno la lista por tiempo y asigno gid
	sort_group_list(group_list_pending);
	assign_gid(group_list_pending->next);

	printf("Grupos con el gid asignado\n");
	printAllGroups(group_list_pending);

	//Crear e inicializar la lista de grupos active
	group_list_active = init_group_list();
	//Crear e inicializar la lista de grupos ready
	group_list_ready = init_group_list();
	

	//Abrir archivo salida
	FILE *output_file = fopen("output.txt", "w");
	
	int tiempo = 0;

	while (group_list_pending->next != NULL || group_list_active->next != NULL){
		int tiempo_anterior = tiempo;

		//Verifico  y activo grupos en el tiempo actual
		check_and_move_groups(group_list_pending, group_list_active, tiempo);

		

		
		if (group_list_active->next == NULL)
		{ 	
			while(group_list_active->next == NULL)
			{	
				printf("Entro a chequear move groups\n");
				tiempo++;
				check_and_move_groups(group_list_pending, group_list_active, tiempo);
				if (group_list_active->next != NULL)
				{
					printf("IDLE %d\n", tiempo- tiempo_anterior);
					fprintf(output_file, "IDLE %d\n", tiempo- tiempo_anterior);
				}
			}
		} 
		else {
		//Proceso los grupos activos en una iteracion y controlo el tiempo
			printf("Entro a procesar grupos activos\n");
			GroupNode* current = group_list_active->next;
			while(current != NULL){
				//Proceso los grupos activos
				//Si el en el grupo se ejecuta tiempo, osea avanza 
				tiempo_anterior = tiempo;
				int tiempo_avanzado = process_group(current->group, tiempo, output_file);
			

				//Si todos los prcesos del grupo terminaron cambio a lista ready
				if (current->group->father->state == FINISHED)
				{
					move_group_to_end(group_list_active, group_list_ready, current->group);
				}


				if (tiempo_avanzado == 0)
				{
					tiempo_avanzado = 1;
				}	

				if (group_list_pending->next == NULL && group_list_active->next == NULL)
				{
					if (tiempo_avanzado == 0)
				{
					tiempo_avanzado = 0;
				}	
				} 
				
				tiempo += tiempo_avanzado;

				//iterar en esta funcion desde el tiempo anterior al tiempo actual 
				//y ver si se ejecuta algun proceso pasa de lista pendeing a active
				for (int i = tiempo_anterior; i <= tiempo; i++)
				{
					// printf("Tiempo actual en iteracion pending: %d\n", i);
					check_and_move_groups(group_list_pending, group_list_active, i);
				}

				//Hacer update de qstart del grupo 
				current = current->next;
				
			}
			printf("REPORT START\n");
			fprintf(output_file, "REPORT START\n");
			printf("TIME %d\n", tiempo);
			fprintf(output_file, "TIME %d\n", tiempo);
			//printAllGroups(group_list_pending);
			//Reportar los pending
			GroupNode* current_active = group_list_active->next;
			while (current_active != NULL)
			{	
				//printf("Entro acá");
				printf("GROUP %d %d\n", current_active->group->father->gid, current_active->group->cantidad_procesos);
				fprintf(output_file, "GROUP %d %d\n", current_active->group->father->gid, current_active->group->cantidad_procesos);
				//Reportar procesosdel grupo
				Process* father = current_active->group->father;
				report_processes_not_finished(father, 0, output_file);
				current_active =current_active->next;	
			}

			//Reporta todos los grupos ready
			GroupNode* current_ready = group_list_ready->next;
			while (current_ready != NULL)
			{
				printf("GROUP %d %d\n", 0, current_ready->group->cantidad_procesos);
				fprintf(output_file, "GROUP %d %d\n", 0, current_ready->group->cantidad_procesos);
				//Reportar procesosdel grupo
				Process* father = current_ready->group->father;
				report_processes_finished(father, 0, output_file);
				current_ready =current_ready->next;	
			}
			printf("REPORT END\n");
			fprintf(output_file, "REPORT END\n");
			
		}	
	}
}

