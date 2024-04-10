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
		if (i > 0)
		{
			int ce;
			fscanf(file, "%d", &ce);
			arg++;
			father->ce[i - 1] = ce;
		}

		int child_ci, child_nh, child_cf;
		fscanf(file, "%d %d", &child_ci, &child_nh);
		arg++;
		arg++;
		Process* new_process_child = create_process(child_ci, child_nh, 0, false, line_ci, arg);
		father->children[i] = new_process_child;
		if (child_nh == 0)
		{
			fscanf(file, "%d", &child_cf);
			new_process_child->cf = child_cf;
			arg++;
		}
		else
		{
			readChilds(file, new_process_child, child_nh, line_ci, arg); 
			fscanf(file, "%d", &child_cf);
			arg++;
			new_process_child->cf = child_cf;
		}		
	}
	return;
}

GroupNode* readFileAndCreatStructures(char *file_name){
	FILE *file = fopen(file_name, "r");
	if (file == NULL)
	{
		return NULL;
	}

	int K, qstart, qdelta, qmin;
	fscanf(file, "%d", &K);
	fscanf(file, "%d %d %d", &qstart, &qdelta, &qmin);

	//crear e inicializar la lista ligada de grupos GroupNode 
	GroupNode* group_list_pending = init_group_list();
	int arg = 1;
	//LEctura resto de las k-1 lineas
	for (int i = 0; i < K -1 ; i++)
	{
		int ti, ci, nh;
		fscanf(file, "%d %d %d", &ti, &ci, &nh);
		//crear el grupo
		Group* new_group = create_group(ti, qstart, qdelta, qmin);
		//crear el proceso padre máximo
		Process* new_process = create_process(ci, nh, 0, true, i+1, arg);
		//agregar el proceso al grupo
		new_group-> father = new_process;
		//agregar el grupo a la lista de grupos
		add_group(group_list_pending, new_group);
		//lo primero es que si el nh del padre maximo es 0 el numero que viene va a ser el cf del padre maximo y no tendrá ni un hijo
		if (nh == 0)
		{ //NO hay hijos leo solo el input que viene y lo guardo como el cf del padre
			int cf;
			fscanf(file, "%d", &cf);
			new_process->cf = cf;
			new_process->cf_original = cf;
			// termina el for 
		}
		else
		{	
			//llamar a la función recursiva que lee los hijos
			readChilds(file, new_process, nh, i+1, arg);
			//Lee el cf despues de construir todos los hijos
			int cf;
			fscanf(file, "%d", &cf);
			//lo asingo al padre
			new_process->cf = cf;
			new_process->cf_original = cf;
			//imprimo la info del padre
		}


	GroupNode* current = group_list_pending->next; // Si tienes un nodo cabeza ficticio, empieza con el siguiente
    while (current != NULL) {
        if (current->group->father->is_father_max) {
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
	//Procesamos el tiempo antes de crear los hijos, osea el ci
	//Si es la primera vez que se corre el ci tengo que asigan el pid y ppid
	if (process->pid == -1)
	{	
		//cambio estado a READY
		process->state = READY;
		fprintf(output_file, "Linea 125 PID: %d State %d\n", process->pid, process->state);
		process->pid = currentPID;
		currentPID++;
		group->cantidad_procesos++;
		//si no hay padre, el ppid es 0
		if (process->is_father_max==true)
		{	
			process->ppid = 0;
			group->gid = process->pid;
			assign_gid_recursively(process, group->gid);
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
	process->ci -= timeToProcess;
	tiempo_proceso += timeToProcess;
	tiempo_avanzado_total += timeToProcess;
	qstart -= timeToProcess;
	
	//Reporto el tiempo que corrio
	//RUN <PID> <TIEMPO_TRABAJADO>
	if (tiempo_proceso > 0)
	{
		printf("RUN %d %d\n", process->pid, tiempo_proceso);
		fprintf(output_file, "RUN %d %d\n", process->pid, tiempo_proceso);
		process->state = RUNNING;
		fprintf(output_file, "Linea 158 PID: %d State %d\n", process->pid, process->state);
		if( process->ci == 0 && process->nh > 0 && qstart>=0)
		{
			process->state = WAITING;
			fprintf(output_file, "Linea 163 PID: %d State %d\n", process->pid, process->state);
		}
		
	}

	//si quedan unidades de trabajo por trabajar y hay hijos por recorrer: 
	if (qstart >= 0 && process->nh > 0)
	{
		for (int i = 0; i < process->nh && qstart>=0 ; i++)
		{	
			int tiempo_hijo = 0;
			
			if (qstart  == 0 && process->ci==0 ){
				//WAIT
				printf("WAIT %d\n", process->pid);
				fprintf(output_file, "WAIT %d\n", process->pid);
				//lamo a recursiva 
				process->state = WAITING;
				fprintf(output_file, "Linea 180 PID: %d State %d\n", process->pid, process->state);
				tiempo_hijo += process_process(process->children[i], qstart, process->pid, tiempo+tiempo_avanzado_total, group, output_file);
			}
			else if (qstart > 0) {
			//solo entra si el proceso del hijo ya termino y hay mas de un hijo y no es el ultimo hijo el proceso terminado
			//Si i mayor a 0 y ell hijo anterior termino
			if (i>0 && process->children[i-1]->state == FINISHED){
				process->state = RUNNING; 
				fprintf(output_file, "Linea 183 PID: %d State %d\n", process->pid, process->state);
				int ceToProcess = min(qstart, process->ce[i-1]);
				process->ce[i-1] -= ceToProcess;
				tiempo_proceso += ceToProcess;
				tiempo_avanzado_total += ceToProcess;
				qstart -= ceToProcess;
				//Si el ce queda en cero cambia a waiting pq va a correr otro hijo 
				if (process->ce[i-1] == 0 )
				{	

					if(ceToProcess > 0)
					{

						printf("RUN %d %d\n", process->pid, ceToProcess);
						fprintf(output_file, "RUN %d %d\n", process->pid, ceToProcess);
						printf("WAIT %d\n", process->pid);
						fprintf(output_file, "WAIT %d\n", process->pid);
					}
					process->state = WAITING;
					fprintf(output_file, "Linea 202 PID: %d State %d\n", process->pid, process->state);
					tiempo_hijo += process_process(process->children[i], qstart, process->pid, tiempo+tiempo_avanzado_total, group, output_file);
					qstart-= tiempo_hijo;
					tiempo_avanzado_total += tiempo_hijo;
					if (process->children[i]-> ci != 0 ||process->children[i]-> cf != 0 ){
						process->children[i]->state= RUNNING;
						process->state = WAITING;
						fprintf(output_file, "Linea 209 PID: %d State %d\n", process->pid, process->state);
					}
				}
				else{
					//estado waiting
					process->state = RUNNING;
					fprintf(output_file, "Linea 215 PID: %d State %d\n", process->pid, process->state);
				}
			}
			
			//Si es el primer hijo o el hijo anterior no termino 
			//no hay ce 
			else {
			     
				//solo imprimir si es el primer hijo no el ultimo
				if (i == 0 && process->children[i]->pid == -1){
					fprintf(output_file, "WAIT %d\n", process->pid);
					printf("WAIT %d\n", process->pid);
				}
				//Procesamos el hijo
				tiempo_hijo += process_process(process->children[i], qstart, process->pid, tiempo+tiempo_avanzado_total, group, output_file);
				qstart-= tiempo_hijo;
				//Si no se alcanza a recorrer el ci del proceso 
				// cambio el estado a waiting 
				if (process->children[i]-> ci != 0 ||process->children[i]-> cf != 0 ){
					process->children[i]->state= RUNNING;
					if(process->children[i]-> ci  == 0){
						process->children[i]->state= WAITING;
					}

					process->state = WAITING;
					fprintf(output_file, "Linea 236 PID: %d State %d\n", process->pid, process->state);
					fprintf(output_file, "Linea 236 PID: %d State %d\n", process->children[i]->pid, process->children[i]->state);
				}

				tiempo_avanzado_total += tiempo_hijo;
			}
			}
		
		}
        
	}
	
	//Si aun quedan unidades de trabajo procesar el cf
	if (qstart > 0 ||(process->cf == 0 && process->ci == 0))
	{	
		int cfToProcess = min(qstart, process->cf);
		process->cf -= cfToProcess;
		tiempo_proceso += cfToProcess;
		tiempo_avanzado_total += cfToProcess;
		qstart -= cfToProcess;
		if (cfToProcess > 0){
			printf("RUN %d %d\n", process->pid, tiempo_proceso);
			fprintf(output_file, "RUN %d %d\n", process->pid, tiempo_proceso);
		}
		if (process->state != FINISHED)
		{
			process->state = RUNNING;
			fprintf(output_file, "Linea 262 PID: %d State %d\n", process->pid, process->state);
		}
		
		if (process->cf == 0 && process->ci == 0 && process->state != FINISHED)
		{
			process->state = FINISHED;
			fprintf(output_file, "Linea 268 PID: %d State %d\n", process->pid, process->state);
			process ->time_finished = tiempo + tiempo_avanzado_total;
			//print el run
			printf("END %d TIME %d\n", process->pid, tiempo + tiempo_avanzado_total);
			fprintf(output_file, "END %d TIME %d\n", process->pid, tiempo + tiempo_avanzado_total);
			//ACA SI EL PROCESO PADRE DEL HIJO ESTA EN WAIT IMPRIMIR RESUME 
			
			if (process->ppid != 0)
			{
				printf("RESUME %d\n", process->ppid);
				fprintf(output_file, "RESUME %d\n", process->ppid);
			}
		}
	}
	process->time_in_cpu += tiempo_proceso;

	return tiempo_avanzado_total;
}


int process_group(Group* group, int tiempo, FILE* output_file){
	//Proceso el grupo
	int tiempo_avanzado = 0;
	Process* currentProcess = group->father;
	tiempo_avanzado = process_process(currentProcess, group->qstart, currentProcess-> pid, tiempo, group,output_file);
	group->qstart = max((group->qstart - group->qdelta), group->qmin);
	return tiempo_avanzado;	
}



int main(int argc, char const *argv[])
{
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);
	
	//finished
	Process* finished_process[1000];
	int finished_process_count = 0;

	GroupNode* group_list_pending;
	GroupNode* group_list_active;
	GroupNode* group_list_ready;
	

	group_list_pending = readFileAndCreatStructures(file_name);
	input_file_destroy(input_file);
	//Ordeno la lista por tiempo y asigno gid
	sort_group_list(group_list_pending);
	assign_gid(group_list_pending->next);


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

		//Si no hay grupos activos en la iteracion
		if (group_list_active->next == NULL)
		{ 	
			while(group_list_active->next == NULL)
			{	
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
				

				//Hacer update de qstart del grupo 
				current = current->next;
			}
			for (int i = tiempo_anterior; i <= tiempo; i++)
				{
					check_and_move_groups(group_list_pending, group_list_active, i);
				}
			/////////////ACÁ HAGO EL REPORTE FINAL ///////////
			finished_process_count = 0;
			printf("REPORT START\n");
			fprintf(output_file, "REPORT START\n");
			printf("TIME %d\n", tiempo);
			fprintf(output_file, "TIME %d\n", tiempo);
			//Recolectamos los procesos terminados de grupos no terminados
			GroupNode* current_ready = group_list_active->next;
			while (current_ready != NULL)
			{
				collect_finished_processes(current_ready->group->father, finished_process, &finished_process_count);
				current_ready = current_ready->next;
			}

			//Recolectamos los procesos de grupos terminados
			GroupNode* current_finished = group_list_ready->next;
			while (current_finished != NULL)
			{
				collect_finished_processes(current_finished->group->father, finished_process, &finished_process_count);
				current_finished = current_finished->next;
			}
			
			//Reportar los pno terminados
			GroupNode* current_active = group_list_active->next;
			while (current_active != NULL && current_active->group->father->pid != -1)
			{	
				printf("GROUP %d %d\n", current_active->group->father->pid, current_active->group->cantidad_procesos);
				fprintf(output_file, "GROUP %d %d\n", current_active->group->father->pid, current_active->group->cantidad_procesos - finished_process_count);
				//Reportar procesosdel grupo
				report_processes(current_active->group->father, output_file);	
				current_active = current_active->next;
			}
			
			sort_finished_processes(finished_process, finished_process_count);

			if (finished_process_count > 0)
			{
				printf("GROUP %d %d\n", 0, finished_process_count);
				fprintf(output_file, "GROUP %d %d\n", 0, finished_process_count);
			}

			for (int i = 0; i < finished_process_count; i++) {
				printf("PROGRAM %d %d %d %s %d\n",
					finished_process[i]->pid, 0, 0, "FINISHED", finished_process[i]->time_in_cpu);
				fprintf(output_file, "PROGRAM %d %d %d %s %d\n",
					finished_process[i]->pid, 0, 0, "FINISHED", finished_process[i]->time_in_cpu);
			}
			printf("REPORT END\n");
			fprintf(output_file, "REPORT END\n");
			
			///////// TERMINA EL REPORTE FINAL //////////
			
		}	
	}
}

