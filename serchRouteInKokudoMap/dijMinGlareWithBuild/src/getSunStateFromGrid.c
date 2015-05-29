#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"header.h"


static double getRadian(double deg){
	return deg * M_PI / 180.0f;
}

/*
static double getDist(struct xy_coord_t p1, struct xy_coord_t p2){
	return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}
*/


/* 
太陽と観測地を結ぶ直線の式と建物座標の垂線の長さを求める
y=ax+bの形で求める為，これをax+by+c = 0の形に変換した場合，
bは必ず-1となる為，垂線を求める式は予め定数‐1を用いている．
 */
static double getPerpendicularDist(double azimuth, struct xy_coord_t observe_pos, struct xy_coord_t build_pos){
	double slope;  //傾き
	double intercept;  //切片
	slope = tan(getRadian(azimuth)); //太陽と観測地点間を通る直線の傾きを求める．
	intercept = observe_pos.x * slope + observe_pos.y; //太陽と観測地点間を通る直線の切片を求める．
	
	return abs(slope * build_pos.x - build_pos.y + intercept) / sqrt(slope * slope + 1);
}


/* 2点間の距離を求める */
static double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos){
	return sqrt((target_pos.x - origin_pos.x) * (target_pos.x - origin_pos.x) + (target_pos.y - origin_pos.y) * (target_pos.y - origin_pos.y));
}

/* 観測地の属するグリッド番号を探索 */
static int getGridLabel(double observe_y, struct build_grid_t* build_grid, int grid_size){
	int grid_num;
	for(grid_num=1;grid_num<=grid_size;grid_num++){
		if(build_grid[grid_num].min_y < observe_y && build_grid[grid_num].max_y > observe_y)
			break;
		
		if(grid_num == grid_size){
			printf("observe_pos_y : %f is ok??\n", observe_y);
			exit(1);
		}
	}
	return grid_num;
}

/*
1. 観測地点と太陽のなす直線の式と建物の位置との垂線の距離を算出する． 
2. 建物の幅より垂線の距離が短いならば影の長さを算出する．
　if 影の長さが建物との距離より長いならば，太陽は隠れると判断
    otherwise 次の建物を参照し，1．へ
 */
int getSunStateWithBuildFromGrid(struct sun_angle_t sun_angle, struct xy_coord_t observe_pos, struct build_grid_t* build_grid, int grid_size){
	struct build_pos_t *p;
	double perpendicular_dist; //垂線の長さ
	double shadow_dist; //影の長さ
	double build_distance;//観測地‐建物間の距離
	int grid_label; //観測者が適用するグリッド番号
	
	grid_label = getGridLabel(observe_pos.y, build_grid, grid_size);
	
	p = build_grid[grid_label].head;
	while(p != NULL){
		perpendicular_dist = getPerpendicularDist(sun_angle.azim, observe_pos, p->pos); //垂線の長さを求める
		if(perpendicular_dist >= p->width){
			shadow_dist = (double)BUILD_HIGH * (1 / tan(getRadian(sun_angle.elev)));  //影の長さを求める
			build_distance = getDist(observe_pos, p->pos);  //建物と観測地の距離を求める
			if(shadow_dist >= build_distance){  //建物‐観測地間の距離と影の距離を比較
				return TRUE;
			}
		}
		p = p->next;  //次の建物データを参照する
	}
	return FALSE;
}
