#ifndef SetEstimateData_C
#define SetEstimateData_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
//#include"glare.h"

double getDistance(struct xy_data, struct xy_data);
double getAngle(struct xy_data, struct xy_data);
static double getDijkstraUncom(int, int, double);

void setUncomEstimateData(){
	struct node_data dropPointData[GENE_SIZE];
	double timeEstimateMatrix[GENE_SIZE][GENE_SIZE];
	double angleEstimateMatrix[GENE_SIZE][GENE_SIZE];
	//double uncomEstimateMatrix[48][GENE_SIZE][GENE_SIZE];
	double estimateDistance;
	double estimateRadian;
	double time;
	int cnt, i, j;
	
	//drop point node number is entered into dropPointData[i] 
	for(i=1;i<GENE_SIZE;i++){
		dropPointData[i].num = dropPoint[i].num;
	}
	//for(i=1;i<GENE_SIZE;i++){
	//	printf("dropPoint[%d].num = %d\n", i, dropPoint[i].num);
	//}
	//enter coord of node corresponds to node number
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<NODE_NUM;j++){
			if(dropPointData[i].num == node[j].num){
				dropPointData[i].pos.x = node[j].pos.x;
				dropPointData[i].pos.y = node[j].pos.y;
			}
		}
	}
	time = 1;
	for(cnt=1;cnt<DATA_BASE_SIZE;cnt++){
		estimateUncom[cnt].time = time;
		for(i=1;i<GENE_SIZE;i++){
			for(j=1;j<GENE_SIZE;j++){
				if(dropPointData[i].num == dropPointData[j].num){
					estimateUncom[cnt].matrix[i][j] = FALSE;
				}
				else{
					estimateUncom[cnt].matrix[i][j] = getDijkstraUncom(dropPointData[i].num, dropPointData[j].num, estimateUncom[cnt].time);
				}
			}
		}
		time += 0.5;
	}
/*	
	for(cnt=1;cnt<DATA_BASE_SIZE;cnt++){
		printf("time = %f\n", estimateUncom[cnt].time);
		printf("     ");
		for(i=1;i<GENE_SIZE;i++){
			printf("%8d", dropPointData[i].num);
		}
		printf("\n");
		for(i=1;i<GENE_SIZE;i++){
			printf("%5d", dropPointData[i].num);
			for(j=1;j<GENE_SIZE;j++){
				printf("%8.1f", estimateUncom[cnt].matrix[i][j]);
			}
			printf("\n");
		}
	}
*/		
			
}

static double getDijkstraUncom(int start, int goal, double departureTime){
	double minUncom; // The minimize uncom value between each node
	double uncom[NODE_NUM], time[NODE_NUM];
	int flag[NODE_NUM], visitNode[NODE_NUM];
	double arrivalTime, travelMin, travelTimeBetween2points;
	double temp_uncom;
	double dijkstraUncom;
	int i, j, k, p;
	/* caculation of departure time[h] */
	/* initialize */
	//start node's uncom value is setted 0
	//other node's uncom value are setted INF(== yet define)
	for(i=1;i<NODE_NUM;i++){
		uncom[i] = INF;
		flag[i] = 0;
		time[i] = 0.0f;
	}
	uncom[start] = 0.0f;
	visitNode[start] = 0;
	for(i=1;i<NODE_NUM;i++){
		minUncom = INF;
		for(j=1;j<NODE_NUM;j++){
			if(flag[j] == 0 && uncom[j] < minUncom){
				p = j;
				minUncom = uncom[j];
			}
		}
		flag[p] = 1;
		if(minUncom == INF){
			printf("Cannnot arrive goal\n");
			printf("%d -> %d\n", start, goal);
			exit(1);
		}
		for(j=1;j<NODE_NUM;j++){
			if(matrix[p][j].time != INF){
				arrivalTime = ((time[p] + matrix[p][j].time) / 60) + departureTime;
				getSunAngle(arrivalTime);
				if(elevation <= 25 && elevation >= 0 && fabs(azimuth - matrix[p][j].angle) < 50.0f){
					temp_uncom = getUncom(p, j, elevation, azimuth);
					if(uncom[p] + temp_uncom < uncom[j]){
						uncom[j] = uncom[p] + temp_uncom;
						visitNode[j] = p;
						time[j] = time[p] + matrix[p][j].time;
					}
				}
				else{
					if(uncom[p] + matrix[p][j].time < uncom[j]){
						uncom[j] = uncom[p] + matrix[p][j].time;
						visitNode[j] = p;
						time[j] = time[p] + matrix[p][j].time;
					}
				}
			}
		}
		if(p == goal)
			break;
	}
	dijkstraUncom = uncom[p];
	return dijkstraUncom;
}
#endif