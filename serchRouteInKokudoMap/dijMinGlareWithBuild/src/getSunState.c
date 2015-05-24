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
���z�Ɗϑ��n�����Ԓ����̎��ƌ������W�̐����̒��������߂�
y=ax+b�̌`�ŋ��߂�ׁC�����ax+by+c = 0�̌`�ɕϊ������ꍇ�C
b�͕K��-1�ƂȂ�ׁC���������߂鎮�͗\�ߒ萔�]1��p���Ă���D
 */
static double getPerpendicularDist(double azimuth, struct xy_coord_t observe_pos, struct xy_coord_t build_pos){
	double slope;  //�X��
	double intercept;  //�ؕ�
	slope = tan(getRadian(azimuth)); //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̌X�������߂�D
	intercept = observe_pos.x * slope + observe_pos.y; //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̐ؕЂ����߂�D
	
	return abs(slope * build_pos.x - build_pos.y + intercept) / sqrt(slope * slope + 1);
}


/* 2�_�Ԃ̋��������߂� */
static double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos){
	return sqrt((target_pos.x - origin_pos.x) * (target_pos.x - origin_pos.x) + (target_pos.y - origin_pos.y) * (target_pos.y - origin_pos.y));
}

/*
1. �ϑ��n�_�Ƒ��z�̂Ȃ������̎��ƌ����̈ʒu�Ƃ̐����̋������Z�o����D 
2. �����̕���萂���̋������Z���Ȃ�Ήe�̒������Z�o����D
�@if �e�̒����������Ƃ̋�����蒷���Ȃ�΁C���z�͉B���Ɣ��f
    otherwise ���̌������Q�Ƃ��C1�D��
 */
int getSunStateWithBuild(struct sun_angle_t sun_angle, struct xy_coord_t observe_pos, struct build_set_t build_set){
	struct build_pos_t *p;
	double perpendicular_dist; //�����̒���
	double shadow_dist; //�e�̒���
	double build_distance;//�ϑ��n�]�����Ԃ̋���
	
	p = build_set.head;
	while(p != NULL){
		perpendicular_dist = getPerpendicularDist(sun_angle.azim, observe_pos, p->pos); //�����̒��������߂�
		if(perpendicular_dist >= p->width){
			shadow_dist = (double)BUILD_HIGH * (1 / tan(getRadian(sun_angle.elev)));  //�e�̒��������߂�
			build_distance = getDist(observe_pos, p->pos);  //�����Ɗϑ��n�̋��������߂�
			if(shadow_dist >= build_distance){  //�����]�ϑ��n�Ԃ̋����Ɖe�̋������r
				return TRUE;
			}
		}
		p = p->next;  //���̌����f�[�^���Q�Ƃ���
	}
	return FALSE;
}
