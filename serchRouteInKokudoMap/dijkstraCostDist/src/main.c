#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

#define MAX_LEN 256
				
int main(void){
	struct vertex_t *vertex;
	/* ノードサイズの取得 */
	int node_size = loadNodeSize();
	
	/* 隣接リストの記憶領域の取得 */	
	vertex = (struct vertex_t *)malloc(sizeof(struct vertex_t) * node_size);
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}

	/* リストの初期化 */
	initVertex(vertex, node_size);
	loadNodeData(vertex, node_size);
	printVertex(vertex, node_size);
	serchRouteDijkstra(vertex, node_size);
	free(vertex);

	return 0;
}