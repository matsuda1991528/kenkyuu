#ifndef SunAngle_C
#define SunAngle_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"
#include "define.h"

static int day_of_year(int year, int month, int day);
static void solarAngle(int y_day, double time, double *elevation, double *azimuth);

/*
入力された時刻に対する太陽高度，方位を返す．
					(北:90度)
（西:180度） 　　　　　　（東:0度）
　　　　　（南:270度）
*/

/**
*引数として受け取る時間(time)の単位は[hour]とし，
*[minutes]は十進数で表します．
*すなわち，時刻16:30における太陽角度を算出する場合，
*引数(tim)には，16.5が与えられる必要があります．
*/
sun_angle_t getSunAngle(double time){
	int year, month, day;
	int y_day;
	double fact;
	struct sun_angle_t sun_angle;

	fact = M_PI / 180; //度数法の角度　×　fact = 弧度法（ラジアン変換）
	year = YEAR;
	month = MONTH;
	day = DAY;

	y_day = day_of_year(year, month, day);
	solarAngle(y_day, time, &sun_angle.elev, &sun_angle.azim);
	sun_angle.elev = sun_angle.elev / fact;
	sun_angle.azim = fabs(360 - (sun_angle.azim / fact));  //角度を反転させる
	sun_angle.azim = fmod(sun_angle.azim + 90, 360);

	return sun_angle;
}

#endif

static int day_of_year(int year, int month, int day){
	int month_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int i, result;

/*
次の規則に従って，400年間に97回のうるう年を設ける
1)西暦年が４で割り切れる年はうるう年
2)西暦年が100で割り切れる年は平年
3)西暦年が400で割り切れる年はうるう年
 */
	if(year%4 == 0 && year%100 == 0 && year%400 == 0  ){
		month_day[1] = 29;
	}else if(year%4 == 0 && year%100 == 0 && year %400 != 0){
		month_day[1] = 28;
	}//else if(year%4 == 0){
	else{
		month_day[1] = 29;
	}

	result = 0;
	for(i=0;i<month-1;i++)
		result = result + month_day[i];

	result = result + day;
	return result;
}

static void solarAngle(int y_day,double time,double *elevation,double *azimuth){
	double lat = 34.39, lon = 135.0; // lat = 緯度　lon = 経度 ＠明石
	double lat1; //lat, lon をそれぞれラジアン変換した値
	double timelon = 135.0;
	double declination; //太陽赤緯（赤道座標における緯度）
	double A, B, et;
	double localtime, t;
	double coszenith, sinkai, coskai, kai, kai1;

	lat1 = lat * M_PI / 180;
	//lon1 = lon * M_PI / 180;

/* 赤緯の計算 */
	A = 2 * M_PI * y_day / 365;
	declination = 0.006918 - 0.399912 * cos(A) + 0.070257 * sin(A)
				-0.006758 * cos(2*A) + 0.000907 * sin(2*A)
				-0.002697 * cos(3*A) + 0.001480 * sin(3*A);

/* 平均太陽時（天空を一定の速度で運動すると仮定した仮想の太陽の動きに基づいて決められた時刻）の計算 */
	localtime = time + (lon - timelon) / 15;

/* 均時差の計算 */
	B = 2 * M_PI * y_day / 365;
	et = ( 0.000075 + 0.001868 * cos(B) - 0.032077*sin(B)
		- 0.014615 * cos(2*B) - 0.040849 * sin(2*B) ) * 12 / M_PI;
	/* 時角の計算 */
	t = 15 * (M_PI/180) * (localtime + et -12);

/* cos(天頂角)の計算 */
	coszenith = sin(declination) * sin(lat1) + cos(declination) * cos(lat1) * cos(t);
	*elevation = 0.5 * M_PI - acos(coszenith);

/* sin(太陽方位角)の計算　　0.5 * M_PI - elevation = 太陽天頂角 */
	sinkai = cos(declination) * sin(t) / sin(0.5 * M_PI - *elevation);
/* cos(太陽方位角)の計算 */
	coskai = (-cos(lat1) * sin(declination) + sin(lat1) * cos(declination) * cos(declination) * cos(t)) / sin(0.5 * M_PI - *elevation);

	kai = asin(sinkai);
	if(coskai < 0)
		kai1 = M_PI - kai;
	else if(coskai > 0 && sinkai < 0)
		kai1 = 2 * M_PI + kai;
	else
		kai1 = kai;

	kai1 = kai1 + M_PI;
	if(kai1 > 2*M_PI)
		kai1 = kai1 - 2*M_PI;

	*azimuth = kai1;
}
