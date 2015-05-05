#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

/* 隣接リストの記憶領域の取得 */
struct adj_list_t *adjListMalloc(void){
	return (struct adj_list_t *)malloc(sizeof(struct adj_list_t));
}

/* 隣接リストデータの初期化 */
void initVertex(struct vertex_t *vertex, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		vertex[i].node.num = EMPTY;
		vertex[i].node.pos.x = EMPTY;
		vertex[i].node.pos.y = EMPTY;
		vertex[i].adj_list_head = adjListMalloc();
		vertex[i].adj_list_old = vertex[i].adj_list_p = vertex[i].adj_list_head;
	}
	return;
}

/* 
ファイルからノードサイズの読み込み 
リストのテールにはNULLが入る為，
ノード数の＋１を返している．
*/
int loadNodeSize(void){
	FILE *fp;
	char *fname = "..\\map_node_num.txt";
	char dummy[MAX_LEN];
	int node_size;
	fileOpenCheck(&fp, fname, "r");
	fscanf(fp, "%s %d\n", dummy, &node_size);
	
	return node_size + 1;
}

/* 
ノードサイズ分の記憶領域を確保
【引数】 
struct node_list* node ノードのポインタ
int node_size ノードサイズ
【戻り値】
無し
*/
void vertexMalloc(struct vertex_t* vertex, int node_size){
	
	vertex = (struct vertex_t *)malloc(node_size);	
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}
	return;
}

/* 
ノードをコピーする
コピー先のノード　destination_node
コピー元のノード　source_node
*/
void copyNode(struct node_t* destination_node, struct node_t source_node){
	destination_node->num = source_node.num;
	destination_node->pos.x = source_node.pos.x;
	destination_node->pos.y = source_node.pos.y;
	
	return;
}

/* 頂点データを隣接リストデータに格納する */
void setVertexData(struct vertex_t* target_vertex, struct node_t prev_node, int  next_node_num){
	if(target_vertex->node.num == EMPTY){
		copyNode(&target_vertex->node, prev_node);
		target_vertex->adj_list_head->num = next_node_num;
		target_vertex->adj_list_old = target_vertex->adj_list_head;
	}
	else{
		target_vertex->adj_list_p = adjListMalloc();
		target_vertex->adj_list_p->num = next_node_num;
		target_vertex->adj_list_old->next = target_vertex->adj_list_p;
		target_vertex->adj_list_old = target_vertex->adj_list_p;
	}
	return;
}

/* リストの終端にNULLを挿入する */
void insertNullToAdjList(struct vertex_t* vertex, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		vertex[i].adj_list_old->next = NULL;
	}
	
	return;
}
	
/* ファイルからノードデータを読み込む */
void loadNodeData(struct vertex_t* vertex, int node_size){
	FILE *fp;
	char *fname = "..\\map_node_num.txt";
	char buf[MAX_LEN];
	int dummy;
	struct node_t buf_prev, buf_next;  //ノードデータを格納するバッファ
	
	fileOpenCheck(&fp, fname, "r");
	fscanf(fp, "%s %d", buf, &dummy);
	while(fscanf(fp, "%d %lf %lf", &buf_prev.num, &buf_prev.pos.x, &buf_prev.pos.y) != EOF){
		fscanf(fp, "%d %lf %lf", &buf_next.num, &buf_next.pos.x, &buf_next.pos.y);
		setVertexData(&vertex[buf_prev.num], buf_prev, buf_next.num);
		setVertexData(&vertex[buf_next.num], buf_next, buf_prev.num);
	}
	insertNullToAdjList(vertex, node_size);
}


void printVertex(struct vertex_t* vertex, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		//printf("%5d %lf %lf |", vertex[i].node.num, vertex[i].node.pos.x, vertex[i].node.pos.y);
		printf("|%5d| -> ", vertex[i].node.num);
		vertex[i].adj_list_p = vertex[i].adj_list_head;
		while(vertex[i].adj_list_p != NULL){
			printf("|%d|", vertex[i].adj_list_p->num);
			vertex[i].adj_list_p = vertex[i].adj_list_p->next;
		}
		printf("\n");
	}
	return;
}
