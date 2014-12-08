#ifndef SetEstimateData_C
#define SetEstimateData_C
#include<stdio.h>
#include<stdlib.h>
#include"extern.h"

void setUncomEstimateData(dropPointData dropPoint[GENE_SIZE]){
	struct node_data dropPointData[GENE_SIZE];
	double timeEstimateMatrix[GENE_SIZE][GENE_SIZE];
	double angleEstimateMatrix[GENE_SIZE][GENE_SIZE];
	double uncomEstimateMatrix[24][GENE_SIZE][GENE_SIZE];
	double estimateDistance;
	double estimateRadian;
	double time;
	int i, j;
	
	//drop point node number is entered into dropPointData[i] 
	for(i=1;i<GENE_SIZE;i++){
		dropPointData[i].num = dropPoint[i].num;
	}
	//enter coord of node corresponds to node number
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<NODE_NUM;j++){
			if(dropPointData[i].num == node[j].num){
				dropPointData[i].pos.x = node[j].pos.x;
				dropPointData[i].pos.y = node[j].pos.y;
			}
		}
	}
	
	//calclation of each drop point's move time 
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(node[dropPointData[i].num].num == node[dropPointData[j]].num){
				timeEstimateMatrix[i][j] = INF;
			}
			else{
				estimateDistance = getDistance(node[dropPointData[i].num].pos, node[dropPointData[j].num].pos);
				timeEstimateMatrix[i][j] = estimateDistance / MOVE_SPEED;
			}
		}
	}
	//calculation of each drop point's angle
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(node[dropPointData[i].num].num == node[dropPointData[j].num].num){
				angleEstimateMatrix[i][j] = INF;
			}
			else{
				angleEstimateMatrix[i][j] = getAngle(node[dropPointData[i].num].pos, node[dropPointData[j].num].pos);
			}
		}
	}
	
	for(time=1;time<24;time++){
		getSunAngle(time);
		for(i=1;i<GENE_SIZE;i++){
			for(j=1;j<GENE_SIZE;j++){
				if(node[dropPointData[i].num].num == node[dropPointData[j].num].num){
					uncomEstimateMatrix[time][i][j] = INF;
				}
				else{
					uncomEstimateMatrix[time][i][j] = getUncom(dropPointData[i].num, dropPointData[j].num, elevation, azimuth);
				}
			}
		}
	}
	
	for(time=1;time<24;time++){
		printf("time = %d\n", time);
		printf("     ");
		for(i=1;i<GENE_SIZE;i++){
			printf("%5d", dropPointData[i].num);
		}
		printf("\n");
		for(i=1;i<GENE_SIZE;i++){
			printf("%5d", dropPointData[i].num);
			for(j=1;j<GENE_SIZE;j++){
				printf("%5d", uncomEstimateMatrix[time][i][j]);
			}
			printf("\n");
		}
	}
			
			
}


double getDistance(struct xy_data currentPos, struct xy_data nextPos){
	double distance;
	double diff_x, diff_y;
	diff_x = nextPos.x - currentPos.x;
	diff_y = nextPos.y - currentPos.y;
	distance = sqrt(diff_x * diff_x + diff_y * diff_y);
	return distance;
}
			
double getAngle(struct xy_data currentPos, struct xy_data nextPos){
	double radian, angle;
	double diff_x, diff_y;
	diff_x = nextPos.x - currentPos.x;
	diff_y = nextPos.y - currentPos.y;
	radian = atan2(diff_y, diff_x);
	angle = radian * 180.0 / M_PI;
	return angle;
}