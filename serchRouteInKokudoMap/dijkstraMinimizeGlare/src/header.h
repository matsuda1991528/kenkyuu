#ifndef HEADER_H
#define HEADER_H

#define MAX_LEN 256
#define EMPTY -1
#define INF -1
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 50

#define PRINT_ROUTE 0 //0->�o�H��\�����Ȃ��@1->�o�H��\������


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

struct time_t{
	int hour;
	int min;
};

/* ���z�p�x��\���\���� */
struct sun_angle_t{
	double elev;   //���x
	double azim;  //����
};

struct include_command_data_t{
	int start_vertex;
	int goal_vertex;
	struct time_t dptr;
	int vel_kph;
};

struct vector_3D_t{
	double x;
	double y;
	double z;
};

/* �v���g�^�C�v�錾 */
/* commonFunc.c */
void fileOpenCheck(FILE**, char*, char*);

/* testFunc.c */
void initIncludeCommand(struct include_command_data_t*);
void checkIncludeCommand(struct include_command_data_t);
void checkStartAndGoalNum(int, int, int);
void printVertexList(int, struct adj_list_t*, struct adj_list_t*);

/* createAdjajencyList.c */
int loadNodeSize(void);
void initVertex(struct vertex_t*, int);
void loadNodeData(struct vertex_t*, int);
void printVertex(struct vertex_t*, int);

/* serchRouteDijkstra.c */
void serchRouteDijkstra(struct vertex_t*, int, int, int, struct time_t, int);

/* getSunAngle.c */
struct sun_angle_t getSunAngle(double);
/* getGlare.c */
double getGlare(struct sun_angle_t, double);

#endif