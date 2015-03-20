#ifndef SetEstimateUncom_C
#define SetEstimateUncom_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"header.h"
#include"param.h"

static double getDijkstraUncom(int, int, struct map_data_t, double);

static double getDijkstraUncom(int start, int goal, struct map_data_t map_data, double departure_time){
	if(start < 1){
		printf("start = %d, goal = %d\n", start, goal);
		exit(1);
	}
	struct sun_angle_t sun_angle;
	double min_uncom, uncom[map_data.node_size], time[map_data.node_size];
	int flag[map_data.node_size];
	double arrival_time;
	double tmp_uncom;
	int i, j, p;
	
	/*
	initiation
	start node's uncom value is setted 0
	other node's uncom value is setted NO_EXIST(means yet define)
	*/
	for(i=1;i<map_data.node_size;i++){
		uncom[i] = INF;
		flag[i] = 0;
		time[i] = 0.0f;
	}
	uncom[start] = 0.0f;
	for(i=1;i<map_data.node_size;i++){
		min_uncom = INF;
		for(j=1;j<map_data.node_size;j++){
			if(flag[j] == 0 && uncom[j] < min_uncom){
				p = j;
				min_uncom = uncom[j];
			}
		}
		flag[p] = 1;
		if(min_uncom == INF){
			printf("Cannnot arrive goal\n");
			printf("%d -> %d\n", start, goal);
			exit(1);
		}
		for(j=1;j<map_data.node_size;j++){
			if(map_data.matrix_trav_time[p][j] != INF){
				arrival_time = ((time[p] + map_data.matrix_trav_time[p][j]) / 60.0) + departure_time;
				sun_angle = getSunAngle(arrival_time);
				if(sun_angle.elevation <= SUN_HIGH && sun_angle.elevation >= SUN_LOW && fabs(sun_angle.azimuth - matrix_angle[p][j]) < SUN_WIDTH){
					tmp_uncom = getGlare(p, j, sun_angle) * map_data.matrix_trav_time[p][j];
					if(uncom[p] + tmp_uncom < uncom[j]){
						uncom[j] = uncom[p] + tmp_uncom;
						time[j] = time[p] + map_data.matrix_trav_time[p][j];
					}
				}
				else{
					if(uncom[p] + map_data.matrix_trav_time[p][j] < uncom[j]){
						uncom[j] = uncom[p] + map_data.matrix_trav_time[p][j];
						time[j] = time[p] + map_data.matrix_trav_time[p][j];
					}
				}
			}
		}
		if(p == goal)
			break;
	}
	return uncom[goal];
}
				
	
	
	

struct dijkstra_uncom_data_t *setEstimateUncomDB(struct drop_node_t *drop_node, int gene_length, struct map_data_t map_data){
	int *drop_node_num = (int *)malloc(sizeof(int) * gene_length);
	int i, j, k;
	double time = 0.0f;
	struct dijkstra_uncom_data_t *dijkstra_uncom_db;
	
	/* ƒƒ‚ƒŠ—Ìˆæ‚ÌŠm•Û */
	dijkstra_uncom_db = (struct dijkstra_uncom_data_t *)malloc(sizeof(struct dijkstra_uncom_data_t) * DATABASE_SIZE);
	for(i=0;i<DATABASE_SIZE;i++){
		dijkstra_uncom_db[i].matrix = (double **)malloc(sizeof(double) * gene_length);
	}
	for(i=0;i<DATABASE_SIZE;i++){
		for(j=1;j<gene_length;j++){
			dijkstra_uncom_db[i].matrix[j] = (double *)malloc(sizeof(double) * gene_length);
		}
	}
	
	/* —§‚¿Šñ‚è’n‚ÌŠi”[‚â‚Å */
	i=1;
	while(drop_node != NULL){
		drop_node_num[i] = drop_node->num;
		drop_node  = drop_node->next;
		i++;
	}
	
	//for(i=1;i<gene_length;i++){
	//	printf("drop_node_num[%d] = %d\n", i, drop_node_num[i]);
	//}
	for(i=0;i<DATABASE_SIZE;i++){
		dijkstra_uncom_db[i].time = time;
		for(j=1;j<gene_length;j++){
			for(k=1;k<gene_length;k++){
				if(drop_node_num[j] == drop_node_num[k]){
					dijkstra_uncom_db[i].matrix[j][k] = NO_EXIST;
				}
				else{
					dijkstra_uncom_db[i].matrix[j][k] = getDijkstraUncom(drop_node_num[j], drop_node_num[k], map_data, dijkstra_uncom_db[i].time);
				}
			}
		}
		time += DATABASE_INTERVAL;
	}
/*
	for(i=0;i<DATABASE_SIZE;i++){
		printf("time = %f\n", dijkstra_uncom_db[i].time);
		for(j=1;j<gene_length;j++){
			printf("from %d\n", drop_node_num[j]);
			for(k=1;k<gene_length;k++){
				printf("%7.1f, ", dijkstra_uncom_db[i].matrix[j][k]);
			}
			printf("\n");
		}
		printf("\n");
	}
*/
	return dijkstra_uncom_db;
}
#endif