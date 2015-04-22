/*
 * main.c
 *
 *  Created on: 2015/04/19
 *      Author: sosmem
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

#define MAX_LEN 256
#define TRACE_TAIL -1

int node_size = 1;
//void fileOpenCheck(FILE*, char*, char*);

/* node_t型の記憶領域の取得 */
struct node_t *my_malloc(void){
	return (struct node_t *)malloc(sizeof(struct node_t));
}

/* ファイルオープンする際のNULLチェック */
void fileOpenCheck(FILE **fp, char *filename, char *mode){
	if((*fp = fopen(filename, mode)) == NULL){
		printf("%s is failed open\n", filename);
		exit(1);
	}
	return;
}

/*
 * データの重複チェックをする
 * if重複している→重複しているノード番号を返す
 * else TRACE_TAILを返す
 */
int getNodeNum(struct xy_coord_data_t target_pos, struct node_t *node){
	printf("a\n");
	while(node != NULL){
		if(node->pos.x == target_pos.x && node->pos.y == target_pos.y){
			return node->num;
		}
		else{
			node = node->pointer;
		}
	}
	return TRACE_TAIL;
}


/* head->ノードの始端 p->新たに追加するノード old->ノードの終端 */
struct node_t* loadNodeData(){
	struct node_t *head, *p, *old;
	FILE *fp;
	char *read_fname = "map_node_data.txt";
	char buf[MAX_LEN];
	int trace_state;

	/* ファイルのNULLチェック */
	fileOpenCheck(&fp, read_fname, "r");
	/* 先頭ノードの記憶領域の確保と値の格納 */
	head = my_malloc();
	fscanf(fp, "%lf %lf\n", &head->pos.x, &head->pos.y);
	head->num = node_size;
	node_size++;
	old = head;

	/* ファイルからxy座標の読み込み */
	while(fgets(buf, sizeof(buf), fp)){
		/* 空行かそうでないかの判断 */
		/* eclipse unix環境の場合 */
		//if(*buf != '\0'){
		/* Cygwin環境の場合 */
		if(*buf != '\0' && *buf != '\r'){
			/* 記憶領域の確保と座標データのロード */
			p = my_malloc();
			sscanf(buf, "%lf %lf\n", &p->pos.x, &p->pos.y);
			/* 座標データの重複チェック */
			trace_state = getNodeNum(p->pos, head);
			/* 重複がなかった場合 */
			if(TRACE_TAIL == trace_state){
				p->num = node_size;
				node_size++;
			}
			else{
				p->num = trace_state;
			}
			old->pointer = p;
			old = p;
		}
	}
	old->pointer = NULL;
	return head;
}



void fWriteNodeData(struct node_t *head){
	struct node_t *node;
	FILE *fp;
	char *write_fname = "map_node_num.txt";
	int line_count = 0;
	node = head;

	fileOpenCheck(&fp, write_fname, "wt");
	fprintf(fp, "nodenum %d\n", node_size-1);
	printf("a\n");
	while(node != NULL){
		fprintf(fp, "%5d %lf %lf\n", node->num, node->pos.x, node->pos.y);
		line_count++;
		if(line_count % 2 == 0){
			fprintf(fp, "\n");
		}
		node = node->pointer;
	}
	return;
}


int main(void){
	
	fWriteNodeData(loadNodeData());

	return 0;
}
