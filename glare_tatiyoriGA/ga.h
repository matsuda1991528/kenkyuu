#ifndef GA_H
#define GA_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"

void printGene(){
	int i, j;
	for(i=0;i<INDI_NUM;i++){
		printf("individual[%d]\n", i);
		printf("uncomValue = %f    ", individual[i].uncomValue);
		printf("waitTime = %d\n", individual[i].waitTime);
		for(j=1;j<GENE_SIZE;j++){
			printf("%d ",individual[i].gene[j]);
		}
		printf("\n\n");
	}
}

/* 選択 */
void selection(){
	int i, j;
	int num1, num2;
	struct individualData newIndividual[INDI_NUM];
	
	for(i=0;i<INDI_NUM;i++){
		while(1){
			num1 = random()%INDI_NUM;
			num2 = random()%INDI_NUM;
			if(num1 != num2)
				break;
		}
		if(individual[num1].uncomValue < individual[num2].uncomValue){
			newIndividual[i].uncomValue = individual[num1].uncomValue;
			for(j=0;j<GENE_SIZE;j++){
				newIndividual[i].gene[j] = individual[num1].gene[j];
			}
		}
		else{
			newIndividual[i].uncomValue = individual[num2].uncomValue;
			for(j=0;j<GENE_SIZE;j++){
				newIndividual[i].gene[j] = individual[num2].gene[j];
			}
		}
	}
	for(i=0;i<INDI_NUM;i++){
		individual[i] = newIndividual[i];
	}
}

/* 交叉(PMX method) */
void crossover(){
	double crossover_rate = CROSSOVER_RATE * 100;
	double crossover_ran;
	int num1, num2;
	int firstCrossPoint, secondCrossPoint;
	int crossNum1, crossNum2;
	int i, j;
	
	for(i=0;i<INDI_NUM;i+=2){
		if(INDI_NUM%2 == 1 && i+1 == INDI_NUM)
			break;
		
		crossover_ran = random()%100+1;
		if(crossover_ran <= crossover_rate){
			while(1){
				num1 = random()%(GENE_SIZE - 3) + 2;//始発点は交叉されないようにするのよ！
				num2 = random()%(GENE_SIZE - 3) + 2;
				if(num1 != num2)
					break;
			}
			if(num1 < num2){
				firstCrossPoint = num1;
				secondCrossPoint = num2;
			}
			else{
				firstCrossPoint = num2;
				secondCrossPoint = num1;
			}
			while(firstCrossPoint <= secondCrossPoint){
				crossNum1 = individual[i].gene[firstCrossPoint];
				crossNum2 = individual[i+1].gene[secondCrossPoint];
				for(j=2;j<GENE_SIZE-1;j++){
					if(individual[i].gene[j] == crossNum1)
						individual[i].gene[j] = crossNum2;
					else if(individual[i].gene[j] == crossNum2)
						individual[i].gene[j] = crossNum2;
				}
				for(j=2;j<GENE_SIZE-1;j++){
					if(individual[i+1].gene[j] == crossNum1)
						individual[i+1].gene[j] = crossNum1;
					else if(individual[i+1].gene[j] == crossNum2)
						individual[i+1].gene[j] = crossNum2;
				}
				firstCrossPoint++;
			}
		}
	}
}

/* 突然変異 */
void mutation(){
	double mutation_rate = MUTATION_RATE * 1000;
	double mutation_ran;
	int i, j;
	int num, temp;
	for(i=0;i<INDI_NUM;i++){
		for(j=2;j<GENE_SIZE-1;j++){
			mutation_ran = random()%1000+1;
			if(mutation_ran <= mutation_rate){
				num = random()%(GENE_SIZE-3)+2;
				temp = individual[i].gene[j];
				individual[i].gene[j] = individual[i].gene[num];
				individual[i].gene[num] = temp;
			}
		}
	}
}

/* 待ち時間専用突然変異 */
void waitTimeMutation(){
	int i;
	double mutation_rate = WAITTIMEMUTATION_RATE * 1000;
	double mutation_ran;
	
	for(i=0;i<INDI_NUM;i++){
		mutation_ran = random()%1000+1;
		if(mutation_ran <= mutation_rate)
			individual[i].waitTime = random()%MAX_WAITTIME;
	}
}

void valuation(){
	int i, j, k;
	double move_hour, move_min, move_time;
	double sun_high, sun_width;
	double time;
	double currentTime;//現在時刻(部分経路を算出する際に必要)
	double sumUncom[INDI_NUM];
	struct routeData partOfRoute;
	
	for(i=0;i<INDI_NUM;i++){
		currentTime = individual[i].waitTime;  //移動時間の初期値は待ち時間
		sumUncom[i] = individual[i].waitTime;       //初期値は待ち時間に起因する不快度
		for(j=1;j<GENE_SIZE-1;j++){
			partOfRoute = serch_route(individual[i].gene[j], individual[i].gene[j+1], currentTime);
			currentTime += partOfRoute.time;
			//printf("%f\n", partOfRoute.uncom);
			individual[i].uncomValue += partOfRoute.uncom;
		}
	}
}

void eliteSave(){
	int i, j;
	for(i=0;i<INDI_NUM;i++){
		if(eliteIndividual.uncomValue > individual[i].uncomValue)
			eliteIndividual = individual[i];
	}
}

void insertElite(){
	individual[INDI_NUM-1] = eliteIndividual;
}

void printElite(){
	int i, j;
	int travelPointNum = 1;  //通過地点数
	int currentTime;
	int eliteRoute[2*nodeNum];  //移動経路を格納する配列
	struct routeData elitePartOfRoute;
	
	/* 初期化 */
	for(i=1;i<2*nodeNum;i++){
		eliteRoute[i] = 0;
	}
	currentTime = eliteIndividual.waitTime;
	for(i=1;i<GENE_SIZE-1;i++){
		elitePartOfRoute = serch_route(eliteIndividual.gene[i], eliteIndividual.gene[i+1], currentTime);
		for(j=1;j<nodeNum;j++){
			if(elitePartOfRoute.route[j] != 0){
				eliteRoute[travelPointNum] = elitePartOfRoute.route[j];
				travelPointNum++;
			}
		}
		currentTime += elitePartOfRoute.time;
		travelPointNum--;
	}
	
	printf("uncomValue = %5.2f\n", eliteIndividual.uncomValue);
//	printf("travelTime   = %5.2f\n", eliteIndividual.time);
	printf("waitTime     = %5d\n", eliteIndividual.waitTime);
	/* 遺伝子の表示 */
	for(i=1;i<GENE_SIZE;i++)
		printf(" -");
	printf("\n");
	for(i=1;i<GENE_SIZE;i++){
		printf("|%d", eliteIndividual.gene[i]);
	}
	printf("|\n");
	for(i=1;i<GENE_SIZE;i++){
		printf(" _");
	}
	printf("\n");
	/* 経路の表示 */
	for(i=1;i<2*nodeNum;i++){
		if(eliteRoute[i] != 0)
			printf("%d->", eliteRoute[i]);
	}
}
#endif
	
	