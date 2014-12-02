#ifndef serch_route_C
#define serch_route_C

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
#include"glare.h"

static double getHour(double time, double path_time, int waiting_time){
//	double temp;
	double hour, minute;
	hour = ((time + path_time) / 60) + START_HOUR;
	hour = floor(hour);
//	temp = time + path_time;
	minute = fmod((time + path_time), 60) + START_MIN + waiting_time;
//	printf("hour = %f, minute = %f, current_time = %f, nexttime = %f\n", hour, minute, time, path_time);
	return hour + (minute / 60);
}
		

void serch_route(){
	int i, j, k, p;
	double min_uncom; //各地点間の最小距離
	double current_uncom[NODE_NUM]; //この地点への現時点で判明している不快度
	int flag[NODE_NUM];
	double current_sumtime[NODE_NUM]; //この地点への合計移動時間
	double temp_uncom;
	double temp;
	double mv_hour; //移動時間（時）
	double mv_min; //移動時間（分）
	double mv_time; //移動時間（時 + 分）
	double temp_sun_width;
	struct sun_data sun;
	
/* 初期化：スタートノードの距離を0，他のノードを未定義（INF）に設定 */
	for(i=1;i<NODE_NUM;i++){
		current_uncom[i] = INF;
		flag[i] = 0;
		route[i] = 0;
		current_sumtime[i] = 0.0f;
	}
	current_uncom[START] = waiting_time;
	v_node[START] = 0;
	
	for(i=1;i<NODE_NUM;i++){
		min_uncom = INF;
	/*　不快度最小の節点の探索　*/
	/* 最初は  p←start */		
		for(j=1;j<NODE_NUM;j++){
//			printf("uncom:%f < %f\n", current_uncom[j], min_uncom);
			if(flag[j] == 0 && current_uncom[j] < min_uncom){
				p = j;
//				printf("%d = %d\n", p, j);
				min_uncom = current_uncom[j];
			}
		}
		
		/* 最小距離の節点に確定フラグを立てる */
		flag[p] = 1;
		
		if(min_uncom == INF){
			printf("Can not arrive goal\n");
			exit(1);
		}		
		/* p(確定ノード)から伸びている辺の距離をそれぞれチェックし，
		それまでの距離最小経路より小さければ更新　*/
		
		/* uncom[p] + matrix_d[p][j] : uncom[p]と各隣接ノードの距離の和 */
		for(j=1;j<NODE_NUM;j++){
		
			/* p(確定ノード)から伸びている辺の移動時間をそれぞれチェックし，
			各々の時間での太陽高度と太陽方位のデータを取得*/
			if(matrix[p][j].time != INF){
				mv_hour = mv_min = mv_time = 0.0f;
//				printf("sumtime[%d] = %f, matrix[%d][%d] = %f ->\n", p, current_sumtime[p], p, j, matrix[p][j].necessary_time);
				mv_time = getHour(current_sumtime[p], matrix[p][j].time, waiting_time);
				getSunAngle(mv_time);
				//printf("%lf, %lf\n", elevation, azimuth);
				
				if(elevation <= 25 && elevation >=0.0f){
					//if(matrix[p][j].path_degree > 130 + azimuth || matrix[p][j].path_degree < -130 - azimuth){
					if(fabs(azimuth - matrix[p][j].angle) < 50.0f){
						temp_uncom = getUncom(p, j, elevation, azimuth);
						if((current_uncom[p] + temp_uncom) < current_uncom[j]){
							current_uncom[j] = current_uncom[p] + temp_uncom;
							v_node[j] = p;
							current_sumtime[j] = current_sumtime[p] + matrix[p][j].time;
//							printf("temp_uncom = %f, matrix = %f\n", temp_uncom, matrix[p][j].necessary_time);
						}
					}
					else{
						if(current_uncom[p] + matrix[p][j].time < current_uncom[j]){
							current_uncom[j] = current_uncom[p] + matrix[p][j].time;
							v_node[j] = p;
							current_sumtime[j] = current_sumtime[p] + matrix[p][j].time;
						}
					}
				}
				else{
					if(current_uncom[p] + matrix[p][j].time < current_uncom[j]){
						current_uncom[j] = current_uncom[p] + matrix[p][j].time;
						v_node[j] = p;
						current_sumtime[j] = current_sumtime[p] + matrix[p][j].time;
						//printf("uncom = %f, sumtime = %f\n", current_uncom[j], current_sumtime[j]);
					}
				}
			}
		}
		if(p == GOAL)
			break;
	}
//	printf("goal = %d\n", p);
//	printf("time = %5.2lf\n", current_sumtime[GOAL]);
//	printf("uncom_value = %5.2lf\n", current_uncom[GOAL]);
	
	i = NODE_NUM-2;
	route[NODE_NUM-1] = GOAL;
	while(v_node[p] != 0){
		route[i] = v_node[p];
		p = v_node[p];
		i--;
//		printf("%d ", v_node[i]);
	}
//	printf("\n");
//	for(i=1;i<NODE_NUM;i++){
//		if(route[i] != 0){
//			printf("%d -> ", route[i]);
//		}
//	}
//	printf("\n");
	
	new_necessary_time = current_sumtime[GOAL];
	new_uncom = current_uncom[GOAL];
	for(i=1;i<NODE_NUM;i++){
		new_route[i] = route[i];
	}
}
#endif
							