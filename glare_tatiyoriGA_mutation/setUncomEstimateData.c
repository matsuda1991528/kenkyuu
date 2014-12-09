#ifndef SetEstimateData_C
#define SetEstimateData_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
//#include"glare.h"

double getDistance(struct xy_data, struct xy_data);
double getAngle(struct xy_data, struct xy_data);

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
	
	
	//calclation of each drop point's move time 
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<GENE_SIZE;j++){
			if(node[dropPointData[i].num].num == node[dropPointData[j].num].num){
				timeEstimateMatrix[i][j] = INF;
			}
			else{
				estimateDistance = getDistance(node[dropPointData[i].num].pos, node[dropPointData[j].num].pos);
				timeEstimateMatrix[i][j] = estimateDistance / MOVE_SPEED * 60;
			}
		}
	}
	/*
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<GENE_SIZE;j++){
			printf("%7.2f", timeEstimateMatrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	*/
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
/*
	for(i=1;i<GENE_SIZE;i++){
		for(j=1;j<GENE_SIZE;j++){
			printf("%7.2f", angleEstimateMatrix[i][j]);
		}
		printf("\n");
	}
*/	
	time = 1;
	for(cnt=1;cnt<DATA_BASE_SIZE;cnt++){
		estimateUncom[cnt].time = time;
		getSunAngle(time);
		//printf("elevation = %f, azimuth = %f\n", elevation, azimuth);
		for(i=1;i<GENE_SIZE;i++){
			for(j=1;j<GENE_SIZE;j++){
				if(node[dropPointData[i].num].num == node[dropPointData[j].num].num){
					estimateUncom[cnt].matrix[i][j] = INF;
				}
				else{
					if(elevation<=25 && elevation>=0 && fabs(azimuth-angleEstimateMatrix[i][j])<50.0){ 
						estimateUncom[cnt].matrix[i][j] = getUncomEstimate(dropPointData[i].num, dropPointData[j].num, timeEstimateMatrix[i][j], angleEstimateMatrix[i][j]);
					}
					else{
						estimateUncom[cnt].matrix[i][j] = timeEstimateMatrix[i][j];
					}
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

double getUncomEstimate(int p1, int p2, double moveTimeEstimate, double edgeAngleEstimate){
	double illuminance; // ‘¾—z–@üÆ“x
	double angle;
	double equivalent_velling_luminance;
	double disability_glare;
	double path_eye_degree; //Ž‹ü‚Æ‘¾—z•ûˆÊ‚Ì‚È‚·Šp(2ŽŸŒ³)
	double degree = 0.0f;
	double radian = 0.0f;
	double time = 0.0f;
	double uncom = 0.0f;
	
	path_eye_degree = azimuth - edgeAngleEstimate;
	illuminance = getIlluminance(elevation, path_eye_degree);
	angle = get3D_angle(elevation, path_eye_degree);
	equivalent_velling_luminance = getEquivalent_velling_luminance(angle, illuminance);
	disability_glare = getDisability_glare(equivalent_velling_luminance);
//	printf("%f * %f\n", disability_glare, matrix[p1][p2].necessary_time);
	return (1 + disability_glare) * moveTimeEstimate;
}
#endif