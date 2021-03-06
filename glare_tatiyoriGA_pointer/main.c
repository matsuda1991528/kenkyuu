#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"
#include"param.h"
#include<time.h>


int main(int argc, char **argv){
	crossover_rate = 0.4;
	mutation_rate = 0.1;
	wait_time_mutation_rate = 0.01;
	estimate_uncom_parameter = 1;
	inverse_parameter = -9;
	seed = 0;
	
	while(--argc > 0 && **(++argv) == '-'){
		argc--;
		if(strcmp(*argv, "-seed") == 0){
			seed = atoi(*++argv);
		}
		else if(strcmp(*argv, "-inverse") == 0){
			inverse_parameter = atof(*++argv);
		}
		else if(strcmp(*argv, "-start_hour") == 0){
			start_hour = atoi(*++argv);
		}
		else{
			printf("invailed command\n");
			exit(1);
		}
	}
	
	crossover_rate = crossover_rate * 100;
	mutation_rate = mutation_rate * 1000;
	srandom(seed);
	
	struct map_data_t map_data = getMapData();
	struct drop_node_t *drop_node_head;
	struct ga_population_t ga_population_parent, ga_population_child; 
	int generation = 0;

	/* 各立ち寄り地の座標情報を取得する */
	drop_node_head = getDropNode(DROP_NODE_NUM, map_data.node_size);
	
	/* 立ち寄り地数(=遺伝子長)を取得する */
	ga_population_child.gene_length = ga_population_parent.gene_length
	= setGeneLength(drop_node_head);
	
	/* 各時刻における各立ち寄り地間のdijkstra不快度のデータベース作成 */
	dijkstra_uncom_db
	= setEstimateUncomDB(drop_node_head, ga_population_parent.gene_length, map_data);
	
	/* 
	メモリ確保の際はheadを返さないといけない．
	多分ga_population_t は構造体と認識されるのだろう
	*/
	gene_malloc(&ga_population_parent);
	gene_malloc(&ga_population_child);
	
	/* 最初の解候補集団をランダムに生成 */
	ga_population_parent = getPopulation(ga_population_parent, drop_node_head);
	
	/* 解候補集団の適応度を評価する */
	evaluation(&ga_population_parent, map_data);
	
	/* 一番最初のエリートの不快度値を挿入する */
	ga_population_parent.elite_individual.fitness = INITIAL_SET_FITNESS;
	
	/* エリート保存 */
	saveElite(&ga_population_parent);
	
	/* generationの回数分，解候補は進化を繰り返す */
	while(generation < GENERATION){
		ga_population_child
		= generatePopulation(ga_population_parent, map_data, drop_node_head);
		
		ga_population_parent = ga_population_child;
		
		generation++;
		//printf("%f,", ga_population_parent.elite_individual.fitness);
		/*
		printf("%d\n", ga_population_parent.elite_individual.wait_time);
		printIndividual(ga_population_parent.elite_individual, ga_population_parent.gene_length);
		printf("*******************************\n");
		*/
	}
	printf("%f\n", ga_population_parent.elite_individual.fitness);
/*
	printf("final\n");
	printf("fitness = %f\n", ga_population_parent.elite_individual.fitness);
	printf("%d\n", ga_population_parent.elite_individual.wait_time);
	printIndividual(ga_population_parent.elite_individual, ga_population_parent.gene_length);	
	printRoute(ga_population_parent.elite_individual, ga_population_parent.gene_length, map_data);
*/
	//printf("gene time = %f\n", (double)((end - rap) / 1000));
	return 0;
}