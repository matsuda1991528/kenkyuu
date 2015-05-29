#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
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
	struct vertex_t *vertex;  //���_�f�[�^
	int node_size; //���_�f�[�^�T�C�Y
	struct build_set_t build_set; //�����f�[�^
	int grid_size;
	struct build_grid_t *build_grid;
	build_grid = NULL;
	clock_t start, end;
	
	start = clock();
	
	/* �m�[�h�T�C�Y�̎擾 */
	node_size = loadNodeSize();
	checkStartAndGoalNum(input.start_vertex, input.goal_vertex, node_size);
	
	/* �אڃ��X�g�̋L���̈�̎擾 */	
	vertex = (struct vertex_t *)malloc(sizeof(struct vertex_t) * node_size);
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}

	/* ���X�g�̏����� */
	initVertex(vertex, node_size);
	//���_�f�[�^�̎擾
	loadNodeData(vertex, node_size);
	//�����f�[�^�̎擾
	getBuildPos(&build_set);

	if(ROUTE_PERPOSE == 0){
		if(GRID_MODE == 0)
			serchRouteDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_set);
		else if(GRID_MODE == 1){
			//�����f�[�^�O���b�h�}�b�v�̐���
			build_grid = createGridMap(build_set.head, build_grid, &grid_size);
			serchRouteGridDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_grid, grid_size);
		}
	}
	else if(ROUTE_PERPOSE == 1)
		serchRouteDistDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_set);
	
	free(vertex);
	
	end = clock();
	printf("process time : %lf[ms]\n", (double)(end - start) / 1000);

	return 0;
}