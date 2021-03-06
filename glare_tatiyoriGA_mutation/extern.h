#ifndef Extern_h
#define Extern_h

/* map data */
#define NODE_NUM 42
#define INF 9999
#define START 1    //X^[gn_Ô
#define GOAL 41      //S[n_Ô
#define START_HOUR 16 //X^[gij
#define START_MIN 0   //X^[giª)
#define MOVE_SPEED 60 //(km/h)
#define MAX_WAITTIME 200

#define DATA_BASE_SIZE 48  //estimateUncom 30min cicle -> 24*2 -> 48

/*  */

/* GA data */
#define GENERATION 100 //¢ã
#define INDI_NUM  10//ÂÌ
#define GENE_SIZE 13  //â`q·i§¿æèn{start+goalj

#define CROSSOVER_RATE 0.6
#define MUTATION_RATE 0.01
#define WAITTIMEMUTATION_RATE 0.1

struct xy_data{
	int x;
	int y;
};
struct node_data{
	int num;  //m[hÔ
	struct xy_data pos;
};
struct matrix_data{
	double distance;        //£
	double time;      //vÔ
	double angle;     //ÓÌpx
};
struct sun_data{
	double high;
	double width;
};
struct individualData{
	int gene[GENE_SIZE]; //â`q
	int waitTime;              //Ò¿Ô
	double uncomValue;    //sõx
};
struct routeData{
	double time;  //Ú®Ô	
	double uncom;  //sõx
	int route[NODE_NUM]; //ªoH
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
void sunAngle(double); //¾zxEûÊZo
double getIlluminance(double, double); //¾zÌ@üÆxZo
double get3D_angle(double, double); //ü²Æõ¹²ÌÈ·pZo
double getEquivalent_velling_luminance(double, double); //¿õPxZo
double getDisability_glare(double); //¸\OAlZo

#endif