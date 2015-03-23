#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"header.h"
#include"param.h"


static void selection(struct ga_population_t*);
static struct ga_population_t specificWesteringSunMutation(struct ga_population_t, struct map_data_t, struct drop_node_t*);
static struct ga_population_t mutation(struct ga_population_t, struct map_data_t, struct drop_node_t*);
static double getPassingTime(struct individual_t, int, struct map_data_t);
static struct estimate_uncom_data_t *selectDijkstraUncomDB(double, int, int, int*, struct estimate_uncom_data_t*);
static double *getEstimateTuningUncom(struct estimate_uncom_data_t*, double*, int);
static double getSumTuningUncom(int, double*);
static double *getEstimateDev(double*, double*, double, int);
static double *getEstimateRatio(double*, int, double*, double);
static double *getEstimateRatio(double*, int, double*, double);
static double getSumRatio(double*, int);
static int getMutateGene(int, double*, double, struct estimate_uncom_data_t*);
static struct individual_t swapGene(int, int, int, struct individual_t);
static struct ga_population_t waitTimeMutation(struct ga_population_t);


void printIndividual(struct individual_t individual, int gene_length){
	int i;
	for(i=1;i<gene_length;i++)
		printf("|%2d",individual.gene[i]);
	printf("|\n");
}

void printRoute(struct individual_t individual, int gene_length, struct map_data_t map_data){
	struct route_data_t route_data;
	double current_time;
	int i;	
	current_time = individual.fitness = individual.wait_time;
	//current_time += start_hour * 60 + START_MIN;
	for(i=1;i<gene_length-1;i++){
		route_data = serch_route(individual.gene[i], individual.gene[i+1], map_data, current_time);
		current_time += route_data.time;
		individual.fitness += route_data.uncom;
		//printf("%d -> %d\n", individual.gene[i], individual.gene[i+1]);
		//printf("time = %3.0lf:%3.0f\n", current_time/24 +start_hour, START_MIN + fmod(current_time, 24));
		//printf("travel time = %f  uncom = %f\n", route_data.time, route_data.uncom);
		//printf("fitness = %f\n\n", individual.fitness);
	}
	return;
}
		
/* 
個体をコピーする
コピー内容は遺伝子，待ち時間遺伝子，適応度である． 
コピー先の個体→destination_individual
コピー元の個体→source_individual
*/
void copyIndividual
(struct individual_t *destination_individual, struct individual_t *source_individual, int gene_length){
	int i;
	
	for(i=1;i<gene_length;i++){
		destination_individual->gene[i] = source_individual->gene[i];
	}	
	destination_individual->wait_time = source_individual->wait_time;
	destination_individual->fitness = source_individual->fitness;
	
	return;
}


struct ga_population_t evaluation
(struct ga_population_t ga_population, struct map_data_t map_data){
	struct route_data_t route_data;
	double current_time;
	int i, j;
	
	for(i=0;i<IND_NUM;i++){
		current_time = ga_population.individual[i].wait_time;
		ga_population.individual[i].fitness = ga_population.individual[i].wait_time;
		for(j=1;j<ga_population.gene_length-1;j++){
			route_data = serch_route(ga_population.individual[i].gene[j], ga_population.individual[i].gene[j+1], map_data, current_time);
			current_time += route_data.time;
			ga_population.individual[i].fitness += route_data.uncom;
		}
	}
	return ga_population;
}

static void selection
(struct ga_population_t* ga_population){

	int num1, num2, i;
	struct ga_population_t ga_population_new;
	ga_population_new.gene_length = ga_population->gene_length;
	gene_malloc(&ga_population_new);
	
	for(i=0;i<IND_NUM;i++){
		/* トーナメントにかける個体をランダムに選択する */
		while(1){
			num1 = random()%IND_NUM;
			num2 = random()%IND_NUM;
			if(num1 != num2)
				break;
		}
		
		/* トーナメントに個体で適応度が低い方を選択していく */
		if(ga_population->individual[num1].fitness < ga_population->individual[num2].fitness){
			copyIndividual(&ga_population_new.individual[i], 
								  &ga_population->individual[num1], 
								  ga_population->gene_length);
		}else{
			copyIndividual(&ga_population_new.individual[i], 
								  &ga_population->individual[num2], 
								  ga_population->gene_length);
		}
	}
	
	/* 
	最後に戻す．
	戻さないと，値は書き換わらないぞ！
	*/
	for(i=0;i<IND_NUM;i++){
		copyIndividual(&ga_population->individual[i], 
							  &ga_population_new.individual[i], 
							  ga_population->gene_length);
	}
	
	for(i=0;i<IND_NUM;i++){
		free(ga_population_new.individual[i].gene);
	}
	free(ga_population_new.elite_individual.gene);
	
	return;
}


/* crossover pmx method */
static void crossover(struct ga_population_t* ga_population){
	double crossover_ran;
	int num1, num2;
	int first_cross_point, second_cross_point;
	int cross_num1, cross_num2;
	int i, j;
	int tmp_gene[IND_NUM][ga_population.gene_length];
	
	for(i=0;i<IND_NUM;i++){
		for(j=1;j<ga_population.gene_length;j++){
			tmp_gene[i][j] = ga_population.individual[i].gene[j];
		}
	}
	

	for(i=0;i<IND_NUM;i+=2){
		if(IND_NUM % 2 == 1 && i+1 == IND_NUM)
			break;

/* crossover するか否かの判定 */			
		crossover_ran = random()%100+1;
		if(crossover_ran <= crossover_rate){
		
		/* 
		それぞれの個体に対して交叉点をランダムに選択する 
		ただし，始発点，終着点（個体の両端に格納されている遺伝子）
		は交叉されないようにしている．
		*/		
			while(1){
				num1 = random()%(ga_population.gene_length - 3) + 2; 
				num2 = random()%(ga_population.gene_length - 3) + 2;
				if(num1 != num2)
					break;
			}
			if(num1 < num2){
				first_cross_point = num1;
				second_cross_point = num2;
			}
			else{
				first_cross_point = num2;
				second_cross_point = num1;
			}
			while(first_cross_point <= second_cross_point){
				cross_num1 = tmp_gene[i][first_cross_point];
				cross_num2 = tmp_gene[i+1][second_cross_point];
				for(j=2;j<ga_population.gene_length-1;j++){
					if(tmp_gene[i][j] == cross_num1){
						tmp_gene[i][j] = cross_num2;
					}
					else if(tmp_gene[i][j] == cross_num2){
						tmp_gene[i][j] = cross_num1;
					}
				}
				for(j=2;j<ga_population.gene_length-1;j++){
					if(tmp_gene[i+1][j] == cross_num1){
						tmp_gene[i+1][j] = cross_num2;
					}
					else if(tmp_gene[i+1][j] == cross_num2){
						tmp_gene[i+1][j] = cross_num1;
					}
				}
				first_cross_point++;
			}
		}
	}
	for(i=0;i<IND_NUM;i++){
		for(j=1;j<ga_population.gene_length;j++){
			ga_population.individual[i].gene[j] = tmp_gene[i][j];
		}
	}
	return ga_population;
}

/* 通常の突然変異法 */
static struct ga_population_t normalMutation(struct ga_population_t ga_population){
	double mutation_ran;
	int i, j, num, temp;
	
	for(i=0;i<IND_NUM;i++){
		for(j=2;j<ga_population.gene_length-1;j++){
			mutation_ran = random()%1000+1;
			if(mutation_ran <= mutation_rate){
				num = random()%(ga_population.gene_length-3)+2;
				temp = ga_population.individual[i].gene[j];
				ga_population.individual[i].gene[j] = ga_population.individual[i].gene[num];
				ga_population.individual[i].gene[num] = temp;
			}
		}
	}
	return ga_population;
}

/* 以下，西日特化突然変異関係 */
static int *getDropNodeNum(int *drop_node_num, struct drop_node_t *drop_node){
	int i = 1;
	while(drop_node != NULL){
		drop_node_num[i] = drop_node->num;
		drop_node = drop_node->next;
		i++;
	}
	return drop_node_num;
}

/* 
参照遺伝子座までの到着時刻を求める 
例）1 3 5 6 2   参照遺伝子座が３とすると，
1->3->5への到着時刻を求める
*/
static double getPassingTime(struct individual_t individual, int bib_genetic_locus, struct map_data_t map_data){
	struct route_data_t part_of_route;
	double move_min = individual.wait_time;
	int i;
	
	for(i=1;i<bib_genetic_locus;i++){
		part_of_route = serch_route(individual.gene[i], individual.gene[i+1], map_data, move_min);
		move_min += part_of_route.time;
	}
	return fmod(start_hour + (START_MIN + move_min) / 60, 24.0f);
}

/* DijkstraDBからbib_gene，passing_timeに対応したでーたの取り出し */
static struct estimate_uncom_data_t *selectDijkstraUncomDB(double passing_time, int gene_length, int bib_gene, int *drop_node_num, struct estimate_uncom_data_t *estimate_uncom_array){
	int i, j, k;
	//struct estimate_uncom_data_t *estimate_uncom_array;
	for(i=1;i<DATABASE_SIZE;i++){
		if(passing_time <= dijkstra_uncom_db[i].time){
			for(j=1;j<gene_length;j++){
				if(bib_gene == drop_node_num[j]){
					break;
				}
			}
			for(k=1;k<gene_length;k++){
				estimate_uncom_array[k].uncom = dijkstra_uncom_db[i-1].matrix[j][k];
				estimate_uncom_array[k].node = drop_node_num[k];
			}
			break;
		}
	}
	return estimate_uncom_array;
}

/* inverse parameterによって推定不快度をチューニングするよ */
static double *getEstimateTuningUncom(struct estimate_uncom_data_t *estimate_uncom_array, double *estimate_tuning_uncom, int gene_length){
	int i;
	for(i=1;i<gene_length;i++){
		if(estimate_uncom_array[i].uncom != NO_EXIST){
			estimate_tuning_uncom[i] = pow(estimate_uncom_array[i].uncom, inverse_parameter);
		}
		else{
			estimate_tuning_uncom[i] = NO_EXIST;
		}
	}
	return estimate_tuning_uncom;
}

static double getSumTuningUncom(int gene_length, double *estimate_tuning_uncom){
	int i;
	double sum_tuning_uncom = 0.0f;
	for(i=2;i<gene_length-1;i++){
		if(estimate_tuning_uncom[i] != NO_EXIST){
			sum_tuning_uncom += estimate_tuning_uncom[i];
		}
	}
	return sum_tuning_uncom;
}

static double *getEstimateDev(double *dev, double *uncom, double avg_uncom, int gene_length){
	int i;
	for(i=1;i<gene_length;i++){
		dev[i] = 0.0f;
	}
	for(i=2;i<gene_length;i++){
		if(uncom[i] != NO_EXIST){
			dev[i] = estimate_uncom_parameter * (uncom[i] - avg_uncom);
		}
		else{
			dev[i] = NO_EXIST;
		}
	}
	return dev;
}

static double getDevRatio(double dev, double sum_reciprocal_uncom){
	return dev / sum_reciprocal_uncom * 100;
}

static double *getEstimateRatio(double *estimate_ratio, int gene_length, double *estimate_dev, double sum_uncom){
	int i;
	double dev_ratio;
	
	for(i=1;i<gene_length;i++){
		estimate_ratio[i] = 0.0f;
	}
	for(i=2;i<gene_length-1;i++){
		if(estimate_dev[i] != NO_EXIST){
			estimate_ratio[i] = 100.0f / (gene_length - 4);
		}
	}
	for(i=2;i<gene_length-1;i++){
		if(estimate_dev[i] != NO_EXIST){
			dev_ratio = getDevRatio(estimate_dev[i], sum_uncom);
			estimate_ratio[i] += dev_ratio;
		}
	}
	return estimate_ratio;
}

static double getSumRatio(double *ratio, int gene_length){
	int i;
	double sum_ratio = 0.0f;
	for(i=2;i<gene_length;i++){
		sum_ratio += ratio[i];
	}
	return sum_ratio;
}

static int getMutateGene(int gene_length, double *estimate_ratio, double ratio_ran, struct estimate_uncom_data_t *estimate_array){
	double specific_mutation_rate = 0.0f;
	int mutate_gene;
	int i;
	for(i=2;i<gene_length;i++){
		specific_mutation_rate += estimate_ratio[i];
		if(specific_mutation_rate > ratio_ran){
			mutate_gene = estimate_array[i].node;
			break;
		}
	}
	return mutate_gene;
}

static struct individual_t swapGene(int temp_gene, int mutate_gene, int gene_length, struct individual_t individual){
	int i, num1, num2, temp;
	
	//printIndividual(individual, gene_length);
	//printf("temp_gene = %d, mutate_gene = %d\n", temp_gene, mutate_gene);
	if(temp_gene != mutate_gene){
		for(i=1;i<gene_length;i++){		
			if(individual.gene[i] == temp_gene){
				num1 = i;
				//individual.gene[i] = mutate_gene;
			}
			else if(individual.gene[i] == mutate_gene){
				//individual.gene[i] = temp_gene;
				num2 = i;
				//printf("%d = %d\n", num2, i);
			}
		}
		//printf("before\n");
		//printIndividual(individual, gene_length);
		temp = individual.gene[num1];
		//printf("temp = %d\n", temp);
		//printf("num1 = %d, num2 = %d\n", num1, num2);
		//printf("%d <- %d\n", individual.gene[num1], individual.gene[num2]);
		individual.gene[num1] = individual.gene[num2];
		individual.gene[num2] = temp;
		//printf("after\n");
		//printIndividual(individual, gene_length);
	}
	return individual;
}


static struct ga_population_t specificWesteringSunMutation(struct ga_population_t ga_population, struct map_data_t map_data, struct drop_node_t *drop_node_head){
	int *drop_node_num = (int *)malloc(sizeof(int) * ga_population.gene_length);
	int mutate_gene;
	double mutation_ran;
	double passing_time;  //待ち時間＋参照遺伝子座までの移動時間
	int i, j;
	struct estimate_uncom_data_t *estimate_uncom_array = (struct estimate_uncom_data_t *)malloc(sizeof(struct estimate_uncom_data_t) * ga_population.gene_length);
	double sum_tuning_uncom, avg_tuning_uncom, sum_ratio, ratio_ran;
	double *estimate_tuning_uncom = (double *)malloc(sizeof(double) * ga_population.gene_length);
	double *estimate_dev = (double *)malloc(sizeof(double) * ga_population.gene_length);
	double *estimate_ratio = (double *)malloc(sizeof(double) * ga_population.gene_length);
	
	for(i=0;i<IND_NUM;i++){
		for(j=1;j<ga_population.gene_length-2;j++){
			mutation_ran = random()%1000+1;
			if(mutation_ran <= mutation_rate){
				drop_node_num = getDropNodeNum(drop_node_num, drop_node_head);
				passing_time = getPassingTime(ga_population.individual[i], j, map_data);
				estimate_uncom_array = selectDijkstraUncomDB(passing_time, ga_population.gene_length, ga_population.individual[i].gene[j], drop_node_num, estimate_uncom_array);
				/*
				変異確率の割り当て
				ただし，始発点，終着点への変異比率は０に設定
				*/
				estimate_tuning_uncom = getEstimateTuningUncom(estimate_uncom_array, estimate_tuning_uncom, ga_population.gene_length);
				sum_tuning_uncom = getSumTuningUncom(ga_population.gene_length, estimate_tuning_uncom);
				avg_tuning_uncom = sum_tuning_uncom / (ga_population.gene_length - 4);
				/*　各立ち寄り地間の推定不快度の標準偏差を求める　*/
				estimate_dev = getEstimateDev(estimate_dev, estimate_tuning_uncom, avg_tuning_uncom, ga_population.gene_length);
				/* 各立ち寄り地間の変異比率を求める */
				estimate_ratio = getEstimateRatio(estimate_ratio, ga_population.gene_length, estimate_dev, sum_tuning_uncom);				
				/* 突然変異比率に基づいて突然変異 */
				sum_ratio = getSumRatio(estimate_ratio, ga_population.gene_length);
				/* ratioを百分率に整える */
				/*
				for(k=2;k<ga_population.gene_length;k++){
					//printf("estimate_ratio[%d] = %f/%f * 100\n", k, estimate_ratio[k], sum_ratio);
					estimate_ratio[k] = estimate_ratio[k] / sum_ratio * 100;
				}
				*/
				/*
				int k;
				printf("*************************\n");
				for(k=1;k<ga_population.gene_length;k++)
					printf("|%3d", estimate_uncom_array[k].node);
				printf("|\n");
				for(k=1;k<ga_population.gene_length;k++)
					printf("|%3.1f", estimate_tuning_uncom[k]);
				printf("|\n");
				for(k=1;k<ga_population.gene_length;k++)
					printf("|%3.1f", estimate_ratio[k]);
				printf("|\n\n");
				*/
				ratio_ran = random()%(int)sum_ratio;
				
				mutate_gene = getMutateGene(ga_population.gene_length, estimate_ratio, ratio_ran, estimate_uncom_array);
				//printf("before swap ind_num:%d\n", i);
				//printf("%d <-> %d\n", ga_population.individual[i].gene[j+1], mutate_gene);
				ga_population.individual[i] = swapGene(ga_population.individual[i].gene[j+1], mutate_gene, ga_population.gene_length, ga_population.individual[i]);
				//printIndividual(ga_population.individual[i], ga_population.gene_length);
				//printf("\n");
				//printf("*************************\n");
				//printf("after swap\n");
				//printIndividual(ga_population.elite_individual, ga_population.gene_length);
				//printf("\n");
				break;
			}
		}
	}
	free(drop_node_num);
	free(estimate_uncom_array);
	free(estimate_tuning_uncom);
	free(estimate_dev);
	free(estimate_ratio);
	return ga_population;
}
								
/* 
突然変異法の選択
MUTATION_METHOD == 0 -> 通常の突然変異
MUTATION_METHOD == 1 -> 西日特化突然変異
 */
static struct ga_population_t mutation(struct ga_population_t ga_population, struct map_data_t map_data, struct drop_node_t *drop_node_head){
	switch(MUTATION_METHOD){
		case 0:
			ga_population = normalMutation(ga_population);
			break;
		case 1:
			ga_population = specificWesteringSunMutation(ga_population, map_data, drop_node_head);
			break;
		default:
			printf("invailed number on MUTATION_METHOD\n");
			exit(1);
	}
	return ga_population;
}

/* 待ち時間突然変異 */
static struct ga_population_t waitTimeMutation(struct ga_population_t ga_population){
	int i;
	double wait_time_mutation_ran;
	for(i=0;i<IND_NUM;i++){
		wait_time_mutation_ran = random()%1000+1;
		if(wait_time_mutation_ran < wait_time_mutation_rate){
			ga_population.individual[i].wait_time = random()%MAX_WAIT_TIME;
		}
	}
	return ga_population;
}

struct ga_population_t saveElite(struct ga_population_t ga_population){
	int i, j;
	for(i=0;i<IND_NUM;i++){
		if(ga_population.elite_individual.fitness > ga_population.individual[i].fitness){
			for(j=1;j<ga_population.gene_length;j++)
				ga_population.elite_individual.gene[j] = ga_population.individual[i].gene[j];
			ga_population.elite_individual.wait_time = ga_population.individual[i].wait_time;
			ga_population.elite_individual.fitness = ga_population.individual[i].fitness;
			//printf("\nupdate!!!!!!!!!!\n\n");
		}
	}
	return ga_population;
}

/*
個体番号が一番後ろの個体にエリート個体を挿入する
*/
void insertElite(struct ga_population_t* ga_population){

	copyIndividual(&ga_population->individual[IND_NUM-1], 
						  &ga_population->elite_individual, 
						  ga_population->gene_length);
						  
	return;
}

	
struct ga_population_t generatePopulation(struct ga_population_t ga_population_parent, struct map_data_t map_data, struct drop_node_t *drop_node_head){
	selection(&ga_population_parent);
	insertElite(&ga_population_parent);
	crossover(&ga_population_parent);
	ga_population_parent = mutation(ga_population_parent, map_data, drop_node_head);
	ga_population_parent = waitTimeMutation(ga_population_parent);
	ga_population_parent = evaluation(ga_population_parent, map_data);
	ga_population_parent = saveElite(ga_population_parent);
	
	return ga_population_parent;
}