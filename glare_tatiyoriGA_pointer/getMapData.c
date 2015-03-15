#ifndef GetMapData_C
#define GetMapData_C
#include<stdio.h>
#include<stdlib.h>
#include"header.h"
#include"param.h"

/* �n�}���擾�Ɋւ���v���g�^�C�v�錾 */
static struct node_t *my_malloc(void);
static struct node_t *getNodeposData(void);
static int countNodeNum(struct node_t *head);
static void getMatrixDistance(double **matrix_distance, int);
static void getMatrixAngle(double **matrix_angle, int);
static void setMatrixTravTime(double **matrix_trav_time, double **matrix_distance, int);
/* �������n���擾�Ɋւ���v���g�^�C�v�錾 */
static struct drop_node_t *drop_node_malloc(void);
static struct drop_node_t *getDropNodeFromFile(void);
//static void initCheckRepeatDropNode(struct check_repeat_drop_t *check_repeat_drop_node, int)

/*
struct node_t�^���̃o�C�g���̃��������m�ۂ��Cstruct node_t* �ւ̃L���X�g
*/
static struct node_t *my_malloc(void){
	return (struct node_t *)malloc(sizeof(struct node_t));
}

/* �m�[�h�ԍ��C�m�[�h�ʒu�̓ǂݍ��� */
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
		
/* �m�[�h���̎擾 */
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

/* �����s��̓ǂݍ��� */
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

/* �p�x�s��̓ǂݍ��� */
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

	/* �������̈�̎擾 */
	matrix_distance = (double **)malloc(sizeof(double*) * map_data.node_size);
	for(i=1;i<map_data.node_size;i++){
		matrix_distance[i] = (double*)malloc(sizeof(double) * map_data.node_size);
	}
	getMatrixDistance(matrix_distance, map_data.node_size);
		
	/* �������̈�̎擾 */
	matrix_angle = (double **)malloc(sizeof(double*) * map_data.node_size);
	for(i=1;i<map_data.node_size;i++){
		matrix_angle[i] = (double*)malloc(sizeof(double) * map_data.node_size);
	}
	getMatrixAngle(matrix_angle, map_data.node_size);
	/* �������̈�̎擾 */
	map_data.matrix_trav_time  = (double **)malloc(sizeof(double) * map_data.node_size);
	for(i=1;i<map_data.node_size;i++){
		map_data.matrix_trav_time[i] = (double*)malloc(sizeof(double) * map_data.node_size);
	}
	setMatrixTravTime(map_data.matrix_trav_time, matrix_distance, map_data.node_size);
	free(matrix_distance);
	
	return map_data;
}


///////////////////////////////////////////////////////////////////////////////////
///                         �������n���̎擾                                         ///
///////////////////////////////////////////////////////////////////////////////////

/* struct drop_node_t�^���̃o�C�g���̃��������m�ۂ��Cstruct drop_node_t* �ւ̃L���X�g */
static struct drop_node_t *drop_node_malloc(void){
	return (struct drop_node_t *)malloc(sizeof(struct drop_node_t));
}

/* �t�@�C�����痧�����n�����擾 */
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
drop_flag[] �̏����� 
start,goal�n�_�͗������n�ɑI�΂�Ȃ��悤�ɐݒ�
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
�������n�̃����_������ 
��ԍŏ��ɗ������n�_��START
��ԍŌ�ɗ������n�_��GOAL�ɑΉ�����
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
�������n���̎擾 
DROP_NODE_STYLE -> 1�@�t�@�C������擾
DROP_NODE_STYLE -> 2�@�����_������
DROP_NODE_STYLE -> otherwise  �����I��
*/
//���̈����̓����_�������̎��Ɏg���񂾂�`�`
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