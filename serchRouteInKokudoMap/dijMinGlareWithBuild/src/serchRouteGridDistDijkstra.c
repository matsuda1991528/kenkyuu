#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"header.h"

/* 速度の値　km/h -> m/min に変換*/
static double convertVelHourMin(int vel_kph){
	return (double)vel_kph / 60.0f * 1000;
}

static void initQueue(double* uncom, int* flag, int* prev, double* time, struct vertex_t* vertex, double* travt, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		uncom[i] = INF;
		flag[i] = FALSE;
		prev[i] = EMPTY;
		time[i] = EMPTY;
		vertex[i].adj_list_p = vertex[i].adj_list_head;
		travt[i] = INF;
	}
	return;
}

/* 
始発点と終着点の連結性をチェック 
繋がっていないならばエラーメッセージを表示する
*/

static void checkConnectedStartToGoal(int min_uncom, int start, int goal){
	if(min_uncom == INF){
		printf("not connected start to goal\n");
		printf("%d -> %d\n", start, goal);
		exit(1);
	}
	return;
}


/* 2点間の距離を計算する関数 */
static double getDist(struct xy_coord_t p1, struct xy_coord_t p2){
	return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

/* 2点間の角度を計算する
					(北:90度)
（西:180度） 　　　　　　（東:0度）
　　　　　　　（南:270度）
*/
static double getEdgeAngle(struct xy_coord_t p1, struct xy_coord_t p2){
	double radian = atan2(p2.y - p1.y, p2.x - p1.x);
	double deg = radian * 180 / M_PI;
	
	if(deg < 0)
		return deg + 360;
	else
		return deg;
}

/* 2点間の西日グレア発生判定 */
static double getUncom(double trav_time, double edge_angle, double arrival_time, struct xy_coord_t ed_begin_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	struct sun_angle_t sun_angle;
	double glare;
	int sun_state;
	
	sun_angle = getSunAngle(fmod(arrival_time/60, 24.0f));
	
		
	if(sun_angle.elev <= SUN_HIGH && sun_angle.elev >= SUN_LOW && fabs(sun_angle.azim - edge_angle) < SUN_WIDTH){
		switch(BUILD_MODE){
			case 0:
				sun_state = FALSE;
				break;
			case 1: 
				sun_state = getSunStateWithBuildFromGrid(sun_angle, ed_begin_pos, build_grid, grid_size);
				break;
			default:
				printf("invailed BUILD_MODE value %d\n", BUILD_MODE);
				exit(1);
		}
		if(sun_state == TRUE){
			return trav_time;
		}
		else{
			glare = getGlare(sun_angle, edge_angle);
			return glare * trav_time;
		}
	}
	else{
		return trav_time;
	}
}

/* 経路をファイルに書き込む */
static void printOptimumRoute(int* prev, int p, struct vertex_t* vertex, int start, int goal, struct time_t dptr){
	FILE *fp;
	char fname[100];
	//ファイル名の指定
	sprintf(fname, "..\\route\\DAT\\dist_%d_%d_%d_%d.dat", start, goal, dptr.hour, dptr.min);
	
	fileOpenCheck(&fp, fname, "w");
	
	while(prev[p] != EMPTY){
		fprintf(fp, "%f %f\n", vertex[prev[p]].node.pos.x, vertex[prev[p]].node.pos.y);
		//printf("%f %f\n", vertex[prev[p]].node.pos.x, vertex[prev[p]].node.pos.y);
		p = prev[p];
	}
	return;
}	

static void evaluateOptimumRoute(int* prev, int p, struct vertex_t* vertex, int start, int goal, struct time_t dptr, int node_size, double mph, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	int next[node_size];
	double travt[node_size];
	double dist, trav_time, arrival_time, edge_angle, edge_uncom;
	double sum_travt = 0;
	int i;
	FILE *fp;
	char fname[100];
	//ファイル名の指定
	sprintf(fname, "..\\glr_route\\DAT\\glr_dist_%d_%d_%d_%d.dat", start, goal, dptr.hour, dptr.min);
	fileOpenCheck(&fp, fname, "w");
	
	/* 直前の通過頂点を示すprev配列から次に通過する頂点を示すnext配列を生成 */
	for(i=1;i<node_size;i++){
		next[i] = EMPTY;
		travt[i] = INF;
	}
	travt[start] = dptr.hour * 60.0 + dptr.min;
	
	while(prev[p] != EMPTY){
		next[prev[p]] = p;
		p = prev[p];
	}
	next[goal] = EMPTY;
	
	p = start;
	while(next[p] != EMPTY){
		dist = getDist(vertex[p].node.pos, vertex[next[p]].node.pos);
		trav_time = dist / mph;
		arrival_time = travt[p] + trav_time;
		edge_angle = getEdgeAngle(vertex[p].node.pos, vertex[next[p]].node.pos);
		edge_uncom = getUncom(trav_time, edge_angle, arrival_time, vertex[p].node.pos, build_grid, grid_size);
		if(edge_uncom != trav_time){
			fprintf(fp,"%f %f\n", vertex[p].node.pos.x, vertex[p].node.pos.y);
			fprintf(fp,"%f %f\n\n", vertex[next[p]].node.pos.x, vertex[next[p]].node.pos.y);
			sum_travt += trav_time;
		}
		travt[next[p]] = travt[p] + trav_time;
		
		p = next[p];
	}
	//printf("glare_travt = %f\n", sum_travt);
	return;
}

void serchRouteGridDistDijkstra(struct vertex_t* vertex, int node_size, int start, int goal, struct time_t dptr, int vel_kph, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	double uncom[node_size], time[node_size], travt[node_size], min_travt = INF;
	int flag[node_size];
	int i, j, p, index_num = 0;
	int prev[node_size];
	double dist, trav_time, edge_angle, edge_uncom, arrival_time;
	/* 速度（m/min）の変換 */
	double vel_mph = convertVelHourMin(vel_kph);
	
	/* 距離配列と探索配列の初期化 */
	initQueue(uncom, flag, prev, time, vertex, travt, node_size);
	
	/* 始点ノードのコスト，時刻を格納*/
	uncom[start] = 0.0f;
	travt[start] = 0.0f;
	//スタートノードの出発時刻の設定
	time[start] = dptr.hour * 60 + dptr.min;
	
	for(i=1;i<node_size;i++){
		min_travt = INF;
		/* 最小節点の探索 */	
		/* 
		コストは分かっているがその頂点への最適経路が確定されていない頂点を探索 
		最適経路は各確定済み頂点からの最適経路
		*/
		for(j=1;j<node_size;j++){
			if(travt[j] != INF){
				if(flag[j] == FALSE){
					if(travt[j] < min_travt || min_travt == INF){
						index_num = j;
						min_travt = travt[j];
					}
				}
			}
		}
		/* 最小距離の節点に確定フラグを立てる */
		flag[index_num] = TRUE;
		checkConnectedStartToGoal(min_travt, start, goal);

		/* 
		vertex[index_num]との辺を保持するvertexとの不快度及び到着時刻を算出する
		*/
		while(vertex[index_num].adj_list_p != NULL){
			test_edge_count++;
			/* 探索点から各節点への距離を計算 */
			dist = getDist(vertex[index_num].node.pos, vertex[vertex[index_num].adj_list_p->num].node.pos);
			trav_time = dist / vel_mph;
			arrival_time = time[index_num] + trav_time;
			edge_angle = getEdgeAngle(vertex[index_num].node.pos, vertex[vertex[index_num].adj_list_p->num].node.pos);
			edge_uncom = getUncom(trav_time, edge_angle, arrival_time, vertex[index_num].node.pos, build_grid, grid_size);
			/* 今までの節点の最小距離より低いならば，最小距離の更新 */
			if(travt[index_num] +  trav_time < travt[vertex[index_num].adj_list_p->num] || travt[vertex[index_num].adj_list_p->num] == INF){
				travt[vertex[index_num].adj_list_p->num] = travt[index_num] + trav_time;
				uncom[vertex[index_num].adj_list_p->num] = uncom[index_num] + edge_uncom;
				time[vertex[index_num].adj_list_p->num] = time[index_num] + trav_time;
				prev[vertex[index_num].adj_list_p->num] = index_num;
			}
			vertex[index_num].adj_list_p = vertex[index_num].adj_list_p->next;
		}
		vertex[index_num].adj_list_p = vertex[index_num].adj_list_head;
		
		if(index_num == goal)
			break;
	}
	p = goal;
	
	printf("%d, %d, %f\n", start, goal, uncom[goal]);
	//printf("uncom : %f\n", uncom[goal]);
	//printf("travt : %f\n", travt[goal]);
	if(PRINT_ROUTE == 1)
		printOptimumRoute(prev, p, vertex, start, goal, dptr);
	if(PRINT_GLARE_ROUTE == TRUE)
		evaluateOptimumRoute(prev, p, vertex, start, goal, dptr, node_size, vel_mph, build_grid, grid_size);
		
	return;
}