/*
 * utmConverter.c
 *
 *  Created on: 2015/04/05
 *      Author: sosmem
 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"header.h"

//パラメータ（世界測地系）
#define SEMI_MAJOR_AXIS 6378137 //楕円の長半径
#define OBLATENESS 1/298.257222101 //扁平率
/* 東経の場合は経度＋180すること */
#define CENTRAL_MERDIAN 315 //中央子午線（ゾーン番号によって異なる）
#define K_0 0.9996 //オフセット値??
#define E_0 500000
#define N_0 0  //北半球→0　南半球→10000000



static double convertDegToRad(double degree){
	return degree * M_PI / 180.0f;
}

/* パラメータを求める関数群 */
static double getA(double n){
	return SEMI_MAJOR_AXIS / (1 + n) * (1 + pow(n, 2) / 4 + pow(n, 4) / 64.0);
}

static void getAlpha(double* alpha, double n){
	alpha[1] = 1.0/2.0*n - 2.0/3.0*pow(n, 2) + 5.0/16.0*pow(n, 3);
	alpha[2] = 13.0/48.0*pow(n, 2) - 3.0/5.0*pow(n, 3);
	alpha[3] = 61.0/240.0*pow(n, 3);

	return;
}

static void getBeta(double* beta, double n){
	beta[1] = 1.0/2.0*n - 2.0/3.0*pow(n, 2) + 37.0/96.0*pow(n, 3);
	beta[2] = 1.0/48.0*pow(n, 2) + 1.0/15.0*pow(n, 3);
	beta[3] = 17.0/480.0*pow(n, 3);

	return;
}


static void getDelta(double* delta, double n){

	delta[1] = 2.0*n - 2.0/3.0*pow(n, 2) - 2.0*pow(n, 3);
	delta[2] = 7.0/3.0*pow(n, 2) - 8.0/5.0*pow(n, 3);
	delta[3] = 56.0/15.0*pow(n, 3);

	return;
}

/* 変数を求める関数群 */
static double getT(double phi, double n){
	double phi_rad = convertDegToRad(phi);

	return sinh(atanh(sin(phi_rad)) - 2*sqrt(n)/(1+n) * atanh(2*sqrt(n)/(1+n)*sin(phi_rad)));
}

static double getXi(double t, double lambda, double lambda_0){
	return atan(t / cos(convertDegToRad(lambda - lambda_0)));
}

static double getEta(double t, double lambda, double lambda_0){
	return atanh((sin(convertDegToRad(lambda - lambda_0)) / sqrt(1 + t*t)));
}

/* xy座標変換 */
static double getXcoord(double* alpha, double A, double eta, double xi){
	int i;
	double end_eq = 0.0f;
	for(i=1;i<=3;i++){
		end_eq += alpha[i] * cos(2.0*i*xi) * sinh(2.0*i*eta);
	}
	return E_0 + K_0 * A * (eta + end_eq);
}

static double getYcoord(double* alpha, double A, double eta, double xi){
	int i;
	double end_eq = 0.0f;
	for(i=1;i<=3;i++){
		end_eq += alpha[i] * sin(2.0*i*xi) * cosh(2.0*i*eta);
	}
	return N_0 + K_0 * A * (xi + end_eq);
}

struct xy_coord_t ConvertLonLatToXyCoord(struct lon_lat_data_t before_convert_pos){
	struct xy_coord_t after_convert_pos;
	double n, A, alpha[4], beta[4], delta[4];
	double t, xi, eta;

	/* latが東経の場合のみ適用 */
	before_convert_pos.lat += 180.0;

	/* 初期値の算出 */
	n = OBLATENESS / (2 - OBLATENESS);
	A = getA(n);
	getAlpha(alpha, n);
	getBeta(beta, n);
	getDelta(delta, n);

	/* 変数の算出 */
	t = getT(before_convert_pos.lon, n);
	xi = getXi(t, before_convert_pos.lat, CENTRAL_MERDIAN);
	eta = getEta(t, before_convert_pos.lat, CENTRAL_MERDIAN);

	/* xy座標の算出 */
	after_convert_pos.x = getXcoord(alpha, A, eta, xi);
	after_convert_pos.y = getYcoord(alpha, A, eta, xi);
	return after_convert_pos;
}

