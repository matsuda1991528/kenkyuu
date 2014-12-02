#ifndef Extern_h
#define Extern_h
#define INF 999
#define NODE_NUM 42 //�n�_�ԍ�
#define START 1    //�X�^�[�g�n�_�ԍ�
#define GOAL 41      //�S�[���n�_�ԍ�
#define START_HOUR 19 //�X�^�[�g�����i���j
#define START_MIN 0   //�X�^�[�g�����i���j
#define MOVE_SPEED 60 //(km/h)
#define MAX_WAITTIME 200

struct xy_data{
	int x;    //�m�[�h�����W
	int y;    //�m�[�h�����W
};
struct xy_data cord[NODE_NUM];

struct node_data{
	int num;  //�m�[�h�ԍ�
	struct xy_data pos;
};
struct node_data node[NODE_NUM];

struct matrix_data{
	double distance;        //����
	double time;  //���v����
	double angle;     //�ӂ̊p�x
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

void getSunAngle(double); //���z���x�E���ʎZ�o
double getIlluminance(double, double); //���z�̖@���Ɠx�Z�o
double get3D_angle(double, double); //�������ƌ������̂Ȃ��p�Z�o
double getEquivalent_velling_luminance(double, double); //���������P�x�Z�o
double getDisability_glare(double); //���\�O���A�l�Z�o

#endif