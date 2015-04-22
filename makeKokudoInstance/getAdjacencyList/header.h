#ifndef HEADER_H
#define HEADER_H

#define MAX_ADJAJENCY_SIZE 8
#define EMPTY -1

/* xy座標を表現する構造体 */
struct xy_coord_t{
	double x;  //x座標
	double y;  //y座標
};

struct node_t{
	int num;  //ノード番号
	struct xy_coord_t pos;  //ノード座標
};

struct adj_list_t{
	int num;
	struct adj_list_t *next;
};

/* 頂点リストを表現する構造体 */
struct vertex_t{
	struct node_t node;
	struct adj_list_t *adj_list_p;
	struct adj_list_t *adj_list_head;
	struct adj_list_t *adj_list_old;
};


#endif