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

static double getIllum(double sun_elev){

	double illumi = 0.0f; //太陽法線照度
	double radian;
	double cosec;
	double extra_illumi = EXTRA_ILLUMI;
	double atmos_trans = ATMOS_TRANS;

	radian = getRadian(sun_elev); //
	cosec = 1 / sin(radian); //
	illumi = extra_illumi * pow(atmos_trans, cosec);
	return illumi;
}

static double getEqvltVllLumn(double degree, double illuminance){
	double equivalent_velling_luminance;
	double parameter = 0.0096;
	double radian;
	radian = getRadian(degree);
	equivalent_velling_luminance = (parameter / M_PI) * (illuminance / pow(radian, 2));
	return equivalent_velling_luminance;
}


double getGlrVal(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos){
  /* 辺の方位を求める */
  double edge_deg = getDegBtwn2pos(orgn_pos, dst_pos);
  /* 水平角(辺の方位と太陽方位の差角)を計算する */
  double edge_sun_deg = fabs(sun.azim - edge_deg);
  /* 太陽法線照度を計算する */
  double illum = getIllum(sun.elev);

  double deg_3D = get3DDeg(sun.elev, edge_sun_deg);

  double eqvlt_vll_lumn = getEqvltVllLumn(deg_3D, illum);

  return 1 + (lmbd * eqvlt_vll_lumn);
}
