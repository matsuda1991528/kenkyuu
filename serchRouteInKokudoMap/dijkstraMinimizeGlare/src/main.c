#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

#define MAX_LEN 256
				
int main(int argc, char **argv){
	struct include_command_data_t input;
	initIncludeCommand(&input);
	
	/* ���s���R�}���h��ϐ��Ɋi�[ */
	while(--argc > 0 && **(++argv) == '-'){
		argc--;
		if(strcmp(*argv, "-start") == 0){
			input.start_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-goal") == 0){
			input.goal_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-dptr_h") == 0){
			input.dptr.hour = atoi(*++argv);
		}
		else if(strcmp(*argv, "-dptr_m") == 0){
			input.dptr.min = atoi(*++argv);
		}
		else if(strcmp(*argv, "-kph") == 0){
			input.vel_kph = atoi(*++argv);
		}
		else{
			printf("invailed command\n");
			exit(1);
		}
	}
	checkIncludeCommand(input);
	struct vertex_t *vertex;
	/* �m�[�h�T�C�Y�̎擾 */
	int node_size = loadNodeSize();
	checkStartAndGoalNum(input.start_vertex, input.goal_vertex, node_size);
	
	/* �אڃ��X�g�̋L���̈�̎擾 */	
	vertex = (struct vertex_t *)malloc(sizeof(struct vertex_t) * node_size);
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}

	/* ���X�g�̏����� */
	initVertex(vertex, node_size);
	loadNodeData(vertex, node_size);
	//printVertex(vertex, node_size);
	serchRouteDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph);
	free(vertex);

	return 0;
}