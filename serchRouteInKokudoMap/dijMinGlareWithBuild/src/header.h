#ifndef HEADER_H
#define HEADER_H

#define MAX_LEN 256
#define EMPTY -1
#define INF -1
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 30

#define BUILD_HIGH 5 //建物の高さ[m]
#define BUILD_WIDTH 5 //建物の幅[m]


#define PRINT_ROUTE 1 //0->経路を表示しない　1->経路を表示する
#define PRINT_GLARE_ROUTE 1 //0->眩しさを感じた部分経路を表示する．1->表示する
#define BUILD_MODE 1 // 0->建物を考慮しない　1->建物を考慮する
#define MEASURE_PROCESS_TIME_MODE 0 //0->処理時間を計測しない 1->処理時間を計測する
#define PRINT_GRID_MAP 0 //0->グリッド地図をファイル書き込みしない　1→ファイル書き込みする
#define BUILD_WIDTH_MODE 1 // 0->走査対象グリッド選定にて，取り扱う影幅を定数で取り扱う 1->最大建物幅で取り扱う．
#define GSZ_EXP 1 //適切なグリッドサイズを探す時→0　そうでない場合→1

#define TEST_GRID 1 //1→グリッド分割のテスト 

#define TRUE 1
#define FALSE 0

#define YEAR 2015
#define MONTH 6
#define DAY 21


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


/* 建物を表す構造体 */
struct build_pos_t{
	int num;       //建物番号
	double width; //建物幅
	struct xy_coord_t pos; //建物の中心位置
	struct build_pos_t *next;
};

struct build_set_t{
	struct build_pos_t *head; //先頭リストはダミー
	int build_size;
};

//グリッド地図のサイズを表す．
struct grid_size_t{
	int vertical; //南北方向
	int width;    //東西方向
};


/* 建物データグリッドマップを表す構造体 */
struct build_grid_t{
	struct xy_coord_t min;  //グリッド地図の最小xy座標
	struct xy_coord_t max; //グリッド地図の最大xy座標
	struct build_pos_t *head;  //建物リスト  //先頭リストはダミー
	struct build_pos_t *p;
	struct build_pos_t *old;
};
	
/* 太陽角度を表す構造体 */
struct sun_angle_t{
	double elev;   //高度
	double azim;  //方位
};

/* コマンドライン引数を表す構造体 */
struct include_command_data_t{
	int start_vertex;//出発地点
	int goal_vertex;//帰着地点
	struct time_t dptr;//出発時刻
	int vel_kph;//移動速度[km/h]
	int route_mode;//0->不快度最小経路　1->距離最短経路
};

/* 3次元ベクトル用の構造体 */
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
	
//グローバル変数
//各グリッドのサイズ
struct xy_coord_t g_size; //グリッドの分割サイズ
double shotestPathTravTime;
struct elite_data_t elite; //エリートデータ
unsigned int test_build_count; //建物探索回数（テスト用）
unsigned int test_grid_count; //グリッド走査回数（テスト用）
unsigned int test_edge_count; //辺探索回数（テスト用）
unsigned int test_overlap_build_counter;
int test_print_build;
double lambda;
double build_width;



/* プロトタイプ宣言 */
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

double serchRouteDistDijkstra(struct vertex_t*, int, int, int, int);

void serchRouteGridDistDijkstra(struct vertex_t*, int, int, int, struct time_t, int, struct build_grid_t**, struct grid_size_t);

/* getSunAngle.c */
struct sun_angle_t getSunAngle(double);
/* getGlare.c */
double getGlare(struct sun_angle_t, double);

void getBuildPos(struct build_set_t*);
double getMaxBuildWidth(struct build_pos_t*);

//建物データによるグレア非直射判定
//一括管理
int getSunStateWithBuild(struct sun_angle_t, struct xy_coord_t, struct build_set_t);
//グリッドで管理
int getSunStateWithBuildFromGrid(struct sun_angle_t, struct xy_coord_t, struct build_grid_t**, struct grid_size_t);

struct xy_coord_t getGridLength(void);
struct build_grid_t** createGridMap(struct build_pos_t*, struct build_grid_t**, struct grid_size_t*);


#endif