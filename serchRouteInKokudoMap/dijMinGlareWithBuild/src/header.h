#ifndef HEADER_H
#define HEADER_H

#define MAX_LEN 256
#define EMPTY -1
#define INF -1
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 50

#define BUILD_HIGH 5 //建物の高さ[m]

#define PRINT_ROUTE 0 //0->経路を表示しない　1->経路を表示する
#define BUILD_MODE 1 // 0->建物を考慮しない　1->建物を考慮する
#define MEASURE_PROCESS_TIME_MODE 1 //0->処理時間を計測しない 1->処理時間を計測する

#define TRUE 1
#define FALSE 0


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

struct time_t{
	int hour;
	int min;
};


struct build_pos_t{
	int num;
	struct xy_coord_t pos;
	struct build_pos_t *next;
};

struct build_set_t{
	struct build_pos_t *head;
	int build_size;
};
	

/* 太陽角度を表す構造体 */
struct sun_angle_t{
	double elev;   //高度
	double azim;  //方位
};

/* コマンドライン引数を表す構造体 */
struct include_command_data_t{
	int start_vertex;
	int goal_vertex;
	struct time_t dptr;
	int vel_kph;
};

/* 3次元ベクトル用の構造体 */
struct vector_3D_t{
	double x;
	double y;
	double z;
};

/* プロトタイプ宣言 */
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
//void serchRouteDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_set_t*);
void serchRouteDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_set_t);

/* getSunAngle.c */
struct sun_angle_t getSunAngle(double);
/* getGlare.c */
double getGlare(struct sun_angle_t, double);

void getBuildPos(struct build_set_t*);

int getSunStateWithBuild(double, double, double, struct xy_coord_t, struct build_set_t);


#endif