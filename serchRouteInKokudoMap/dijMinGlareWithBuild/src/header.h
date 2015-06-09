#ifndef HEADER_H
#define HEADER_H

#define MAX_LEN 256
#define EMPTY -1
#define INF -1
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 50

#define BUILD_HIGH 5 //�����̍���[m]

#define PRINT_ROUTE 0 //0->�o�H��\�����Ȃ��@1->�o�H��\������
#define BUILD_MODE 1 // 0->�������l�����Ȃ��@1->�������l������
#define ROUTE_PERPOSE 0 //0->�s���x�ŏ��o�H�@1->�����ŒZ�o�H
#define MEASURE_PROCESS_TIME_MODE 1 //0->�������Ԃ��v�����Ȃ� 1->�������Ԃ��v������
#define GRID_MODE  1 //0->�n�}�f�[�^�̃O���b�h���@1���n�}�f�[�^�̃O���b�h������
#define GRID_PATTERN 1 //0���e�̒����Ɋ�Â����O���b�h�c�����P���R���\�[��������͂����O���b�h�c����
#define PRINT_GRID_MAP 0 //0->�O���b�h�n�}���t�@�C���������݂��Ȃ��@1���t�@�C���������݂���

#define TEST_GRID 1 //1���O���b�h�����̃e�X�g 

#define TRUE 1
#define FALSE 0


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


/* ������\���\���� */
struct build_pos_t{
	int num;       //�����ԍ�
	double width; //������
	struct xy_coord_t pos; //�����̒��S�ʒu
	struct build_pos_t *next;
};

struct build_set_t{
	struct build_pos_t *head; //�擪���X�g�̓_�~�[
	int build_size;
};

//�O���b�h�n�}�̃T�C�Y��\���D
struct grid_size_t{
	int vertical;
	int width;
};


/* �����f�[�^�O���b�h�}�b�v��\���\���� */
struct build_grid_t{
	struct xy_coord_t min;  //�O���b�h�n�}�̍ŏ�xy���W
	struct xy_coord_t max; //�O���b�h�n�}�̍ő�xy���W
	struct build_pos_t *head;  //�������X�g  //�擪���X�g�̓_�~�[
	struct build_pos_t *p;
	struct build_pos_t *old;
};
	
/* ���z�p�x��\���\���� */
struct sun_angle_t{
	double elev;   //���x
	double azim;  //����
};

/* �R�}���h���C��������\���\���� */
struct include_command_data_t{
	int start_vertex;
	int goal_vertex;
	struct time_t dptr;
	int vel_kph;
};

/* 3�����x�N�g���p�̍\���� */
struct vector_3D_t{
	double x;
	double y;
	double z;
};

struct elite_data_t{
	double uncom;
	double wait_time;
	int* prev;
};	
	
//�O���[�o���ϐ�
//�e�O���b�h�̃T�C�Y
struct xy_coord_t g_size;
double grid_scale_fact;
struct elite_data_t elite; //�G���[�g�f�[�^


/* �v���g�^�C�v�錾 */
/* commonFunc.c */
void fileOpenCheck(FILE**, char*, char*);
void initElite(int node_size);
void updateElite(double target_uncom, double target_wait, int* prev, int node_size);


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
//void serchRouteDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_set_t*);
void serchRouteDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_set_t);

int serchRouteGridDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_grid_t**, struct grid_size_t);

void serchRouteDistDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_set_t);

/* getSunAngle.c */
struct sun_angle_t getSunAngle(double);
/* getGlare.c */
double getGlare(struct sun_angle_t, double);

void getBuildPos(struct build_set_t*);

//�����f�[�^�ɂ��O���A�񒼎˔���
//�ꊇ�Ǘ�
int getSunStateWithBuild(struct sun_angle_t, struct xy_coord_t, struct build_set_t);
//�O���b�h�ŊǗ�
int getSunStateWithBuildFromGrid(struct sun_angle_t, struct xy_coord_t, struct build_grid_t**, struct grid_size_t);

struct xy_coord_t getGridLength(void);
struct build_grid_t** createGridMap(struct build_pos_t*, struct build_grid_t**, struct grid_size_t*);


#endif