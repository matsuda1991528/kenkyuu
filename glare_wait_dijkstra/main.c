#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "extern.h"

void update_elite(){
	int i;
	elite_uncom = new_uncom;
	elite_necessary_time = new_necessary_time;
	elite_waiting_time = waiting_time;
	for(i=1;i<NODE_NUM;i++)
		elite_route[i] = new_route[i];
}

int main(){
	int i;
	char *fname = "map1_kekka.txt";
	FILE *fp;
	
	/* initiation of elite_uncom */
	elite_uncom = INF;
	waiting_time = 0;
	getMapData();

	for(;;){
		serch_route();
		/* update elite route */
		if(elite_uncom > new_uncom)
			update_elite();		
		waiting_time++;
		if(waiting_time > elite_uncom)
			break;
	}
	
	printf("time = %lf, uncom = %lf, waitingtime = %lf\n", elite_necessary_time, elite_uncom, elite_waiting_time);
	for(i=1;i<NODE_NUM;i++){
		if(elite_route[i] != 0)
			printf("%d -> ", elite_route[i]);
	}
	
/*	fp = fopen(fname, "wt");
	if(fp == NULL){
		printf("file open error\n");
		exit(1);
	}
	else{
		for(i=1;i<N-1;i++){
			if(route[i] != 0){
				fprintf(fp,"%d,%d,%d,%d\n",
				cord[route[i]].x, cord[route[i]].y, cord[route[i+1]].x, cord[route[i+1]].y);
				printf("%d,%d,%d,%d\n",
				cord[route[i]].x, cord[route[i]].y, cord[route[i+1]].x, cord[route[i+1]].y);
			}
		}
	}
	fclose(fp);
*/				
	return 0;
}


	