#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"
#include "define.h"

double getRadian(double deg);
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);
double getDegBtwn2pos(struct xy_coord_t p1, struct xy_coord_t p2);
double get3DDeg(double hght, double wdth);

/**
*太陽高度sun_elevにおける太陽法線照度を求める．
@param sun_elev 太陽高度
@return 太陽法線照度
*/
static double getIllum(double sun_elev){

	double radian = getRadian(sun_elev); //
	double cosec = 1 / sin(radian); //

	return EXTRA_ILLUMI * pow(ATMOS_TRANS, cosec);
}

/**
*3次元空間における視線軸と太陽の差角から等価光幕輝度値を計算する
*@param edge_sun_deg 3次元空間における太陽と視線軸の差角
*@param illum 太陽法線照度
*@return 等価光幕輝度値
*/
static double getEqvltVllLumn(double edge_sun_deg, double illum){
	double param = 0.0096;
	double edge_sun_rad = getRadian(edge_sun_deg);
	return (param / M_PI) * (illum / pow(edge_sun_rad, 2));
}

/**
*太陽位置sunに頂点orgn_posにおいて，頂点dst_posを向いたときの，
*グレア値(等価光幕輝度値)を計算する．
*@param sun 太陽角度(鉛直角と水平角)
*@param orgn_pos 原点(移動元)座標
*@param dst_pos 目的点(移動先)座標
*@return グレア値→(1+等価光幕輝度値×西日に対する不快さを表す係数)
*/
double getGlrVal(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos){
  /* 辺の方位を求める */
  double edge_deg = getDegBtwn2pos(orgn_pos, dst_pos);
  /* 水平角(辺の方位と太陽方位の差角)を計算する */
  double edge_sun_deg = fabs(sun.azim - edge_deg);
  /* 太陽法線照度を計算する */
  double illum = getIllum(sun.elev);
	/* 3次元空間における視線軸と太陽の差角を計算する */
  double deg_3D = get3DDeg(sun.elev, edge_sun_deg);
	/* 等価光幕輝度値を計算する */
  double eqvlt_vll_lumn = getEqvltVllLumn(deg_3D, illum);

  return 1 + (lmbd * eqvlt_vll_lumn);
}
