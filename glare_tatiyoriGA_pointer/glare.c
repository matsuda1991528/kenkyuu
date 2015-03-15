#ifndef Glare_C
#define Glare_C

#include<stdio.h>
#include<math.h>
#include"header.h"

/* glare data */
#define EXTRA_ILLUMI 134000 //taiyou housen syoudo
#define ATMOS_TRANS 0.60    //taiki toukaritu
#define SUN_LENGTH 149597870000 //taiyou tono kyori
#define BACKGROUND_LUMINANCE 1500

double getIlluminance(double);
double get3D_angle(double, double);
double getEquivalent_velling_luminance(double, double);
double getDisability_glare(double);

double getRadian(double degree){
	return degree * M_PI / 180.0f;
}


double getGlare(int p1, int p2, struct sun_angle_t sun_angle){
	double illuminance; // ���z�@���Ɠx
	double angle;
	double equivalent_velling_luminance;
	double disability_glare;
	double path_eye_degree; //�����Ƒ��z���ʂ̂Ȃ��p(2����)
	
	path_eye_degree = sun_angle.azimuth - matrix_angle[p1][p2];
	illuminance = getIlluminance(sun_angle.elevation);
	angle = get3D_angle(sun_angle.elevation, path_eye_degree);
	equivalent_velling_luminance = getEquivalent_velling_luminance(angle, illuminance);
	disability_glare = getDisability_glare(equivalent_velling_luminance);
//	printf("%f * %f\n", disability_glare, matrix[p1][p2].necessary_time);
	//return (1 + disability_glare) * matrix[p1][p2].time;
	return (1 + disability_glare);
}


////////////////////////////////////////////////////
//       ���z�@���Ɠx�v�Z                         //
////////////////////////////////////////////////////

double getIlluminance(double sun_high){
	
	double illumi = 0.0f; //���z�@���Ɠx
	double radian;
	double cosec;
	double extra_illumi = EXTRA_ILLUMI;
	double atmos_trans = ATMOS_TRANS;
	
	
	radian = getRadian(sun_high); //
	cosec = 1 / sin(radian); //
	illumi = extra_illumi * pow(atmos_trans, cosec);
	return illumi;
}

///////////////////////////////////////////////////
//           �������Ƒ��z�̂Ȃ��p(3����)         //
///////////////////////////////////////////////////

/* �����W�ϊ� */
static double get_vector_length(struct vector_3D_t v){
	return pow((v.x * v.x) + (v.y * v.y) + (v.z * v.z), 0.5);
}

static double dot_product(struct vector_3D_t vl, struct vector_3D_t vr){
	return vl.x * vr.x + vl.y * vr.y + vl.z * vr.z;
}

double get3D_angle(double high, double width){
	struct vector_3D_t vector_A;
	struct vector_3D_t vector_B;
	double length_A, length_B;
	double sita, fai;
	double cos_angle;
	double result_sita;
	double angle;
	double temp_sita;
	
	temp_sita = 90 - high;
	sita = getRadian(temp_sita);
	fai = getRadian(width);
	
	vector_A.x = SUN_LENGTH * sin(sita) * cos(fai);
	vector_A.y = SUN_LENGTH * sin(sita) * sin(fai);
	vector_A.z = SUN_LENGTH + cos(sita);
	
	vector_B.x = 1.0f;
	vector_B.y = 0.0f;
	vector_B.z = 0.0f;
	
	length_A = get_vector_length(vector_A);
	length_B = get_vector_length(vector_B);
	
	cos_angle = dot_product(vector_A,  vector_B) / (length_A * length_B);
	result_sita = acos(cos_angle);
	angle = result_sita * 180.0 / M_PI;
//	printf("angle = %f\n\n", angle);
	
	return angle;
	
}

///////////////////////////////////////////////////
//             ���������P�x�Z�o                  //
///////////////////////////////////////////////////

double getEquivalent_velling_luminance(double degree, double illuminance){
	double equivalent_velling_luminance;
	double parameter = 0.0096;
	double radian;
//	printf("degree = %f, illuminance = %f\n", degree, illuminance);
	radian = getRadian(degree);
	equivalent_velling_luminance = (parameter / M_PI) * (illuminance / pow(radian, 2));
//	printf("%f = (%f / %f) * (%f / %f)\n", equivalent_velling_luminance, parameter, M_PI, illuminance, pow(radian, 2));
	return equivalent_velling_luminance;
}

/////////////////////////////////////////////////
//          ���\�O���A�l�Z�o                   //
/////////////////////////////////////////////////

double getDisability_glare(double lv){
	double TI = 0.0f;
	double lr = BACKGROUND_LUMINANCE;
	
	if(lr > 5.0f){
		TI = 95.0f * lv / (pow(lr, 1.05));
//		printf("%f = 95 * %f / %f ^ 1.05(= %f)\n", TI, lv, lr, pow(lr, 1.05));
	}
	else{
		TI = 65.0f * lv / (pow(lr, 0.8));
	}
	return TI;
}
#endif