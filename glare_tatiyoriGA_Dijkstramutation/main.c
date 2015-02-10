#include<stdio.h>
#include<time.h>
#include"extern.h"
#include"ga.h"


int main(int argc, char **argv){
	int biasMutation_rate;
	int biasMutation_ran;
	while(--argc > 0 && **(++argv) == '-'){
		argc--;
		if(strcmp(*argv, "-seed") == 0){
			seed = atoi(*++argv);  //コンソール上で入力したseed値をchar型からint型へ変換
		}
		else if(strcmp(*argv, "-cross") == 0){
			crossover_rate = atof(*++argv);  //コンソール上で入力したcrossover_rateをchar型からdouble型へ変換
		}
		else if(strcmp(*argv, "-mutate") == 0){  ////コンソール上で入力したmutation_rateをchar型からdouble型へ変換
			mutation_rate = atof(*++argv);
		}
		else if(strcmp(*argv, "-estimate") == 0){
			estimateUncomParameter = atof(*++argv);  //コンソール上で入力したestimateUncomParameterをchar型からdouble型へ変換
		}
		else if(strcmp(*argv, "-bias") == 0){
			biasMutation_rate = atoi(*++argv);
		}
		else{
			printf("invaild Command!!\n");
			exit(-1);
		}
	}
//	clock_t start, end;
//	start = clock();
	crossover_rate = crossover_rate * 100;
	mutation_rate = mutation_rate * 1000;
	//printf("%f\n", estimateUncomParameter);
	srandom(seed);
	//FILE *fp;
	//char *fname = "resultOfExperiment/specificWesteringSunlMutation_sample.csv";
	//char *fname = "resultOfExperiment/normalMutation.csv";
	//fp = fopen(fname, "w");
	//if(fp == NULL){
	//	printf("%s open error\n", fname);
	//	exit(1);
	//}
	int cnt, i;

	//for(i=0;i<=GENERATION;i++){
	//	fprintf(fp,"%d,", i);
	//}
	//fprintf(fp,"\n");
	//for(i=0;i<=GENERATION;i++){
	//	printf("%d,", i);
	//}
	//printf("\n");

//	for(seed=0;seed<10;seed++){
	eliteIndividual.uncomValue = INF;
	getMapData();
	getGene();
	setUncomEstimateData();
	valuation();
	eliteSave();
//	for(cnt=0;cnt<GENERATION;cnt++){
	cnt = 0;
//	seed = 0;

	while(1){
		cnt++;
		//printf("cnt = %d\n", cnt);
		selection();
		//printGene();
		insertElite();
		crossover();
		biasMutation_ran = random()%100;
//		if(biasMutation_rate < biasMutation_ran){
//			mutation();
//		}
//		else{
//			specificWesteringSunMutation();
//		}
		specificWesteringSunMutation();
		waitTimeMutation();
		valuation();
		eliteSave();
		//fprintf(fp, "%f,", eliteIndividual.uncomValue);

		//printf("%f,", eliteIndividual.uncomValue);
		if(cnt > GENERATION)
			break;
	}
	//fprintf(fp,"\n");
	//printf("\n");
//	}
//	end = clock();
	//fclose(fp);
	printf("%f\n", eliteIndividual.uncomValue);
//	printElite();
//	printf("%d\n", cnt);
//	printf("%d\n", end - start);
	return 0;
}

	