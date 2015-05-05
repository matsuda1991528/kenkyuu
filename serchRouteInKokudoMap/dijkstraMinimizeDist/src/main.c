#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

#define MAX_LEN 256
				
int main(int argc, char **argv){
	start_vertex = goal_vertex = 0;
	/* 実行時コマンドを変数に格納 */
	while(--argc > 0 && **(++argv) == '-'){
		argc--;
		if(strcmp(*argv, "-start") == 0){
			start_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-goal") == 0){
			goal_vertex = atoi(*++argv);
		}
		else{
			printf("invailed command\n");
			exit(1);
		}
	}
	checkIncludeCommand(start_vertex, goal_vertex);
	
	struct vertex_t *vertex;
		//printf("a\n");
	/* ノードサイズの取得 */
	int node_size = loadNodeSize();

	//printf("node_size = %d\n", node_size);
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
	serchRouteDijkstra(vertex, node_size, start_vertex, goal_vertex);
	free(vertex);

	return 0;
}