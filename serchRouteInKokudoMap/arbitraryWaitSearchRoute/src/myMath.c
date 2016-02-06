#include <stdio.h>
#include <math.h>
#include "header.h"
#include "define.h"

double getRadian(double deg){
	return deg * M_PI / 180.0f;
}

/**
*2点間の距離を求める
*@param origin_pos 原点座標
*@param target_pos 目的座標
*/
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos){
	return sqrt((target_pos.x - origin_pos.x) * (target_pos.x - origin_pos.x) + (target_pos.y - origin_pos.y) * (target_pos.y - origin_pos.y));
}


/**
*2点間の角度を計算する(単位は度)
*					(北:90度)
*（西:180度） 　　（東:0度）
*　　　　　（南:270度）
*/
double getDegBtwn2pos(struct xy_coord_t p1, struct xy_coord_t p2){
	double radian = atan2(p2.y - p1.y, p2.x - p1.x);
	double deg = radian * 180 / M_PI;

	if(deg < 0)
		return deg + 360;
	else
		return deg;
}


/* 3次元ベクトルを表す構造体 */
typedef struct vector_3D_t{
	double x;
	double y;
	double z;
}vector_3D_t;

double get3DDeg(double hght, double wdth){
	return sqrt(hght * hght + wdth * wdth);
}


static void calcRotatMatrx(vector_3D_t rotat_rad, double rotat_matrx[3][3]){

	rotat_matrx[0][0] = cos(rotat_rad.y) * cos(rotat_rad.z);
	rotat_matrx[0][1] = -cos(rotat_rad.y) * sin(rotat_rad.z);
	rotat_matrx[0][2] = sin(rotat_rad.y);

	rotat_matrx[1][0] = sin(rotat_rad.z);
	rotat_matrx[1][1] = cos(rotat_rad.z);
	rotat_matrx[1][2] = 0.0f;

	rotat_matrx[2][0] = -sin(rotat_rad.y) * cos(rotat_rad.z);
	rotat_matrx[2][1] = sin(rotat_rad.y) * sin(rotat_rad.z);
	rotat_matrx[2][2] = cos(rotat_rad.y);

	return;
}

/**
*z軸を中心にhrznl_deg回転させ，さらに，y軸を中心にvrtcl_deg回転させた
*時の3次元空間における回転角度を求める．
*/
double get3DRotatDeg(double vrtcl_deg, double hrznl_deg){
	vector_3D_t rotat_rad;
	double rotat_matrx[3][3];
	rotat_rad.x = 0.0f;
	rotat_rad.y = getRadian(vrtcl_deg);
	rotat_rad.z = getRadian(hrznl_deg);

	calcRotatMatrx(rotat_rad, rotat_matrx);
	return EMPTY;
}
