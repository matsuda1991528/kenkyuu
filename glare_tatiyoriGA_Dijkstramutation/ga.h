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

/* �I�� */
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

/* ����(PMX method) */
void crossover(){
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
			//printf("crossover\n");
			while(1){
				num1 = random()%(GENE_SIZE - 3) + 2;//�n���_�͌�������Ȃ��悤�ɂ���̂�I
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
						individual[i].gene[j] = crossNum1;
				}
				for(j=2;j<GENE_SIZE-1;j++){
					if(individual[i+1].gene[j] == crossNum1)
						individual[i+1].gene[j] = crossNum2;
					else if(individual[i+1].gene[j] == crossNum2)
						individual[i+1].gene[j] = crossNum1;
				}
				firstCrossPoint++;
			}
		}
	}
}

/* �ˑR�ψ� */
void mutation(){
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

/* �҂����Ԑ�p�ˑR�ψ� */
void waitTimeMutation(){
	int i;
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
	double currentTime;//���ݎ���(�����o�H���Z�o����ۂɕK�v)
	double sumUncom[INDI_NUM];
	struct routeData partOfRoute;
	
	for(i=0;i<INDI_NUM;i++){
		currentTime = individual[i].waitTime;  //�ړ����Ԃ̏����l�͑҂�����
		individual[i].uncomValue = individual[i].waitTime;       //�����l�͑҂����ԂɋN������s���x
		for(j=1;j<GENE_SIZE-1;j++){
			partOfRoute = serch_route(individual[i].gene[j], individual[i].gene[j+1], currentTime);
			//printf("b\n");
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
	int travelPointNum = 1;  //�ʉߒn�_��
	int currentTime;
	int eliteRoute[2*NODE_NUM];  //�ړ��o�H���i�[����z��
	struct routeData elitePartOfRoute;
	
	/* ������ */
	for(i=1;i<2*NODE_NUM;i++){
		eliteRoute[i] = 0;
	}
	currentTime = eliteIndividual.waitTime;
	for(i=1;i<GENE_SIZE-1;i++){
		elitePartOfRoute = serch_route(eliteIndividual.gene[i], eliteIndividual.gene[i+1], currentTime);
		for(j=1;j<NODE_NUM;j++){
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
	/* ��`�q�̕\�� */
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
	/* �o�H�̕\�� */
	for(i=1;i<2*NODE_NUM;i++){
		if(eliteRoute[i] != 0)
			printf("%d->", eliteRoute[i]);
	}
}
#endif
	
	