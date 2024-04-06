#include <stdio.h>	
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h> 
#include "../file_manager/manager.h"
#include "group.h"
#include "process.h"
#include "groupNode.h"
#include "printing.h"


void readChilds(FILE *file, Process* father, int nh){
	 for (int i = 0; i < nh; i++) {
		printf("PROCESO EN CREACION\n");
		if (i > 0)
		{
			printf("Es el padre maximo y no es el primer hijo\n");
			int ce;
			fscanf(file, "%d", &ce);
			printf("CE: %d\n", ce);
			father->ce[i - 1] = ce;
		}

		int child_ci, child_nh, child_cf;
		fscanf(file, "%d %d", &child_ci, &child_nh);
		printf("CI: %d - NH: %d\n", child_ci, child_nh);
		Process* new_process_child = create_process(child_ci, child_nh, 0, false);
		father->children[i] = new_process_child;
		printf("Proceso hijo creado: %d - %d - %ls - %d\n", new_process_child->ci, new_process_child->nh, new_process_child->ce, new_process_child->cf);

		if (child_nh == 0)
		{
			fscanf(file, "%d", &child_cf);
			printf("CF: %d\n", child_cf);
			new_process_child->cf = child_cf;
			printf("No tiene hijos el proceso hijo\n");
		}
		else
		{
			readChilds(file, new_process_child, child_nh); 
			fscanf(file, "%d", &child_cf);
			printf("CF: %d\n", child_cf);
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

	//LEctura resto de las k-1 lineas
	for (int i = 0; i < K -1 ; i++)
	{
		int ti, ci, nh;
		fscanf(file, "%d %d %d", &ti, &ci, &nh);
		//printf("TI: %d - CI: %d - NH: %d\n", ti, ci, nh);
		//crear el grupo
		Group* new_group = create_group(ti, qstart, qdelta, qmin);
		//crear el proceso padre máximo
		Process* new_process = create_process(ci, nh, 0, true);
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
			readChilds(file, new_process, nh);
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







int main(int argc, char const *argv[])
{
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	GroupNode* group_list_pending;
	GroupNode* group_list_active;
	GroupNode* group_list_ready;

	group_list_active = init_group_list();
	group_list_ready = init_group_list();
	group_list_pending = readFileAndCreatStructures(file_name);
	//Ordeno la lista por tiempo y asigno gid
	sort_group_list(group_list_pending);
	assign_gid(group_list_pending->next);

	printf("Grupos con el gid asignado\n");
	printAllGroups(group_list_pending);


	int tiempo = 0;

	//Función check en el tiempo que este y ver si hay uno o más grupos pendientes que se deban agregar a la lista active
	//hacer funcion
	//check_pending_groups(tiempo, group_list_pending, group_list_active);
	// void check_pending_groups(tiempo, group_list_pending, group_list_active){
	// 	GroupNode* current = group_list_pending->next;
	// 	while (current != NULL) {
	// 		if (current->group->time_arrival == tiempo) {
	// 			GroupNode* next = current->next;
	// 			//Mover el grupo de la lista de pendientes a la lista de activos
	// 			add_group(group_list_active, current->group);
	// 			//Eliminar el grupo de la lista de pendientes
	// 			delete_group(group_list_pending, current->group->gid);
	// 			current = next;
	// 		} else {
	// 			current = current->next;
	// 		}
	// 	}
	// }


	// Liberar memoria
	input_file_destroy(input_file);
}