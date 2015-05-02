#ifndef HEADER_H
#define HEADER_H

#define MAX_LEN 256
#define EMPTY -1
#define INF 9999999


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


int start_vertex, goal_vertex;

/* プロトタイプ宣言 */
/* commonFunc.c */
void fileOpenCheck(FILE**, char*, char*);

/* testFunc.c */
void checkIncludeCommand(int, int);
void printVertexList(int, struct adj_list_t*, struct adj_list_t*);

/* createAdjajencyList.c */
int loadNodeSize(void);
void initVertex(struct vertex_t*, int);
void loadNodeData(struct vertex_t*, int);
void printVertex(struct vertex_t*, int);

/* serchRouteDijkstra.c */
void serchRouteDijkstra(struct vertex_t*, int, int, int);

#endif