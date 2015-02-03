#ifndef serch_route_C
#define serch_route_C
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"extern.h"
//#include"glare.h"

struct routeData serch_route(int start, int goal, double temp_time){
	int i, j, k, p;
	double min;  //�e�n�_�Ԃ̍ŏ��s���x
	int flag[NODE_NUM];  //�T���̊m��t���O
	double uncom[NODE_NUM];  //���̒n�_�ւ̌����_�Ŕ������Ă���s���x
	double time[NODE_NUM];
	double temp, temp_uncom, temp_hour, temp_min;
	double mv_hour, mv_min, mv_time;
	double st_hour, st_min;
	int v_node[NODE_NUM];
	struct routeData dijk;


//	printf("%d, %d\n",start, goal);
/* st_hour, st_min �̌v�Z */	
//	temp_time = 0.0f;

	temp_hour = temp_time / 60.0;
	temp_hour = floor(temp_hour);
	st_hour = START_HOUR + temp_hour;
	temp_min = fmod(temp_time, 60.0);
	st_min = START_MIN + temp_min;
	if(st_min >= 60.0){
		temp_hour = st_min / 60.0;
		temp_hour = floor(temp_hour);
		st_min = fmod(st_min, 60.0);
		st_hour += temp_hour;
	}
	
/* �������F�X�^�[�g�m�[�h�̋�����0�C���̃m�[�h�𖢒�`�iINF�j�ɐݒ�*/
	for(i=1;i<NODE_NUM;i++){
		uncom[i] = INF;
		flag[i] = 0;
		dijk.route[i] = 0;
		time[i] = 0.0f;
	}
	uncom[start] = 0.0f;
	v_node[start] = 0;
	for(i=1;i<NODE_NUM;i++){
		min = INF;
	/*�@�s���x�ŏ��̐ߓ_�̒T���@*/
	/* �ŏ���  p��start */
		for(j=1;j<NODE_NUM;j++){
			if(flag[j] == 0 && uncom[j] < min){
				p = j;
				min = uncom[j];
			}
		}
		/* �ŏ������̐ߓ_�Ɋm��t���O�𗧂Ă� */
		flag[p] = 1;
		/* �n�}�@���A�����Ă��Ȃ��ꍇ�̃G�������*/
		if(min == INF){
			printf("Can not arrive goal\n");
			printf("%d, %d\n",start, goal);
			exit(1);
		}
		/* p(�m��m�[�h)����L�тĂ���ӂ̋��������ꂼ��`�F�b�N���C
		����܂ł̋����ŏ��o�H��菬������΍X�V�@*/
		
		/* uncom[p] + matrix_d[p][j] : uncom[p]�Ɗe�אڃm�[�h�̋����̘a */
		for(j=1;j<NODE_NUM;j++){
		/* p(�m��m�[�h)����L�тĂ���ӂ̈ړ����Ԃ����ꂼ��`�F�b�N���C
		�e�X�̎��Ԃł̑��z���x�Ƒ��z���ʂ̃f�[�^���擾*/
			if(matrix[p][j].time != INF){
				mv_hour = ((time[p] + matrix[p][j].time) / 60) + st_hour;
				mv_hour = floor(mv_hour);
				temp = time[p] + matrix[p][j].time;
				mv_min = fmod(temp, 60.0) + st_min;
				mv_time = mv_hour + mv_min / 60;
				getSunAngle(mv_time);
				/* �@���z���x�@= 0 < �� < 25 �̏ꍇ */
				if(elevation <= 25 && elevation >= 0){
				/* �@ && ���z���ʂƂ̍��p = -50 < �� < 50 �̏ꍇ*/
					if(fabs(azimuth - matrix[p][j].angle) < 50.0f){
						temp_uncom = getUncom(p, j, elevation, azimuth);
						if(uncom[p] + temp_uncom < uncom[j]){
							uncom[j] = uncom[p] + temp_uncom;
							v_node[j] = p;
							time[j] = time[p] + matrix[p][j].time;
						}
					}
					/* �@ && ���z���ʂƂ̍��p = -70 < �� < 70 �ȊO�̏ꍇ */
					else{
						if(uncom[p] + matrix[p][j].time < uncom[j]){
							uncom[j] = uncom[p] + matrix[p][j].time;
							v_node[j] = p;
							time[j] = time[p] + matrix[p][j].time;
						}
					}
				}
				/* �A���z���x�@= 0 < �� < 25 �ȊO�̏ꍇ */
				else{
					if(uncom[p] + matrix[p][j].time < uncom[j]){
						uncom[j] = uncom[p] + matrix[p][j].time;
						//printf("%f\n", matrix[p][j].time);
						v_node[j] = p;
						time[j] = time[p] + matrix[p][j].time;
					}
				}
			}
		}
		if(p == goal)
			break;
	}
	
	i=NODE_NUM-2;
	dijk.uncom = uncom[p];
	dijk.time = time[p];
	dijk.route[NODE_NUM-1] = goal;
	while(v_node[p] != 0){
		dijk.route[i] = v_node[p];
		p = v_node[p];
		i--;
	}
	//printf("%f\n", dijk.uncom);
	return dijk;
}
#endif
				