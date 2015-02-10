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
	double passingTime; //�҂����ԁ{�ˑR�ψٓ_�܂ł̈ړ�����[h]
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
			if(mutation_ran <= mutation_rate){  //�ˑR�ψيm���Ɋ�Â��ēˑR�ψق̎��s
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
					moveMin += partOfRoute.time;   //moveMin�֓ˑR�ψٓ_�ʉ߂܂ł̏��v���Ԃ�����
				}
				passingTime = START_HOUR + (START_MIN + moveMin) / 60;   //passingTime�֓ˑR�ψٓ_�̒ʉߗ\�莞��������
				//printf("passingTime = %f\n", passingTime);
				/* �ʉߎ����C�ˑR�ψٓ_�ɑ΂���s���x����l�f�[�^�̎擾 */
				for(k=1;k<DATA_BASE_SIZE;k++){
					if(passingTime <= estimateUncom[k].time){
						//printf("%f -> estimateUncom[%d].time = %f\n", passingTime, k-1, estimateUncom[k-1].time);
						for(m=1;m<GENE_SIZE;m++){
							if(individual[i].gene[j] == dropPoint[m].num){
								//printf("individual[%d][%d] = %d, dropPoint[%d].num = %d\n", i, j, individual[i].gene[j], m, dropPoint[m].num);
								break;
							}
						}
						//�Q�ƈ�`�q����e�������n�ւ̐���l���i�[���ꂽ�f�[�^��slect
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
				/* �ψٔ䗦�̊��蓖�� */
				/* �������C�n���_�C�I���_�ւ̕ψٔ䗦��0�ɐݒ� */
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
            ///* �e�������n�Ԃ̐���s���x�̕W���΍������߂� *///
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
            ///       * /�e�������n�Ԃ̕ψٔ䗦�����߂�          *///
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
				/* �ψٔ䗦�Ɋ�Â��ēˑR�ψ� */
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
	double reciprocalOfUncom; //�s���x�̋t��
	reciprocalOfUncom = 1 / uncom;
	return reciprocalOfUncom / sumReciprocalUncom * 100;
}

double getDevRatio(double dev, double sumReciprocalUncom){
	return dev / sumReciprocalUncom * 100;
}
#endif