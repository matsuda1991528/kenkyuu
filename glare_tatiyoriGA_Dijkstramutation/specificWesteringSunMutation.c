#ifndef SpecificWesteringSunMutation_C
#define SpecificWesteringSunMutation_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"

double getMutateRatio(double, double);
double getDevRatio(double, double);

void specificWesteringSunMutation(){
	//double mutation_rate = MUTATION_RATE * 1000;
	double mutation_ran;
	double moveMin;
	double passingTime; //待ち時間＋突然変異点までの移動時間[h]
	//double arriveHour;  //arrival time from start point to mutate point[h]
	struct routeData partOfRoute;
	int i, j, k, l, m;
	struct estimateUncomData{
		double ratio;
		double dev;
		double uncom;
		int node;
	};
	struct estimateUncomData estimateUncomArray[GENE_SIZE];
	double sumEstimateUncom;
	double avgEstimateUncom;
	double sumRatio;
	double ratioRan;
	double sumDev;
	double specific_mutation_rate;
	double devRatio;
	int temp;
	

	
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE-2;j++){
			mutation_ran = random()%1000+1;
			if(mutation_ran <= mutation_rate){  //突然変異確率に基づいて突然変異の実行
				////printf("\n");
				////for(k=1;k<GENE_SIZE;k++){	
				////	printf("|%d|", individual[i].gene[k]);
				////}
				////printf("\n");
				////printf("mutate point -> %d\n", individual[i].gene[j]);
				//calculate the time when passing mutate point
				moveMin = individual[i].waitTime;
				for(k=1;k<j;k++){
					partOfRoute = serch_route(individual[i].gene[k], individual[i].gene[k+1], moveMin);
					moveMin += partOfRoute.time;   //moveMinへ突然変異点通過までの所要時間を入れる
				}
				passingTime = START_HOUR + (START_MIN + moveMin) / 60;   //passingTimeへ突然変異点の通過予定時刻を入れる
				//printf("passingTime = %f\n", passingTime);
				/* 通過時刻，突然変異点に対する不快度推定値データの取得 */
				for(k=1;k<DATA_BASE_SIZE;k++){
					if(passingTime <= estimateUncom[k].time){
						//printf("%f -> estimateUncom[%d].time = %f\n", passingTime, k-1, estimateUncom[k-1].time);
						for(m=1;m<GENE_SIZE;m++){
							if(individual[i].gene[j] == dropPoint[m].num){
								//printf("individual[%d][%d] = %d, dropPoint[%d].num = %d\n", i, j, individual[i].gene[j], m, dropPoint[m].num);
								break;
							}
						}
						//参照遺伝子から各立ち寄り地への推定値が格納されたデータをslect
						for(l=1;l<GENE_SIZE;l++){
							estimateUncomArray[l].uncom = estimateUncom[k-1].matrix[m][l];
							estimateUncomArray[l].node = dropPoint[l].num;
						}
						break;
					}
				}
/*
				for(k=1;k<GENE_SIZE;k++){
					printf("|%6d|", estimateUncomArray[k].node);
				}
				printf("\n");
				for(k=1;k<GENE_SIZE;k++){
					printf("|%6.1f|", estimateUncomArray[k].uncom);
				}
				printf("\n\n");
*/
				/* 変異比率の割り当て */
				/* ただし，始発点，終着点への変異比率は0に設定 */
				/* initialize */
				for(k=1;k<GENE_SIZE;k++){
					estimateUncomArray[k].ratio = 0.0f;
				}
				/* calculation sum of estimate uncom */
				sumEstimateUncom = 0.0f;
				avgEstimateUncom = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					//printf("estUncomArray[%d] = %f\n", k, estimateUncomArray[k].uncom);
					if(estimateUncomArray[k].uncom != FALSE){
						sumEstimateUncom += 1 / estimateUncomArray[k].uncom;
						//printf("sumEst:%f += %f\n\n",sumEstimateUncom,  1/estimateUncomArray[k].uncom);
					}
				}
				
				avgEstimateUncom = sumEstimateUncom / (GENE_SIZE - 4);
				//printf("%f = %f / %d\n", avgEstimateUncom, sumEstimateUncom, GENE_SIZE-4);

			////////////////////////////////////////////////////////////	///
            ///* 各立ち寄り地間の推定不快度の標準偏差を求める *///
			//////////////////////////////////////////////////////////////
				for(k=1;k<GENE_SIZE;k++){
					estimateUncomArray[k].dev = 0.0f;
				}
				for(k=2;k<GENE_SIZE-1;k++){
					if(estimateUncomArray[k].uncom != FALSE){
						estimateUncomArray[k].dev = estimateUncomParameter * (1 / estimateUncomArray[k].uncom - avgEstimateUncom);
						//printf("%f = %f * (1 / %f - %f)\n", estimateUncomArray[k].dev, estimateUncomParameter, estimateUncomArray[k].uncom, avgEstimateUncom);
					}
				}

						
				////printf("sumEstimateUncom = %f\n", sumEstimateUncom);
				//for(k=2;k<GENE_SIZE-1;k++){
				//	if(estimateUncomArray[k].uncom != FALSE){
				//		estimateUncomArray[k].ratio = getMutateRatio(estimateUncomArray[k].uncom, sumEstimateUncom);
				//		//printf("%f, ",estimateUncomArray[k].ratio);
				//	}
				//}
				//printf("\n");
				
			////////////////////////////////////////////////////////////	///
            ///       * /各立ち寄り地間の変異比率を求める          *///
			//////////////////////////////////////////////////////////////
				for(k=2;k<GENE_SIZE-1;k++){
					if(estimateUncomArray[k].uncom != FALSE)
						estimateUncomArray[k].ratio = 100.0f / (GENE_SIZE - 4);
				}
				for(k=2;k<GENE_SIZE-1;k++){
					if(estimateUncomArray[k].uncom != FALSE){
						devRatio = getDevRatio(estimateUncomArray[k].dev, sumEstimateUncom);
						estimateUncomArray[k].ratio += devRatio;
					}
				}
				//for(k=2;k<GENE_SIZE-1;k++){
				//	if(estimateUncomArray[k].ratio <= 1){
				//		estimateUncomArray[k].ratio = 1;
				//	}
				//}
/*
				for(k=1;k<GENE_SIZE;k++){
					printf("|%7d|", estimateUncomArray[k].node);
				}
				printf("\n");
				for(k=1;k<GENE_SIZE;k++){
					printf("|%7.1f|", estimateUncomArray[k].uncom);
				}
				printf("\n");
				for(k=1;k<GENE_SIZE;k++){
					printf("|%6.5f|", estimateUncomArray[k].dev);
				}
				printf("\n");
				for(k=1;k<GENE_SIZE;k++){
					printf("|%7.1f|", estimateUncomArray[k].ratio);
				}
				printf("\n\n");
*/				
				/* 変異比率に基づいて突然変異 */
				sumRatio = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					sumRatio += estimateUncomArray[k].ratio;
				}
				////printf("sumRatio = %f\n", sumRatio);
				for(k=1;k<GENE_SIZE;k++){
					estimateUncomArray[k].ratio = estimateUncomArray[k].ratio / sumRatio * 100;
				}
				////for(k=1;k<GENE_SIZE;k++){
				////	printf("|%7.1f|", estimateUncomArray[k].ratio);
				////}
				////printf("\n\n");
				sumRatio = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					sumRatio += estimateUncomArray[k].ratio;
				}
				////printf("sumRatio = %f\n", sumRatio);
				ratioRan = random()%(int)sumRatio;
				////printf("ratioRan = %f\n", ratioRan);
				specific_mutation_rate = 0.0f;
				for(k=2;k<GENE_SIZE-1;k++){
					specific_mutation_rate += estimateUncomArray[k].ratio;
					if(ratioRan < specific_mutation_rate){
						temp = individual[i].gene[j+1];
						//printf("temp = %d,estimateUncomnode = %d\n", temp, estimateUncomArray[k].node);
						for(l=1;l<GENE_SIZE;l++){
							if(individual[i].gene[l] == temp){
								individual[i].gene[l] = estimateUncomArray[k].node;
							}
							else if(individual[i].gene[l] == estimateUncomArray[k].node){
								individual[i].gene[l] = temp;
							}
						}
						//individual[i].gene[j+1] = estimateUncomArray[k].node;
						//for(l=2;l<GENE_SIZE-1;l++){
						//	if(temp == individual[i].gene[l]){
						//		individual[i].gene[l] = temp;
						//	}
						//	break;
						break;
					}
				}
/*
				printf("after mutation\n");
				for(k=1;k<GENE_SIZE;k++){
					printf("|%d|", individual[i].gene[k]);
				}
				printf("\n\n");
*/
			}
		}
	}
	//printf("a\n");
	//printf("\n\n");
}

double getMutateRatio(double uncom, double sumReciprocalUncom){
	double reciprocalOfUncom; //不快度の逆数
	reciprocalOfUncom = 1 / uncom;
	return reciprocalOfUncom / sumReciprocalUncom * 100;
}

double getDevRatio(double dev, double sumReciprocalUncom){
	return dev / sumReciprocalUncom * 100;
}
#endif