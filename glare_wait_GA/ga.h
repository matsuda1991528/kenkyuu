#ifndef GA_H
#define GA_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
#include"glare.h"

void printGene(){
	int i, j;
	for(i=0;i<INDI_NUM;i++){
		printf("individual[%d]\n", i);
		printf("uncomValue = %f\n", individual[i].uncomValue);
		printf("travelTime = %f\n", individual[i].travelTime);
		printf("waitTime = %d\n", individual[i].waitTime);
		for(j=1;j<=individual[i].visitNum;j++){
			printf("|%d", individual[i].gene[j]);
		}
		printf("|\n\n");
	}
}

void printElite(){
	int i;
		printf("\n-----------------\n");
		printf("elite individual\n");
		printf("uncomValue = %f\n", eliteIndividual.uncomValue);
		printf("travelTime = %f\n", eliteIndividual.travelTime);
		printf("waitTime = %d\n", eliteIndividual.waitTime);
		for(i=1;i<=eliteIndividual.visitNum;i++){
			printf("|%d", eliteIndividual.gene[i]);
		}
		printf("|\n");
}

void getGene(){
	int i, j, k;
	int old_num, current_num;
	
	/* nextPosList の初期化 */
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<LIST_SIZE;j++){
			nextPosList[i][j] = 0;
		}
	}
	
	/* 各経由地に対する隣接リストの生成 */
	for(i=1;i<NODE_NUM;i++){
		k=1;
		for(j=1;j<NODE_NUM;j++){
			if(matrix[i][j].time != INF){
				nextPosList[i][k] = j;
				k++;
			}
		}
	}
	
	/* 遺伝子生成するよ～～ */
	/* 出発地の挿入 */
	for(i=0;i<INDI_NUM;i++){
		individual[i].gene[1] = node[START].num;
	}
	for(i=0;i<INDI_NUM;i++){
		old_num = START;        //old_num:参照する遺伝子番号
		j = 1;
		for(j=2;j<GENE_SIZE;j++){
			while(1){
				current_num = random()%(LIST_SIZE - 1) + 1;
				if(nextPosList[old_num][current_num] != 0)
					break;
			}
	/* old_numと辺を保持する地点current_numの挿入	*/
			individual[i].gene[j] = nextPosList[old_num][current_num];
			/* old_numの更新 */
			old_num = nextPosList[old_num][current_num];
		}
	}
	/* ゴール地の挿入 */
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(individual[i].gene[j] == node[GOAL].num){
				individual[i].gene[j+1] = node[GOAL].num;
			}
		}
	}
	for(i=0;i<INDI_NUM;i++){
		individual[i].gene[GENE_SIZE-1] = node[GOAL].num;
	}
	/* 訪問地数のカウント */
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(individual[i].gene[j] == node[GOAL].num){
				individual[i].visitNum = j;
					break;
			}
		}
	}
	
	for(i=0;i<INDI_NUM;i++){
		individual[i].waitTime = random()%MAX_WAITTIME;
	}
}



/* 選択 */
void selection(){
	int i;
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
			newIndividual[i] = individual[num1];
		}
		else{
			newIndividual[i] = individual[num2];
		}
	}
	for(i=0;i<INDI_NUM;i++){
		individual[i] = newIndividual[i];
	}
}

/* 交叉(one point crossover) */
void crossover(){
	double crossover_rate = CROSSOVER_RATE * 100;
	double crossover_ran;
	int crossPoint;
	int i, j;
	int currentGene[INDI_NUM][GENE_SIZE];
	
	for(i=0;i<INDI_NUM;i+=2){
		if(INDI_NUM%2 == 1 && i+1 == INDI_NUM){
			break;
		}
		crossover_ran = random()%100+1;
		if(crossover_ran <= crossover_rate){
			if(individual[i].visitNum < individual[i+1].visitNum){
				crossPoint = random()%(individual[i].visitNum-1)+1;
			}
			else{
				crossPoint = random()%(individual[i+1].visitNum-1)+1;
			}
			for(j=1;j<crossPoint;j++){
				currentGene[i][j] = individual[i].gene[j];
				currentGene[i+1][j] = individual[i+1].gene[j];
			}
			for(j=crossPoint;j<GENE_SIZE;j++){
				currentGene[i+1][j] = individual[i].gene[j];
				currentGene[i][j] = individual[i+1].gene[j];
			}
			for(j=1;j<GENE_SIZE;j++){
				individual[i].gene[j] = currentGene[i][j];
				individual[i+1].gene[j] = currentGene[i+1][j];
			}
		}
	}
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(individual[i].gene[j] == node[GOAL].num){
				individual[i].visitNum = j;
				break;
			}
		}
	}
}

/* 突然変異 */
void mutation(){
	double mutation_rate = MUTATION_RATE * 1000;
	double mutation_ran;
	int mutation_num;
	int i, j;
	
	for(i=0;i<INDI_NUM;i++){
		for(j=2;j<individual[i].visitNum;j++){
			mutation_ran = random()%1000+1;
			if(mutation_rate <= mutation_ran){
				while(1){
					mutation_num = random()%(LIST_SIZE-1)+1;
					if(nextPosList[j-1][mutation_num] != 0){
						break;
					}
				}
				individual[i].gene[j] = nextPosList[j-1][mutation_num];
			}
		}
	}
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(individual[i].gene[j] == node[GOAL].num){
				individual[i].visitNum = j;
				break;
			}
		}
	}
	for(i=0;i<INDI_NUM;i++){
		for(j=individual[i].visitNum;j<GENE_SIZE;j++){
			individual[i].gene[j] = node[GOAL].num;
		}
	}
}

void waitTimeMutation(){
	int i;
	double mutation_rate = WAITTIME_MUTATION_RATE * 1000;
	double mutation_ran;
	
	for(i=0;i<INDI_NUM;i++){
		mutation_ran = random()%1000+1;
		if(mutation_ran <= mutation_rate){
			individual[i].waitTime = random()%MAX_WAITTIME;
		}
	}
}

/* 引数:これまでの時刻，辺の予測移動時間 */
/* 辺の予測到着時刻(単位hour) */
static double getHour(double time, double edgeTime){
	double hour, minute;
	hour = (time + edgeTime) / 60 + START_HOUR;
	hour = floor(hour);
	minute = fmod((time + edgeTime), 60) + START_MIN;
	return hour + (minute / 60);
}

void valuation(){
	int i, j;
	double currentTime;   //現在時刻(hour)
	
	for(i=0;i<INDI_NUM;i++){
		individual[i].travelTime = individual[i].waitTime;
		individual[i].uncomValue = individual[i].waitTime;
	}
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<individual[i].visitNum;j++){
			if(matrix[individual[i].gene[j]][individual[i].gene[j+1]].time == INF){
				individual[i].uncomValue = 9999;
				break;
			}
			else{
				currentTime = getHour(individual[i].travelTime, matrix[individual[i].gene[j]][individual[i].gene[j+1]].time);
				individual[i].travelTime += matrix[individual[i].gene[j]][individual[i].gene[j+1]].time;
				getSunAngle(currentTime);
				if(elevation <= 25 && elevation >= 0 && fabs(azimuth - matrix[individual[i].gene[j]][individual[i].gene[j+1]].angle) < 50){
					individual[i].uncomValue += getUncom(individual[i].gene[j], individual[i].gene[j+1], elevation, azimuth);
				}
				else{
					individual[i].uncomValue += matrix[individual[i].gene[j]][individual[i].gene[j+1]].time;
				}
			}
		}
	}
}

void eliteSave(){
	int i;
	for(i=0;i<INDI_NUM;i++){
		if(eliteIndividual.uncomValue > individual[i].uncomValue){
			eliteIndividual = individual[i];
		}
	}
}

void insertElite(){
	individual[INDI_NUM - 1] = eliteIndividual;
}
#endif