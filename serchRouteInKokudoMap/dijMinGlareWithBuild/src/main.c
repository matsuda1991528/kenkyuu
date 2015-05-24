#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"header.h"

#define MAX_LEN 256
				
int main(int argc, char **argv){
	struct include_command_data_t input;
	initIncludeCommand(&input);
	
	/* 実行時コマンドを変数に格納 */
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
	int node_size;
	struct build_set_t build_set;
	/* ノードサイズの取得 */
	node_size = loadNodeSize();
	checkStartAndGoalNum(input.start_vertex, input.goal_vertex, node_size);
	
	/* 隣接リストの記憶領域の取得 */	
	vertex = (struct vertex_t *)malloc(sizeof(struct vertex_t) * node_size);
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}

	/* リストの初期化 */
	initVertex(vertex, node_size);
	loadNodeData(vertex, node_size);
	//printVertex(vertex, node_size);
	getBuildPos(&build_set);
	
	//serchRouteDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_set);
	serchRouteDistDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_set);
	free(vertex);

	return 0;
}