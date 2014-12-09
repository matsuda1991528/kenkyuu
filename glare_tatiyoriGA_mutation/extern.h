#ifndef Extern_h
#define Extern_h

/* map data */
#define NODE_NUM 42
#define INF 9999
#define START 1    //スタート地点番号
#define GOAL 41      //ゴール地点番号
#define START_HOUR 16 //スタート時刻（時）
#define START_MIN 0   //スタート時刻（分)
#define MOVE_SPEED 60 //(km/h)
#define MAX_WAITTIME 200

#define DATA_BASE_SIZE 48  //estimateUncom 30min cicle -> 24*2 -> 48

/*  */

/* GA data */
#define GENERATION 100 //世代数
#define INDI_NUM  10//個体数
#define GENE_SIZE 13  //遺伝子長（立ちより地＋start+goal）

#define CROSSOVER_RATE 0.6
#define MUTATION_RATE 0.01
#define WAITTIMEMUTATION_RATE 0.1

struct xy_data{
	int x;
	int y;
};
struct node_data{
	int num;  //ノード番号
	struct xy_data pos;
};
struct matrix_data{
	double distance;        //距離
	double time;      //所要時間
	double angle;     //辺の角度
};
struct sun_data{
	double high;
	double width;
};
struct individualData{
	int gene[GENE_SIZE]; //遺伝子
	int waitTime;              //待ち時間
	double uncomValue;    //不快度
};
struct routeData{
	double time;  //移動時間	
	double uncom;  //不快度
	int route[NODE_NUM]; //部分経路
};
struct vector_3D{
	double x;
	double y;
	double z;
};
typedef struct{
	int flag;
	int num;
}dropPointData;
struct estimateData{
	double time;
	double matrix[GENE_SIZE][GENE_SIZE];
};
struct xy_data cord[NODE_NUM];
struct node_data node[NODE_NUM];
struct matrix_data matrix[NODE_NUM][NODE_NUM];
struct sun_data sun;
struct individualData individual[INDI_NUM];
struct individualData eliteIndividual;
struct estimateData estimateUncom[DATA_BASE_SIZE]; 
double elevation;
double azimuth;
int v_node[NODE_NUM];
int route[NODE_NUM];
dropPointData dropPoint[GENE_SIZE];

struct routeData serch_route(int, int, double);
double getUncomEstimate(int, int, double, double);
double getUncom(int, int, double, double);
void sunAngle(double); //太陽高度・方位算出
double getIlluminance(double, double); //太陽の法線照度算出
double get3D_angle(double, double); //視線軸と光源軸のなす角算出
double getEquivalent_velling_luminance(double, double); //等価光幕輝度算出
double getDisability_glare(double); //減能グレア値算出

#endif