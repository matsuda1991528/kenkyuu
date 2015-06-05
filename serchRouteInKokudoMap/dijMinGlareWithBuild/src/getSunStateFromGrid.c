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

/* �ϑ��n�̑�����O���b�h�ԍ���T�� */
static struct grid_size_t getGridLabel(struct xy_coord_t obs_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	struct grid_size_t grid_num;
	int find_flag = FALSE;
	for(grid_num.vertical=1;grid_num.vertical<grid_size.vertical;grid_num.vertical++){
		for(grid_num.width=1;grid_num.width<grid_size.width;grid_num.width++){
			//printf("%d %d\n", grid_num.vertical, grid_num.width);
			/*
			if(obs_pos.x == 270236.249225 && obs_pos.y == 3811151.733564){
				if(build_grid[grid_num.vertical][grid_num.width].min.x > (obs_pos.x - 50) && build_grid[grid_num.vertical][grid_num.width].min.x < (obs_pos.x + 50)
				&& build_grid[grid_num.vertical][grid_num.width].min.y > (obs_pos.y - 50) && build_grid[grid_num.vertical][grid_num.width].min.y < (obs_pos.y + 50)){
					printf("min : %f %f\n", build_grid[grid_num.vertical][grid_num.width].min.x, build_grid[grid_num.vertical][grid_num.width].min.y);
					printf("max : %f %f\n", build_grid[grid_num.vertical][grid_num. width].max.x, build_grid[grid_num.vertical][grid_num. width].max.y);
				}
			}
			*/
			if(build_grid[grid_num.vertical][grid_num.width].min.y <= obs_pos.y && build_grid[grid_num.vertical][grid_num. width].max.y > obs_pos.y
			  && build_grid[grid_num.vertical][grid_num.width].min.x <= obs_pos.x && build_grid[grid_num.vertical][grid_num. width].max.x > obs_pos.x){
				find_flag = TRUE;
				goto end;
			}
		}
	}
	end:
	
	if(find_flag == FALSE){
		printf("okasii-> %f %f\n", obs_pos.x, obs_pos.y);
		exit(1);
	}
		
	return grid_num;
}

/*
�e�O���b�h�ɑ΂��錚���ɂ��O���A�������菈��

1. �ϑ��n�_�Ƒ��z�̂Ȃ������̎��ƌ����̈ʒu�Ƃ̐����̋������Z�o����D 
2. �����̕���萂���̋������Z���Ȃ�Ήe�̒������Z�o����D
�@if �e�̒����������Ƃ̋�����蒷���Ȃ�΁C���z�͉B���Ɣ��f
    otherwise ���̌������Q�Ƃ��C1�D��
 */
 int serchGlareStateFromBuild(struct sun_angle_t sun_angle, struct xy_coord_t observe_pos, struct build_pos_t *head){
	struct build_pos_t *p;
	double perpendicular_dist; //�����̒���
	double shadow_dist; //�e�̒���
	double build_distance;//�ϑ��n�]�����Ԃ̋���
	
	p = head;
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
 
int getSunStateWithBuildFromGrid(struct sun_angle_t sun_angle, struct xy_coord_t observe_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	struct grid_size_t grid_label; //�ϑ��҂��K�p����O���b�h�ԍ�
	int glare_state; //�O���A������������
	int i, j;
	
	grid_label = getGridLabel(observe_pos, build_grid, grid_size);
	
	//�O���b�h�n�}�𑖍�����
	for(i=grid_label.vertical-1;i<=grid_label.vertical+1;i++){
		for(j=grid_label.width-1;j<=grid_label.width;j++){
			if(i > 0 && j > 0 && i < grid_size.vertical && j < grid_size.width){
				glare_state = serchGlareStateFromBuild(sun_angle, observe_pos, build_grid[i][j].head);
				if(glare_state == TRUE)
					return TRUE;
			}
		}
	}
	return FALSE;
}
