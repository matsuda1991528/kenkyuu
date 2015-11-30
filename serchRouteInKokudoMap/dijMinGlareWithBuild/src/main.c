#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"header.h"

#define MAX_LEN 256

void readArgm(int argc, char **argv, struct include_command_data_t *input){

	while(--argc > 0 && **(++argv) == '-'){
		argc--;
		if(strcmp(*argv, "-start") == 0){
			input->start_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-goal") == 0){
			input->goal_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-dptr_h") == 0){
			input->dptr.hour = atoi(*++argv);
		}
		else if(strcmp(*argv, "-dptr_m") == 0){
			input->dptr.min = atoi(*++argv);
		}
		else if(strcmp(*argv, "-kph") == 0){
			input->vel_kph = atoi(*++argv);
		}
		else if(strcmp(*argv, "-gx") == 0){
			g_size.x = atof(*++argv);
		}
		else if(strcmp(*argv, "-gy") == 0){
			g_size.y = atof(*++argv);
		}
		else if(strcmp(*argv, "-lam") == 0){
			lambda = atof(*++argv);
		}
		else if(strcmp(*argv, "-mode")==0){
			input->route_mode = atoi(*++argv);
		}else{
			printf("invailed command\n");
			exit(1);
		}
	}
	return;
}

int main(int argc, char **argv){
	struct include_command_data_t input;
	initIncludeCommand(&input);
	readArgm(argc, argv, &input);
	
	test_build_count = 0;
	test_grid_count = 0;
	test_edge_count = 0;
	test_overlap_build_counter = 0;
	
	/* ���s���R�}���h��ϐ��Ɋi�[ */
	checkIncludeCommand(input);
	struct vertex_t *vertex;  //���_�f�[�^
	int node_size; //���_�f�[�^�T�C�Y
	struct build_set_t build_set; //�����f�[�^
	struct grid_size_t grid_size; //�O���b�h�̓��������Ɠ�k�����ɂ�����Z����
	struct build_grid_t **build_grid;
	int loop_state = FALSE;
	build_grid = NULL;
	clock_t start, end;
	start = end = 0;
	
	//start = clock();
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
	//�����̍ő啝�̎擾
	build_width = getMaxBuildWidth(build_set.head);		
	
/* �s���x�ŏ��o�H��T������ꍇ */
	if(input.route_mode == 0){
		/* �G���[�g�f�[�^�̏����� */
		initElite(node_size);
		//�����f�[�^�O���b�h�}�b�v�̐���
		build_grid = createGridMap(build_set.head, build_grid, &grid_size);
		start = clock();
		shotestPathTravTime = serchRouteDistDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.vel_kph);

		while(loop_state == FALSE){
			loop_state = serchRouteGridDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_grid, grid_size);
		}
		end = clock();
	}
	
/* �ŒZ�o�H��T������ꍇ */
	else if(input.route_mode == 1){
		//start = clock();
		build_grid = createGridMap(build_set.head, build_grid, &grid_size);
		serchRouteGridDistDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_grid, grid_size);
	}			
		//end = clock();
	else{
		printf("invailed route mode  -mode -> 0 minimize uncom route  -mode -> 1 minimize dist route\n");
	}
	
	free(vertex);

	//printf("%d, %d, %d, %d, %f, %d\n", (int)g_size.x, (int)g_size.y, test_grid_count, test_build_count, (double)(end - start) / 1000.0f, test_overlap_build_counter);
/* �v�Z���Ԃ̎Z�o */	
/*
	if(g_size.y == 1000)
		printf("%lf\n", (double)(end - start) / 1000);
	else
		printf("%lf, ", (double)(end - start) / 1000);
*/
		
	//if(g_size.y == 2000)
	//	printf("%d\n", test_build_count);
	//else
	//	printf("%d,", test_build_count);
		
	//if(g_size.y == 2000)
	//	printf("%d\n", test_grid_count);
	//else
	//	printf("%d,", test_grid_count);
	
	//printf("%d\n", test_edge_count);
	return 0;
}