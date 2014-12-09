#ifndef SunAngle_C
#define SunAngle_C

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"

static int day_of_year(int year, int month, int day);
static void solarAngle(int y_day, double time, double *elevation, double *azimuth);

void getSunAngle(double time){
	int year, month, day;
	int y_day;
	double fact = 0.0f;
	int hour;
	double min;
	int i, j;
	
	elevation = 0.0f;
	azimuth = 0.0f;
	fact = M_PI / 180; //�x���@�̊p�x�@�~�@fact = �ʓx�@�i���W�A���ϊ��j
	year = 2014;
	month = 6;
	day = 21;
	
	y_day = day_of_year(year, month, day);
	solarAngle(y_day, time, &elevation, &azimuth);
	elevation = elevation / fact;
	azimuth = (azimuth / fact) - 270;  //����0�x�ɂ��鏈��
	return;	
}
#endif

static int day_of_year(int year, int month, int day){
	int month_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int i, result;
	
/* ���̋K���ɏ]���āC400�N�Ԃ�97��̂��邤�N��݂���
1,����N���S�Ŋ���؂��N�͂��邤�N
2,�������C����N��100�Ŋ���؂��N�͕��N
3,�������C����N��400�Ŋ���؂��N�͂��邤�N
 */	
	if(year%4 == 0 && year%100 == 0 && year%400 == 0  ){
		month_day[1] = 29;
	}else if(year%4 == 0 && year%100 == 0 && year %400 != 0){
		month_day[1] = 28;
	}else{
		month_day[1] = 29;
	}
	
	result = 0;
	for(i=0;i<month-1;i++)
		result = result + month_day[i];
		
	result = result + day;
	return result;
}

static void solarAngle(int y_day,double time,double *elevation,double *azimuth){
	double lat = 34.39, lon = 135.0; // lat = �ܓx�@lon = �o�x ������
	double lat1, lon1; //lat, lon �����ꂼ�ꃉ�W�A���ϊ������l
	double timelon = 135.0;
	double declination; //���z�Ԉ܁i�ԓ����W�ɂ�����ܓx�j
	double A, B, et;
	double localtime, t;
	double coszenith, sinkai, coskai, kai, kai1;
	
	lat1 = lat * M_PI / 180;
	lon1 = lon * M_PI / 180;
	
/* �Ԉ܂̌v�Z */	
	A = 2 * M_PI * y_day / 365;	
	declination = 0.006918 - 0.399912 * cos(A) + 0.070257 * sin(A)
				-0.006758 * cos(2*A) + 0.000907 * sin(2*A)
				-0.002697 * cos(3*A) + 0.001480 * sin(3*A);
	
/* ���ϑ��z���i�V������̑��x�ŉ^������Ɖ��肵�����z�̑��z�̓����Ɋ�Â��Č��߂�ꂽ�����j�̌v�Z */
	localtime = time + (lon - timelon) / 15;

/* �ώ����̌v�Z */	
	B = 2 * M_PI * y_day / 365;	
	et = ( 0.000075 + 0.001868 * cos(B) - 0.032077*sin(B)
		- 0.014615 * cos(2*B) - 0.040849 * sin(2*B) ) * 12 / M_PI;
	/* ���p�̌v�Z */	
	t = 15 * (M_PI/180) * (localtime + et -12);

/* cos(�V���p)�̌v�Z */	
	coszenith = sin(declination) * sin(lat1) + cos(declination) * cos(lat1) * cos(t);
	*elevation = 0.5 * M_PI - acos(coszenith);
	
/* sin(���z���ʊp)�̌v�Z�@�@0.5 * M_PI - elevation = ���z�V���p */
	sinkai = cos(declination) * sin(t) / sin(0.5 * M_PI - *elevation);
/* cos(���z���ʊp)�̌v�Z */
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
		
	