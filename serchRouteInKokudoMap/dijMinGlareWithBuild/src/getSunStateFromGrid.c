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

http://www004.upp.so-net.ne.jp/s_honma/urawaza/distance.htm
 */
static double getPerpendicularDist(double azimuth, struct xy_coord_t observe_pos, struct xy_coord_t build_pos){
	double slope;  //�X��
	double intercept;  //�ؕ�
	
	slope = tan(getRadian(azimuth)); //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̌X�������߂�D
	//intercept = observe_pos.y - observe_pos.x * slope; //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̐ؕЂ����߂�D	
	//return abs(build_pos.y - slope * build_pos.x - intercept) / sqrt(slope * slope + 1);
	intercept = observe_pos.y + observe_pos.x * slope;
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
	p = p->next;
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
	struct xy_coord_t shadow_length = getGridLength();
	int i, j;
	int count = 0;
	
	for(i=1;i<grid_size.vertical;i++){
		for(j=1;j<grid_size.width;j++){
			build_grid[i][j].p = build_grid[i][j].head;
			build_grid[i][j].p = build_grid[i][j].p->next;
			while(build_grid[i][j].p != NULL){
				count++;
				build_grid[i][j].p = build_grid[i][j].p->next;
			}
		}
	}
	//printf("count = %d\n", count);
	//exit(1);
	
	
	
	grid_label = getGridLabel(observe_pos, build_grid, grid_size);
	//printf("max:%f %f\n", build_grid[grid_label.vertical][grid_label.width].max.x, build_grid[grid_label.vertical][grid_label.width].max.y);
	//printf("min:%f %f\n", build_grid[grid_label.vertical][grid_label.width].min.x, build_grid[grid_label.vertical][grid_label.width].min.y);
	//printf("obs:%f %f\n", observe_pos.x, observe_pos.y);
	//printf("\n ***************************\n");
	//printf("grid_label %d %d\n", grid_label.vertical, grid_label.width);
	//�O���b�h�n�}�𑖍�����
	//���O���b�h�Ƃ��̍ő�e���͈͓̔��̃O���b�h�n�}�𑖍�����D
	for(i=1;i<grid_size.vertical;i++){
		for(j=1;j<grid_size.width;j++){
			if(build_grid[i][j].min.y <= build_grid[grid_label.vertical][grid_label.width].max.y + shadow_length.y
			&& build_grid[i][j].max.y >= build_grid[grid_label.vertical][grid_label.width].min.y - shadow_length.y
			&& build_grid[i][j].max.x >= build_grid[grid_label.vertical][grid_label.width].min.x - shadow_length.x
			&& build_grid[i][j].min.x <= build_grid[grid_label.vertical][grid_label.width].max.x)
				glare_state = serchGlareStateFromBuild(sun_angle, observe_pos, build_grid[i][j].head);
				
				if(glare_state == TRUE)
					return TRUE;
			//}
		}
	}
	//printf("grid_count = %d\n", count);
	return FALSE;
}
