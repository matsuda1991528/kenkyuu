#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

#define MAX_LEN 256

/* �t�@�C���I�[�v������ۂ�NULL�`�F�b�N */
void fileOpenCheck(FILE **fp, char *filename, char *mode){
	if((*fp = fopen(filename, mode)) == NULL){
		printf("%s is failed open\n", filename);
		exit(1);
	}
	return;
}


/* 
�t�@�C������m�[�h�T�C�Y�̓ǂݍ��� 
���X�g�̃e�[���ɂ�NULL������ׁC
�m�[�h���́{�P��Ԃ��Ă���D
*/
int loadNodeSize(){
	FILE *fp;
	char *fname = "map_node_num.txt";
	char dummy[MAX_LEN];
	int node_size;
	fileOpenCheck(&fp, fname, "r");
	fscanf(fp, "%s %d\n", dummy, &node_size);
	
	return node_size + 1;
}

/* 
�m�[�h�T�C�Y���̋L���̈���m��
�y�����z 
struct node_list* node �m�[�h�̃|�C���^
int node_size �m�[�h�T�C�Y
�y�߂�l�z
����
*/
void vertexMalloc(struct vertex_t* vertex, int node_size){
	
	vertex = (struct vertex_t *)malloc(node_size);	
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}
	return;
}

/* �אڃ��X�g�̋L���̈�̎擾 */
struct adj_list_t *adjListMalloc(void){
	return (struct adj_list_t *)malloc(sizeof(struct adj_list_t));
}

/* �אڃ��X�g�f�[�^�̏����� */
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
�m�[�h���R�s�[����
�R�s�[��̃m�[�h�@destination_node
�R�s�[���̃m�[�h�@source_node
*/
void copyNode(struct node_t* destination_node, struct node_t source_node){
	destination_node->num = source_node.num;
	destination_node->pos.x = source_node.pos.x;
	destination_node->pos.y = source_node.pos.y;
	
	return;
}

/* ���_�f�[�^��אڃ��X�g�f�[�^�Ɋi�[���� */
void setVertexData(struct vertex_t* target_vertex, struct node_t prev_node, int  next_node_num){
	if(target_vertex->node.num == EMPTY){
		copyNode(&target_vertex->node, prev_node);
		target_vertex->adj_list_head = adjListMalloc();
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


void insertNullToAdjList(struct vertex_t* vertex, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		vertex[i].adj_list_p = NULL;
	}
	
	return;
}
	
/* �t�@�C������m�[�h�f�[�^��ǂݍ��� */
void loadNodeData(struct vertex_t* vertex, int node_size){
	FILE *fp;
	char *fname = "map_node_num.txt";
	char buf[MAX_LEN];
	int dummy;
	struct node_t buf_prev, buf_next;  //�m�[�h�f�[�^���i�[����o�b�t�@
	
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
	//node_size = 10;
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
		
		
int main(void){
	struct vertex_t *vertex;
	/* �m�[�h�T�C�Y�̎擾 */
	int node_size = loadNodeSize();
	
	/* �אڃ��X�g�̋L���̈�̎擾 */	
	vertex = (struct vertex_t *)malloc(sizeof(struct vertex_t) * node_size);
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}

	/* ���X�g�̏����� */
	initVertex(vertex, node_size);
	loadNodeData(vertex, node_size);
	printVertex(vertex, node_size);
	free(vertex);

	return 0;
}