#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"header.h"


static double get2PointAngle(struct xy_coord_t p1, struct xy_coord_t p2){
	double radian = atan2(p2.y - p1.y, p2.x - p1.x);
	double deg = radian * 180 / M_PI + 180.0f;
	if(deg > 180.0f)
		return deg - 180.0f;
	else
		return deg;
}

static double getRadian(double deg){
	return deg * M_PI / 180.0f;
}

static double getDist(struct xy_coord_t p1, struct xy_coord_t p2){
	return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

int getSunStateWithBuild(double edge_sun_diff_angle, double elev, double edge_angle, struct xy_coord_t edge_pos, struct build_set_t build_set){
	//int build_state[build_set.build_size];
	double build_edge_diff_angle;
	struct build_pos_t *p;
	double shadow_length;
	double build_edge_diff_length;
	p = build_set.head;
	while(p != NULL){
		build_edge_diff_angle = get2PointAngle(edge_pos, p->pos) - edge_angle;
		if(fabs(edge_sun_diff_angle - build_edge_diff_angle) < 10.0f){
			shadow_length = (double)BUILD_HIGH * 1/tan(getRadian(elev));
			build_edge_diff_length = getDist(edge_pos, p->pos);
			if(shadow_length >= build_edge_diff_length)
				return TRUE;
		}
		p = p->next;
	}
	return FALSE;
}