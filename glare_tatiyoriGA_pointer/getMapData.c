#ifndef GetMapData_C
#define GetMapData_C
#include<stdio.h>
#include<stdlib.h>
#include"header.h"
#include"param.h"

/* 地図情報取得に関するプロトタイプ宣言 */
static struct node_t *my_malloc(void);
static struct node_t *getNodeposData(void);
static int countNodeNum(struct node_t *head);
static void getMatrixDistance(double **matrix_distance, int);
static void getMatrixAngle(double **matrix_angle, int);
static void setMatrixTravTime(double **matrix_trav_time, double **matrix_distance, int);
/* 立ち寄り地情報取得に関するプロトタイプ宣言 */
static struct drop_node_t *drop_node_malloc(void);
static struct drop_node_t *getDropNodeFromFile(void);
//static void initCheckRepeatDropNode(struct check_repeat_drop_t *check_repeat_drop_node, int)

/*
struct node_t型分のバイト数のメモリを確保し，struct node_t* へのキャスト
*/
static struct node_t *my_malloc(void){
	return (struct node_t *)malloc(sizeof(struct node_t));
}

/* ノード番号，ノード位置の読み込み */
static struct node_t *getNodeposData(void){
	char *fname = "map500/node.csv";
	FILE *fp;
	char dummy[255];
	struct node_t *head, *node, *old;
	
	head = my_malloc();
	fp = fopen(fname, "r");
	if(fp == NULL){
		printf("file1 open error\n");
		exit(1);
	}
	else{
		fscanf(fp, "%s", dummy);
		fscanf(fp, "%s", dummy);
		fscanf(fp, "%s", dummy);
		old = head;
		while(node = my_malloc(), fscanf(fp, "%d,", &node->num) != EOF){
			fscanf(fp, "%d,", &node->x);
			fscanf(fp, "%d", &node->y);
			old->next = node;
			old = node;
		}
	}
	fclose(fp);
	old->next = NULL;
	return head;
}
		
/* ノード数の取得 */
static int countNodeNum(struct node_t *head){
	struct node_t *p;
	int node_size = 0;
	p = head;
	while(p != NULL){
		//printf("%5d%5d%5d\n", p->num, p->x, p->y);
		p = p->next;
		node_size++;
	}
	//printf("node_size = %d\n", node_size);
	return node_size;
}

/* 距離行列の読み込み */
static void getMatrixDistance(double **matrix_distance, int size){
	char *fname = "map500/matrixDistance.csv";
	FILE *fp;
	int i, j,  integerDummy;
	fp = fopen(fname, "r");
	if(fp == NULL){
		printf("file2 open error\n");
	}else{
		fscanf(fp, "  ,");
		for(i=1;i<size;i++){
			fscanf(fp, "%d,", &integerDummy);
		}
		fscanf(fp, "\n");
		for(i=1;i<size;i++){
			fscanf(fp, "%d,", &integerDummy);
			for(j=1;j<size;j++){
				fscanf(fp,"%lf,", &matrix_distance[i][j]);
			}
			fscanf(fp, "\n");
		}
	}
	fclose(fp);
	return;
}

/* 角度行列の読み込み */
static void getMatrixAngle(double **matrix_angle, int size){
	char *fname = "map500/matrixAngle.csv";
	FILE *fp;
	int i, j, integerDummy;
	
	fp = fopen(fname, "r");
	if(fp == NULL){
		printf("file3 open error\n");
	}else{
		fscanf(fp, "  ,");
		for(i=1;i<size;i++){
			fscanf(fp, "%d,", &integerDummy);				
		}
		fscanf(fp,"\n");
		for(i=1;i<size;i++){
			fscanf(fp, "%d,", &integerDummy);
			for(j=1;j<size;j++){
				fscanf(fp, "%lf,", &matrix_angle[i][j]);
			}
			fscanf(fp, "\n");
		}
	}
	fclose(fp);
	return;
}

void setMatrixTravTime(double **matrix_trav_time, double **matrix_distance, int size){
	int i, j;
	for(i=1;i<size;i++){
		for(j=1;j<size;j++){
			if(matrix_distance[i][j] != INF){
				matrix_trav_time[i][j] = matrix_distance[i][j] / MOVE_SPEED * UNIT_OF_TRAVTIME;
			}
			else{
				matrix_trav_time[i][j] = INF;
			}
		}
	}
	return;
}
			
struct map_data_t getMapData(void){
	struct map_data_t map_data;
	double **matrix_distance;
	//double **matrix_angle;
	map_data.node_size = countNodeNum(getNodeposData());
	int i;

	/* メモリ領域の取得 */
	matrix_distance = (double **)malloc(sizeof(double*) * map_data.node_size);
	for(i=1;i<map_data.node_size;i++){
		matrix_distance[i] = (double*)malloc(sizeof(double) * map_data.node_size);
	}
	getMatrixDistance(matrix_distance, map_data.node_size);
		
	/* メモリ領域の取得 */
	matrix_angle = (double **)malloc(sizeof(double*) * map_data.node_size);
	for(i=1;i<map_data.node_size;i++){
		matrix_angle[i] = (double*)malloc(sizeof(double) * map_data.node_size);
	}
	getMatrixAngle(matrix_angle, map_data.node_size);
	/* メモリ領域の取得 */
	map_data.matrix_trav_time  = (double **)malloc(sizeof(double) * map_data.node_size);
	for(i=1;i<map_data.node_size;i++){
		map_data.matrix_trav_time[i] = (double*)malloc(sizeof(double) * map_data.node_size);
	}
	setMatrixTravTime(map_data.matrix_trav_time, matrix_distance, map_data.node_size);
	free(matrix_distance);
	
	return map_data;
}


///////////////////////////////////////////////////////////////////////////////////
///                         立ち寄り地情報の取得                                         ///
///////////////////////////////////////////////////////////////////////////////////

/* struct drop_node_t型分のバイト数のメモリを確保し，struct drop_node_t* へのキャスト */
static struct drop_node_t *drop_node_malloc(void){
	return (struct drop_node_t *)malloc(sizeof(struct drop_node_t));
}

/* ファイルから立ち寄り地情報を取得 */
static struct drop_node_t *getDropNodeFromFile(void){
	char *fname = "map500/visitPos10.csv";
	FILE *fp;
	char dummy[255];
	struct drop_node_t *head, *drop_node, *old;
	int cnt = 1;
	
	head = drop_node_malloc();
	fp = fopen(fname, "r");
	if(fp == NULL){
		printf("drop_file open error\n");
		exit(1);
	}
	else{
		fscanf(fp, "%s,", dummy);
		fscanf(fp, "%d", &head->num);
		old = head;
		while(drop_node = drop_node_malloc(), fscanf(fp, "%s,", dummy) != EOF){
			fscanf(fp, "%d", &drop_node->num);
			//printf("drop_node %d = %d\n",cnt, drop_node->num);
			old->next = drop_node;
			old = drop_node;
			cnt++;
		}
	}
	fclose(fp);
	old->next = NULL;
	return head;
}

/* 
drop_flag[] の初期化 
start,goal地点は立ち寄り地に選ばれないように設定
*/
static void initCheckRepeatDropNode(struct check_repeat_drop_t *check_repeat_drop_node, int size){
	int i;
	for(i=1;i<size;i++){
		check_repeat_drop_node->num = i;
		if(i == START){
			check_repeat_drop_node->flag = 1;
		}
		else if(i==GOAL){
			check_repeat_drop_node->flag = 1;
		}
		else{
			check_repeat_drop_node->flag = 0;
		}
	}
	return;
}

/* 
立ち寄り地のランダム生成 
一番最初に立ち寄る地点はSTART
一番最後に立ち寄る地点はGOALに対応する
*/		
static struct drop_node_t *getDropNodeRandom(int drop_size, int node_size){
	struct drop_node_t *head, *drop_node, *old;
	struct check_repeat_drop_t *check_repeat_drop_node;
	int i, tmp;
	
	check_repeat_drop_node = (struct check_repeat_drop_t*)malloc(sizeof(struct check_repeat_drop_t) * node_size);
	initCheckRepeatDropNode(check_repeat_drop_node, node_size);
	
	head = drop_node_malloc();
	head->num = START;
	old = head;
	for(i=2;i<drop_size -1;i++){
		drop_node = drop_node_malloc();	
		while(1){
			tmp = rand()%(node_size-2) + 1;
			if(check_repeat_drop_node[tmp].flag ==0){
				drop_node->num = check_repeat_drop_node[tmp].num;
				old->next = drop_node;
				old = drop_node;
				break;
			}
		}
	}
	drop_node = drop_node_malloc();
	drop_node->num = GOAL;
	old->next = drop_node;
	old = drop_node;
	old->next = NULL;
	return head;
}

/* 
立ち寄り地情報の取得 
DROP_NODE_STYLE -> 1　ファイルから取得
DROP_NODE_STYLE -> 2　ランダム生成
DROP_NODE_STYLE -> otherwise  強制終了
*/
//この引数はランダム生成の時に使うんだよ～～
struct drop_node_t *getDropNode(int drop_size, int node_size){
	struct drop_node_t *head;
	switch(DROP_NODE_STYLE){
		case 1:
			head = getDropNodeFromFile();
			break;
		case 2:
			head = getDropNodeRandom(drop_size, node_size);
			break;
		default:
			printf("invailed number on DROP_NODE_STYLE\n");
			exit(1);
	}
	return head;
}

#endif