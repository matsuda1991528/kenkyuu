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
static struct grid_size_t getGridLabel(struct xy_coord_t obs_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	struct grid_size_t grid_num;
	int find_flag = FALSE;
	for(grid_num.vertical=1;grid_num.vertical<grid_size.vertical;grid_num.vertical++){
		for(grid_num.width=1;grid_num.width<grid_size.width;grid_num.width++){
			if(build_grid[grid_num.vertical][grid_num.width].min.y <= obs_pos.y && build_grid[grid_num.vertical][grid_num. width].max.y > obs_pos.y
			  && build_grid[grid_num.vertical][grid_num.width].min.x <= obs_pos.x && build_grid[grid_num.vertical][grid_num. width].max.x > obs_pos.x){
				find_flag = TRUE;
				goto end;
			}
		}
	}
	end:
	
	if(find_flag == FALSE){
		printf("okasii-> %f %f\n", obs_pos.x, obs_pos.y);
		exit(1);
	}
		
	return grid_num;
}

/*
各グリッドに対する建物によるグレア発生判定処理

1. 観測地点と太陽のなす直線の式と建物の位置との垂線の距離を算出する． 
2. 建物の幅より垂線の距離が短いならば影の長さを算出する．
　if 影の長さが建物との距離より長いならば，太陽は隠れると判断
    otherwise 次の建物を参照し，1．へ
 */
 int serchGlareStateFromBuild(struct sun_angle_t sun_angle, struct xy_coord_t observe_pos, struct build_pos_t *head){
	struct build_pos_t *p;
	double perpendicular_dist; //垂線の長さ
	double shadow_dist; //影の長さ
	double build_distance;//観測地‐建物間の距離
	
	p = head;
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
 
int getSunStateWithBuildFromGrid(struct sun_angle_t sun_angle, struct xy_coord_t observe_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	struct grid_size_t grid_label; //観測者が適用するグリッド番号
	int glare_state; //グレア発生か未発生
	static struct xy_coord_t shadow_length;
	static int flag = FALSE;
	int serch_state[grid_size.vertical][grid_size.width];
	double param;
	int i, j;
	
	for(i=1;i<grid_size.vertical;i++){
		for(j=1;j<grid_size.width;j++){
			serch_state[i][j] = FALSE;
		}
	}
	
	if(flag == FALSE){
		shadow_length = getGridLength();
		//printf("shadow_length = %f %f\n", shadow_length.x, shadow_length.y);
		flag = TRUE;
	}
	
	grid_label = getGridLabel(observe_pos, build_grid, grid_size);
	//printf("\n ***************************\n");
	//printf("grid_label %d %d\n", grid_label.vertical, grid_label.width);
	//グリッド地図を走査する
	//自グリッドとその最大影長の範囲内のグリッド地図を走査する．
	for(param = 0.2; param <= 1; param += 0.2){
		for(i=1;i<grid_size.vertical;i++){
			for(j=1;j<grid_size.width;j++){
				if(build_grid[i][j].min.y <= build_grid[grid_label.vertical][grid_label.width].min.y + shadow_length.y * param
				&& build_grid[i][j].max.y >= build_grid[grid_label.vertical][grid_label.width].min.y - shadow_length.y * param
				&& build_grid[i][j].max.x >= build_grid[grid_label.vertical][grid_label.width].min.x - shadow_length.x * param
				&& build_grid[i][j].min.x <= build_grid[grid_label.vertical][grid_label.width].min.x
				&& serch_state[i][j] != TRUE){
					//printf("%d %d -> " ,i, j);
					glare_state = serchGlareStateFromBuild(sun_angle, observe_pos, build_grid[i][j].head);
					if(glare_state == TRUE)
						return TRUE;
					else
						serch_state[i][j] = TRUE;  //グリッドに対する探索済みフラグを立てる
				}
			}
		}
	}
	return FALSE;
}
