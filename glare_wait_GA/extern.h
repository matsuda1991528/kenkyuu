#ifndef Extern_h
#define Extern_h

#define INF 999
#define NODE_NUM 42    //�n�_��
#define START 1             //�X�^�[�g�n�_�ԍ�
#define GOAL 41             //�S�[���n�_�ԍ�
#define START_HOUR 19//�X�^�[�g�����i���j
#define START_MIN 30   //�X�^�[�g�����i���j
#define MOVE_SPEED 60 //�ړ����x(km/h)

#define GENERATION 500000    //���㐔
#define INDI_NUM 500        //�̐�
#define LIST_SIZE 10      //�אڃ��X�g�ő�l
#define GENE_SIZE 42     //��`�q��(�ő�K��n��)
#define MAX_WAITTIME 100
#define CROSSOVER_RATE 0.6
#define MUTATION_RATE 0.6
#define WAITTIME_MUTATION_RATE 0.3

struct xy_data{
	int x;   //x���W
	int y;   //y���W
};

struct nodeData{
	int num;  //�m�[�h�ԍ�
	struct xy_data pos;
};

struct matrixData{
	double distance;   //����
	double time;        //���v����
	double angle;       //�ӂ̊p�x
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
	int gene[GENE_SIZE];    //��`�q���
	int waitTime;                 //�҂�����
	double uncomValue;       //�s���x
	double travelTime;         //�ړ�����
	int visitNum;                  //�K��n��
};

struct individualData individual[INDI_NUM];
struct individualData eliteIndividual;
struct matrixData matrix[NODE_NUM][NODE_NUM];
struct nodeData node[NODE_NUM];
int nextPosList[NODE_NUM][LIST_SIZE];  //�אڃ��X�g
double elevation;              //���z���x
double azimuth;                //���z����

void getSunAngle(double);//���z���x�E���ʎZ�o
double getIlluminance(double, double); //���z�̖@���Ɠx�Z�o
double get3D_angle(double, double); //�������ƌ������̂Ȃ��p�Z�o
double getEquivalent_velling_luminance(double, double); //���������P�x�Z�o
double getDisability_glare(double); //���\�O���A�l�Z�o
#endif