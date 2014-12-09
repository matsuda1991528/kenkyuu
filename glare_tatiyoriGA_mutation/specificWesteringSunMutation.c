#ifndef SpecificWesteringSunMutation_C
#define SpecificWesteringSunMutation_C
#include<stdio.h>
#include<stdlib.h>
#include"extern.h"

double getMutateRatio(double, double);

void specificWesteringSunMutation(){
	double mutation_rate = MUTATION_RATE * 1000;
	double mutation_ran;
	double moveMin;
	double passingTime; //待ち時間＋突然変異点までの移動時間[h]
	//double arriveHour;  //arrival time from start point to mutate point[h]
	struct routeData partOfRoute;
	int i, j, k, l, m;
	struct estimateUncomData{
		double ratio;
		double uncom;
		int node;
	};
	struct estimateUncomData estimateUncomArray[GENE_SIZE];
	double sumEstimateUncom;
	double sumRatio;
	double ratioRan;
	double specific_mutation_rate;
	int temp;
	

	
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE-2;j++){
			mutation_ran = random()%1000+1;
			if(mutation_ran <= mutation_rate){
				printf("\n");
				for(k=1;k<GENE_SIZE;k++){	
					printf("|%d|", individual[i].gene[k]);
				}
				printf("\n");
				printf("mutate point -> %d\n", individual[i].gene[j]);
				//calculate the time when passing mutate point
				moveMin = individual[i].waitTime;
				for(k=1;k<j;k++){
					partOfRoute = serch_route(individual[i].gene[k], individual[i].gene[k+1], moveMin);
					moveMin += partOfRoute.time;
				}
				passingTime = START_HOUR + (START_MIN + moveMin) / 60;
				/* 通過時刻，突然変異点に対する不快度推定値データの取得 */
				for(k=1;k<DATA_BASE_SIZE;k++){
					if(passingTime >= estimateUncom[k].time){
						for(m=1;m<GENE_SIZE;m++){
							if(individual[i].gene[k] == dropPoint[m].num){
								break;
							}
						}
						for(l=1;l<GENE_SIZE;l++){
							estimateUncomArray[l].uncom = estimateUncom[k].matrix[m][l];
							estimateUncomArray[l].node = dropPoint[l].num;
						}
						for(l=1;l<GENE_SIZE;l++){
							printf("|%5d|", estimateUncomArray[l].node);
						}
						printf("\n");
						for(l=1;l<GENE_SIZE;l++){
							printf("|%5.1f|", estimateUncomArray[l].uncom);
						}
						printf("\n");
						break;
					}
				}
				/* 変異比率の割り当て */
				/* ただし，始発点，終着点への変異比率は0に設定 */
				/* initialize */
				for(k=1;k<GENE_SIZE;k++){
					estimateUncomArray[k].ratio = 0.0f;
				}
				/* calculation sum of estimate uncom */
				sumEstimateUncom = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					sumEstimateUncom += estimateUncomArray[k].uncom;
				}
				for(k=2;k<GENE_SIZE-1;k++){
					estimateUncomArray[k].ratio = getMutateRatio(estimateUncomArray[k].uncom, sumEstimateUncom);
				}
				/* 変異比率に基づいて突然変異 */
				sumRatio = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					sumRatio += estimateUncomArray[k].ratio;
				}
				ratioRan = random()%(int)sumRatio;
				specific_mutation_rate = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					specific_mutation_rate += estimateUncomArray[k].ratio;
					if(ratioRan < specific_mutation_rate){
						temp = individual[i].gene[j+1];
						individual[i].gene[j+1] = estimateUncomArray[k].node;
						for(l=2;l<GENE_SIZE-1;l++){
							if(temp == individual[i].gene[l]){
								individual[i].gene[l] = temp;
							}
							break;
						}
						break;
					}
				}		
			}
		}
	}
	printf("\n\n");
}

double getMutateRatio(double uncom, double sumUncom){
	double reciprocalOfUncom; //不快度の逆数
	reciprocalOfUncom = 1 / uncom;
	return reciprocalOfUncom * sumUncom;
}
#endif							
					