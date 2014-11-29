#ifndef Extern_h
#define Extern_h

/* map data */
#define NODE_NUM 42
#define INF 999
#define START 28    //�X�^�[�g�n�_�ԍ�
#define GOAL 1      //�S�[���n�_�ԍ�
#define START_HOUR 19 //�X�^�[�g�����i���j
#define START_MIN 30   //�X�^�[�g�����i��)
#define MOVE_SPEED 60 //(km/h)
#define MAX_WAITTIME 200

/*  */

/* GA data */
#define GENERATION 1  //���㐔
#define INDI_NUM  5//�̐�
#define GENE_SIZE 6  //��`�q���i�������n�{start+goal�j

#define CROSSOVER_RATE 0.6
#define MUTATION_RATE 0.01
#define WAITTIMEMUTATION_RATE 0.01

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
struct xy_data cord[NODE_NUM];
struct node_data node[NODE_NUM];
struct matrix_data matrix[NODE_NUM][NODE_NUM];
struct sun_data sun;
struct individualData individual[INDI_NUM];
struct individualData eliteIndividual;
int nodeNum;
double elevation;
double azimuth;
int v_node[NODE_NUM];
int route[NODE_NUM];

struct routeData serch_route(int, int, double);
void sunAngle(double); //���z���x�E���ʎZ�o
double getIlluminance(double, double); //���z�̖@���Ɠx�Z�o
double get3D_angle(double, double); //�������ƌ������̂Ȃ��p�Z�o
double getEquivalent_velling_luminance(double, double); //���������P�x�Z�o
double getDisability_glare(double); //���\�O���A�l�Z�o

#endif