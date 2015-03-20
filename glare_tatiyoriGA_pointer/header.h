#ifndef Header_H
#define Header_H
#include"param.h"

struct node_t{
	struct node_t *next;
	int num;
	int x;
	int y;
};

struct drop_node_t{
	struct drop_node_t *next;
	int num;
};

struct check_repeat_drop_t{
	int num;
	int flag;
};

struct matrix_data{
	double distance;
	double trav_time;
	double angle;
};

struct map_data_t{
	double **matrix_trav_time;
	int node_size;
};

struct individual_t{
	int *gene;
	int wait_time;
	double fitness;
};

struct ga_population_t{
	struct individual_t individual[IND_NUM];
	struct individual_t elite_individual;
	int gene_length;
};


struct dijkstra_uncom_data_t{
	double time;
	double **matrix;
};

struct sun_angle_t{
	double elevation;  //太陽高度
	double azimuth;   //太陽方位
};

struct vector_3D_t{
	double x;
	double y;
	double z;
};

struct route_data_t{
	double time;
	double uncom;
};

struct estimate_uncom_data_t{
	double uncom;
	int node;
};

/* グローバル変数の定義 */
struct dijkstra_uncom_data_t *dijkstra_uncom_db;
double **matrix_angle;
double crossover_rate, mutation_rate, wait_time_mutation_rate;
double estimate_uncom_parameter;
double inverse_parameter;
int start_hour;
int seed;

void print_gene(struct ga_population_t);
void printIndividual(struct individual_t, int);
struct map_data_t getMapData(void);
struct drop_node_t *getDropNode(int, int);
int setGeneLength(struct drop_node_t *drop_node);
void copyCheckRepeatDropNode(struct drop_node_t *drop_node, struct check_repeat_drop_t *check_repeat_drop);
struct ga_population_t getPopulation(struct ga_population_t ga_population, struct drop_node_t *drop_node_head);
void checkRepeatGene(struct ga_population_t, struct check_repeat_drop_t *check_repeat_drop);
struct ga_population_t gene_malloc(struct ga_population_t);
struct dijkstra_uncom_data_t *setEstimateUncomDB(struct drop_node_t *drop_node, int, struct map_data_t);
struct sun_angle_t getSunAngle(double);
double getGlare(int, int, struct sun_angle_t);
struct ga_population_t generatePopulation(struct ga_population_t, struct map_data_t, struct drop_node_t *drop_node_head);
struct ga_population_t evaluation(struct ga_population_t, struct map_data_t);
struct route_data_t serch_route(int, int, struct map_data_t, double);
struct ga_population_t saveElite(struct ga_population_t);
#endif