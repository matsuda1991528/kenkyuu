#ifndef serch_route_C
#define serch_route_C

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
#include"glare.h"

static double getHour(double time, double path_time, int waiting_time){
//	double temp;
	double hour, minute;
	hour = ((time + path_time) / 60) + START_HOUR;
	hour = floor(hour);
//	temp = time + path_time;
	minute = fmod((time + path_time), 60) + START_MIN + waiting_time;
//	printf("hour = %f, minute = %f, current_time = %f, nexttime = %f\n", hour, minute, time, path_time);
	return hour + (minute / 60);
}
		

void serch_route(){
	int i, j, k, p;
	double min_uncom; //�e�n�_�Ԃ̍ŏ�����
	double current_uncom[NODE_NUM]; //���̒n�_�ւ̌����_�Ŕ������Ă���s���x
	int flag[NODE_NUM];
	double current_sumtime[NODE_NUM]; //���̒n�_�ւ̍��v�ړ�����
	double temp_uncom;
	double temp;
	double mv_hour; //�ړ����ԁi���j
	double mv_min; //�ړ����ԁi���j
	double mv_time; //�ړ����ԁi�� + ���j
	double temp_sun_width;
	struct sun_data sun;
	
/* �������F�X�^�[�g�m�[�h�̋�����0�C���̃m�[�h�𖢒�`�iINF�j�ɐݒ� */
	for(i=1;i<NODE_NUM;i++){
		current_uncom[i] = INF;
		flag[i] = 0;
		route[i] = 0;
		current_sumtime[i] = 0.0f;
	}
	current_uncom[START] = waiting_time;
	v_node[START] = 0;
	
	for(i=1;i<NODE_NUM;i++){
		min_uncom = INF;
	/*�@�s���x�ŏ��̐ߓ_�̒T���@*/
	/* �ŏ���  p��start */		
		for(j=1;j<NODE_NUM;j++){
//			printf("uncom:%f < %f\n", current_uncom[j], min_uncom);
			if(flag[j] == 0 && current_uncom[j] < min_uncom){
				p = j;
//				printf("%d = %d\n", p, j);
				min_uncom = current_uncom[j];
			}
		}
		
		/* �ŏ������̐ߓ_�Ɋm��t���O�𗧂Ă� */
		flag[p] = 1;
		
		if(min_uncom == INF){
			printf("Can not arrive goal\n");
			exit(1);
		}		
		/* p(�m��m�[�h)����L�тĂ���ӂ̋��������ꂼ��`�F�b�N���C
		����܂ł̋����ŏ��o�H��菬������΍X�V�@*/
		
		/* uncom[p] + matrix_d[p][j] : uncom[p]�Ɗe�אڃm�[�h�̋����̘a */
		for(j=1;j<NODE_NUM;j++){
		
			/* p(�m��m�[�h)����L�тĂ���ӂ̈ړ����Ԃ����ꂼ��`�F�b�N���C
			�e�X�̎��Ԃł̑��z���x�Ƒ��z���ʂ̃f�[�^���擾*/
			if(matrix[p][j].time != INF){
				mv_hour = mv_min = mv_time = 0.0f;
//				printf("sumtime[%d] = %f, matrix[%d][%d] = %f ->\n", p, current_sumtime[p], p, j, matrix[p][j].necessary_time);
				mv_time = getHour(current_sumtime[p], matrix[p][j].time, waiting_time);
				getSunAngle(mv_time);
				//printf("%lf, %lf\n", elevation, azimuth);
				
				if(elevation <= 25 && elevation >=0.0f){
					//if(matrix[p][j].path_degree > 130 + azimuth || matrix[p][j].path_degree < -130 - azimuth){
					if(fabs(azimuth - matrix[p][j].angle) < 50.0f){
						temp_uncom = getUncom(p, j, elevation, azimuth);
						if((current_uncom[p] + temp_uncom) < current_uncom[j]){
							current_uncom[j] = current_uncom[p] + temp_uncom;
							v_node[j] = p;
							current_sumtime[j] = current_sumtime[p] + matrix[p][j].time;
//							printf("temp_uncom = %f, matrix = %f\n", temp_uncom, matrix[p][j].necessary_time);
						}
					}
					else{
						if(current_uncom[p] + matrix[p][j].time < current_uncom[j]){
							current_uncom[j] = current_uncom[p] + matrix[p][j].time;
							v_node[j] = p;
							current_sumtime[j] = current_sumtime[p] + matrix[p][j].time;
						}
					}
				}
				else{
					if(current_uncom[p] + matrix[p][j].time < current_uncom[j]){
						current_uncom[j] = current_uncom[p] + matrix[p][j].time;
						v_node[j] = p;
						current_sumtime[j] = current_sumtime[p] + matrix[p][j].time;
						//printf("uncom = %f, sumtime = %f\n", current_uncom[j], current_sumtime[j]);
					}
				}
			}
		}
		if(p == GOAL)
			break;
	}
//	printf("goal = %d\n", p);
//	printf("time = %5.2lf\n", current_sumtime[GOAL]);
//	printf("uncom_value = %5.2lf\n", current_uncom[GOAL]);
	
	i = NODE_NUM-2;
	route[NODE_NUM-1] = GOAL;
	while(v_node[p] != 0){
		route[i] = v_node[p];
		p = v_node[p];
		i--;
//		printf("%d ", v_node[i]);
	}
//	printf("\n");
//	for(i=1;i<NODE_NUM;i++){
//		if(route[i] != 0){
//			printf("%d -> ", route[i]);
//		}
//	}
//	printf("\n");
	
	new_necessary_time = current_sumtime[GOAL];
	new_uncom = current_uncom[GOAL];
	for(i=1;i<NODE_NUM;i++){
		new_route[i] = route[i];
	}
}
#endif
							