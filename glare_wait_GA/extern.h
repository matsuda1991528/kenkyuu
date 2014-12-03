#ifndef Extern_h
#define Extern_h

#define INF 999
#define NODE_NUM 42    //地点数
#define START 1             //スタート地点番号
#define GOAL 41             //ゴール地点番号
#define START_HOUR 19//スタート時刻（時）
#define START_MIN 30   //スタート時刻（分）
#define MOVE_SPEED 60 //移動速度(km/h)

#define GENERATION 500000    //世代数
#define INDI_NUM 500        //個体数
#define LIST_SIZE 10      //隣接リスト最大値
#define GENE_SIZE 42     //遺伝子長(最大訪問地数)
#define MAX_WAITTIME 100
#define CROSSOVER_RATE 0.6
#define MUTATION_RATE 0.6
#define WAITTIME_MUTATION_RATE 0.3

struct xy_data{
	int x;   //x座標
	int y;   //y座標
};

struct nodeData{
	int num;  //ノード番号
	struct xy_data pos;
};

struct matrixData{
	double distance;   //距離
	double time;        //所要時間
	double angle;       //辺の角度
};

struct vector_3D{
	double x;
	double y;
	double z;
};
//struct sun_data{
//	double high;
//	double width;
//};

struct individualData{
	int gene[GENE_SIZE];    //遺伝子情報
	int waitTime;                 //待ち時間
	double uncomValue;       //不快度
	double travelTime;         //移動時間
	int visitNum;                  //訪問地数
};

struct individualData individual[INDI_NUM];
struct individualData eliteIndividual;
struct matrixData matrix[NODE_NUM][NODE_NUM];
struct nodeData node[NODE_NUM];
int nextPosList[NODE_NUM][LIST_SIZE];  //隣接リスト
double elevation;              //太陽高度
double azimuth;                //太陽方位

void getSunAngle(double);//太陽高度・方位算出
double getIlluminance(double, double); //太陽の法線照度算出
double get3D_angle(double, double); //視線軸と光源軸のなす角算出
double getEquivalent_velling_luminance(double, double); //等価光幕輝度算出
double getDisability_glare(double); //減能グレア値算出
#endif