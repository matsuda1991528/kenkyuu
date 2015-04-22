#ifndef HEADER_H
#define HEADER_H

#define MAX_ADJAJENCY_SIZE 8
#define EMPTY -1

/* xy���W��\������\���� */
struct xy_coord_t{
	double x;  //x���W
	double y;  //y���W
};

struct node_t{
	int num;  //�m�[�h�ԍ�
	struct xy_coord_t pos;  //�m�[�h���W
};

struct adj_list_t{
	int num;
	struct adj_list_t *next;
};

/* ���_���X�g��\������\���� */
struct vertex_t{
	struct node_t node;
	struct adj_list_t *adj_list_p;
	struct adj_list_t *adj_list_head;
	struct adj_list_t *adj_list_old;
};


#endif