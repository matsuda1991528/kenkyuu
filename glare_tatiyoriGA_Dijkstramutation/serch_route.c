#ifndef serch_route_C
#define serch_route_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
//#include"glare.h"

struct routeData serch_route(int start, int goal, double temp_time){
	int i, j, k, p;
	double min;  //各地点間の最小不快度
	int flag[NODE_NUM];  //探索の確定フラグ
	double uncom[NODE_NUM];  //この地点への現時点で判明している不快度
	double time[NODE_NUM];
	double temp, temp_uncom, temp_hour, temp_min;
	double mv_hour, mv_min, mv_time;
	double st_hour, st_min;
	int v_node[NODE_NUM];
	struct routeData dijk;


//	printf("%d, %d\n",start, goal);
/* st_hour, st_min の計算 */	
//	temp_time = 0.0f;

	temp_hour = temp_time / 60.0;
	temp_hour = floor(temp_hour);
	st_hour = START_HOUR + temp_hour;
	temp_min = fmod(temp_time, 60.0);
	st_min = START_MIN + temp_min;
	if(st_min >= 60.0){
		temp_hour = st_min / 60.0;
		temp_hour = floor(temp_hour);
		st_min = fmod(st_min, 60.0);
		st_hour += temp_hour;
	}
	
/* 初期化：スタートノードの距離を0，他のノードを未定義（INF）に設定*/
	for(i=1;i<NODE_NUM;i++){
		uncom[i] = INF;
		flag[i] = 0;
		dijk.route[i] = 0;
		time[i] = 0.0f;
	}
	uncom[start] = 0.0f;
	v_node[start] = 0;
	for(i=1;i<NODE_NUM;i++){
		min = INF;
	/*　不快度最小の節点の探索　*/
	/* 最初は  p←start */
		for(j=1;j<NODE_NUM;j++){
			if(flag[j] == 0 && uncom[j] < min){
				p = j;
				min = uncom[j];
			}
		}
		/* 最小距離の節点に確定フラグを立てる */
		flag[p] = 1;
		/* 地図　が連結していない場合のエラｰ処理*/
		if(min == INF){
			printf("Can not arrive goal\n");
			printf("%d, %d\n",start, goal);
			exit(1);
		}
		/* p(確定ノード)から伸びている辺の距離をそれぞれチェックし，
		それまでの距離最小経路より小さければ更新　*/
		
		/* uncom[p] + matrix_d[p][j] : uncom[p]と各隣接ノードの距離の和 */
		for(j=1;j<NODE_NUM;j++){
		/* p(確定ノード)から伸びている辺の移動時間をそれぞれチェックし，
		各々の時間での太陽高度と太陽方位のデータを取得*/
			if(matrix[p][j].time != INF){
				mv_hour = ((time[p] + matrix[p][j].time) / 60) + st_hour;
				mv_hour = floor(mv_hour);
				temp = time[p] + matrix[p][j].time;
				mv_min = fmod(temp, 60.0) + st_min;
				mv_time = mv_hour + mv_min / 60;
				getSunAngle(mv_time);
				/* ①太陽高度　= 0 < α < 25 の場合 */
				if(elevation <= 25 && elevation >= 0){
				/* ① && 太陽正面との差角 = -50 < β < 50 の場合*/
					if(fabs(azimuth - matrix[p][j].angle) < 50.0f){
						temp_uncom = getUncom(p, j, elevation, azimuth);
						if(uncom[p] + temp_uncom < uncom[j]){
							uncom[j] = uncom[p] + temp_uncom;
							v_node[j] = p;
							time[j] = time[p] + matrix[p][j].time;
						}
					}
					/* ① && 太陽正面との差角 = -70 < β < 70 以外の場合 */
					else{
						if(uncom[p] + matrix[p][j].time < uncom[j]){
							uncom[j] = uncom[p] + matrix[p][j].time;
							v_node[j] = p;
							time[j] = time[p] + matrix[p][j].time;
						}
					}
				}
				/* ②太陽高度　= 0 < α < 25 以外の場合 */
				else{
					if(uncom[p] + matrix[p][j].time < uncom[j]){
						uncom[j] = uncom[p] + matrix[p][j].time;
						//printf("%f\n", matrix[p][j].time);
						v_node[j] = p;
						time[j] = time[p] + matrix[p][j].time;
					}
				}
			}
		}
		if(p == goal)
			break;
	}
	
	i=NODE_NUM-2;
	dijk.uncom = uncom[p];
	dijk.time = time[p];
	dijk.route[NODE_NUM-1] = goal;
	while(v_node[p] != 0){
		dijk.route[i] = v_node[p];
		p = v_node[p];
		i--;
	}
	//printf("%f\n", dijk.uncom);
	return dijk;
}
#endif
				