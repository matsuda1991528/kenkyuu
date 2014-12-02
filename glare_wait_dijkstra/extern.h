#ifndef Extern_h
#define Extern_h
#define INF 999
#define NODE_NUM 42 //地点番号
#define START 1    //スタート地点番号
#define GOAL 41      //ゴール地点番号
#define START_HOUR 19 //スタート時刻（時）
#define START_MIN 0   //スタート時刻（分）
#define MOVE_SPEED 60 //(km/h)
#define MAX_WAITTIME 200

struct xy_data{
	int x;    //ノードｘ座標
	int y;    //ノードｙ座標
};
struct xy_data cord[NODE_NUM];

struct node_data{
	int num;  //ノード番号
	struct xy_data pos;
};
struct node_data node[NODE_NUM];

struct matrix_data{
	double distance;        //距離
	double time;  //所要時間
	double angle;     //辺の角度
};
struct matrix_data matrix[NODE_NUM][NODE_NUM];

struct sun_data{
	double high;
	double width;
};
struct sun_data sun;

struct vector_3D{
	double x;
	double y;
	double z;
};


double elevation;
double azimuth;
int v_node[NODE_NUM];
int route[NODE_NUM];
double uncomfot_value;
double new_uncomfort_value;

double new_necessary_time;
double new_uncom;
int new_route[NODE_NUM];
int waiting_time;

double elite_necessary_time;
double elite_uncom;
int elite_route[NODE_NUM];
double elite_waiting_time;

void getSunAngle(double); //太陽高度・方位算出
double getIlluminance(double, double); //太陽の法線照度算出
double get3D_angle(double, double); //視線軸と光源軸のなす角算出
double getEquivalent_velling_luminance(double, double); //等価光幕輝度算出
double getDisability_glare(double); //減能グレア値算出

#endif