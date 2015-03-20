#ifndef Serch_Route_C
#define Serch_Route_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"header.h"
#include"param.h"

struct route_data_t serch_route(int start, int goal, struct map_data_t map_data, double departure_time){
	struct sun_angle_t sun_angle;
	struct route_data_t route_data;
	double min_uncom, uncom[map_data.node_size], time[map_data.node_size];
	int flag[map_data.node_size];
	double arrival_time;
	double tmp_uncom;
	double departure_hour, departure_min;
	int i, j, p;
	
	departure_hour = start_hour + floor(START_MIN + departure_time / 60.0f);
	departure_min =  fmod(START_MIN + departure_time, 60.0f);
	
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
			printf("Cannnot arrival goal\n");
			printf("%d -> %d\n", start, goal);
			exit(1);
		}
		for(j=1;j<map_data.node_size;j++){
			if(map_data.matrix_trav_time[p][j] != INF){
				arrival_time = ((time[p] + map_data.matrix_trav_time[p][j] + departure_min) / 60.0) + departure_hour;
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
	route_data.uncom = uncom[goal];
	route_data.time = time[goal];
	return route_data;
}

#endif