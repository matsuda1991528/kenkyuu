#ifndef Extern_h
#define Extern_h

/* map data */
#define NODE_NUM 42
#define INF 9999
#define START 1    //�X�^�[�g�n�_�ԍ�
#define GOAL 41      //�S�[���n�_�ԍ�
#define START_HOUR 17 //�X�^�[�g�����i���j
#define START_MIN 0   //�X�^�[�g�����i��)
#define MOVE_SPEED 60 //(km/h)
#define MAX_WAITTIME 200

#define DATA_BASE_SIZE 48  //estimateUncom 30min cicle -> 24*2 -> 48

/*  */

/* GA data */
#define GENERATION 1000 //���㐔
#define INDI_NUM  5//�̐�
#define GENE_SIZE 42  //��`�q���i�������n�{start+goal+1�j

#define CROSSOVER_RATE 0.6
#define MUTATION_RATE 0.01
#define WAITTIMEMUTATION_RATE 0.1

struct xy_data{
	int x;
	int y;
};
struct node_data{
	int num;  //�m�[�h�ԍ�
	struct xy_data pos;
};
struct matrix_data{
	double distance;        //����
	double time;      //���v����
	double angle;     //�ӂ̊p�x
};
struct sun_data{
	double high;
	double width;
};
struct individualData{
	int gene[GENE_SIZE]; //��`�q
	int waitTime;              //�҂�����
	double uncomValue;    //�s���x
};
struct routeData{
	double time;  //�ړ�����	
	double uncom;  //�s���x
	int route[NODE_NUM]; //�����o�H
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
double crossover_rate;
double mutation_rate;
int seed;
double estimateUncomParameter; //parameter of estimateUncom


/* definition of prottype function */
struct routeData serch_route(int, int, double);
//double getUncomEstimate(int, int, double);
double getUncom(int, int, double, double);
void sunAngle(double); //���z���x�E���ʎZ�o
double getIlluminance(double, double); //���z�̖@���Ɠx�Z�o
double get3D_angle(double, double); //�������ƌ������̂Ȃ��p�Z�o
double getEquivalent_velling_luminance(double, double); //���������P�x�Z�o
double getDisability_glare(double); //���\�O���A�l�Z�o

#endif