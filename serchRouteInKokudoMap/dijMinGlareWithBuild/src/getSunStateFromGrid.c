#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"header.h"

#define PAI 3.14159265358979323846
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ADD(a, b) (a + b)
#define ABS(a) ((a) < 0 ? - (a) : (a))


static double getRadian(double deg){
	return deg * M_PI / 180.0f;
}

/** 
���z�Ɗϑ��n�����Ԓ����̎��ƌ������W�̐����̒��������߂�
y=ax+c�̌`�ŋ��߂�ׁC�����ax+by+c = 0�̌`�ɕϊ������ꍇ�C
ax-y+c=0�ƂȂ�D������C
b�͕K��-1�ƂȂ�ׁC���������߂鎮�͗\�ߒ萔�]1��p���Ă���D

http://www004.upp.so-net.ne.jp/s_honma/urawaza/distance.htm
 */
static double getPerpendicularDist(double azimuth, struct xy_coord_t observe_pos, struct xy_coord_t build_pos){
	double slope;  //�X��
	double intercept;  //�ؕ�
	
	slope = tan(getRadian(azimuth)); //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̌X�������߂�D
	intercept = observe_pos.y - observe_pos.x * slope; //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̐ؕЂ����߂�D	
	return abs(build_pos.y - slope * build_pos.x - intercept) / sqrt(slope * slope + 1);
	
}


/** 
*2�_�Ԃ̋��������߂� 
*@param origin_pos ���_���W
*@param target_pos �ړI���W
*/
static double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos){
	return sqrt((target_pos.x - origin_pos.x) * (target_pos.x - origin_pos.x) + (target_pos.y - origin_pos.y) * (target_pos.y - origin_pos.y));
}

/**
*�����ɑ΂���e�̒��������߂�D
*@param sun_angle ���z�̈ʒu
*@param build_height �����̍���
*@return xy_shadow_len x�Cy�����ɂ�������e�x�N�g���̒���
*/
static struct xy_coord_t getShadowLen(struct sun_angle_t sun_angle, double build_height){
	double shadow_len;
	struct xy_coord_t xy_shadow_len;
	double tmp;
	
	shadow_len = build_height * (1 / tan(getRadian(sun_angle.elev)));
	tmp = shadow_len * sin(getRadian(sun_angle.azim));
	xy_shadow_len.y = ABS(tmp);

	tmp = shadow_len * cos(getRadian(sun_angle.azim));
	xy_shadow_len.x = ABS(tmp);

	return xy_shadow_len;
}

static struct xy_coord_t getMinPos(struct xy_coord_t fst_pos, struct xy_coord_t scn_pos){
	struct xy_coord_t min;
	
	min.x = MIN(fst_pos.x, scn_pos.x);
	min.y = MIN(fst_pos.y, scn_pos.y);
	
	return min;
}

static struct xy_coord_t getMaxPos(struct xy_coord_t fst_pos, struct xy_coord_t scn_pos){
	struct xy_coord_t max;
	
	max.x = MAX(fst_pos.x, scn_pos.x);
	max.y = MAX(fst_pos.y, scn_pos.y);
	
	return max;
}

/**
*src_bgn �� src_end��Ίp���_�Ɏ������`�ɁCtrg_bgn, trg_end��Ίp���_�Ɏ������`��
*�d�����邩�𔻒肷��֐�
*/
int dtctOvrRpWithRect(struct xy_coord_t src_bgn, struct xy_coord_t src_end, struct xy_coord_t trg_bgn, struct xy_coord_t trg_end){
	
	//src�̒[�_������(bgn)�ƉE��(end)�łƂ�ꂽ�ꍇ�D
	if(src_bgn.x <= trg_end.x && trg_bgn.x <= src_end.x && 
		src_bgn.y <=  trg_end.y && trg_bgn.y <= src_end.y){
		return TRUE;
	}
	else
		return FALSE;
}
	

/**
*�e�O���b�h�ɑ΂��錚���ɂ��O���A�������菈��
*
*1. �ϑ��n�_�Ƒ��z�̂Ȃ������̎��ƌ����̈ʒu�Ƃ̐����̋������Z�o����D 
*2. �����̕���萂���̋������Z���Ȃ�Ήe�̒������Z�o����D
*�@if �e�̒����������Ƃ̋�����蒷���Ȃ�΁C���z�͉B���Ɣ��f
*   otherwise ���̌������Q�Ƃ��C1�D��
 */
 int serchGlareStateFromBuild(struct sun_angle_t sun_angle, struct xy_coord_t obs_pos, struct build_pos_t *head){
	struct build_pos_t *p;
	double perp_dist; //�����̒���
	double sh_dist; //�e�̒���
	double bu_dist;//�ϑ��n�]�����Ԃ̋���

	p = head;
	p = p->next;
	while(p != NULL){
		perp_dist = getPerpendicularDist(sun_angle.azim, obs_pos, p->pos); //�����̒��������߂�
		//if(perp_dist >= p->width){
		sh_dist = BUILD_HIGH * (1 / tan(getRadian(sun_angle.elev)))
				+ fabs(cos(getRadian(sun_angle.azim))) * p->width;  //�e�̒��������߂�
		bu_dist = getDist(obs_pos, p->pos);  //�����Ɗϑ��n�̋��������߂�
		//if((sh_dist >= bu_dist || p->width >= bu_dist)){ //�����]�ϑ��n�Ԃ̋����Ɖe�̋������r
		if(sh_dist >= bu_dist && perp_dist <= p->width){
			return TRUE;
		}
		p = p->next;  //���̌����f�[�^���Q�Ƃ���
	}
	return FALSE;
}
 
 /**
* �O���b�h�n�}���猚���ɑ΂���O���A���������o����֐�
*@param sun_angle ���z�̊p�x
*@param obs_pos  �ϑ��n�i�����Ԃ̌��ݒn�j
*@param build_grid : �O���b�h�n�}�f�[�^�Z�b�g
*@param grid_size : ��k�����Ɠ��������ɂ�����O���b�h�̃Z����
*@return �O���A���Օ������Ȃ��TRUE ����ȊO��FALSE
 */
int getSunStateWithBuildFromGrid(struct sun_angle_t sun_angle, struct xy_coord_t obs_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	int glare_state; //�O���A��� TRUE -> �O���A�͔������Ȃ�	FALSE -> �O���A����
	struct xy_coord_t sh_tip_pos;  //�e�̐�[���W 
	struct xy_coord_t sh_min, sh_max;
	int indx_bx, indx_by, indx_ex, indx_ey; //�����ΏۃO���b�h�̎n�[�y�яI�[�C���f�b�N�X�ԍ�
	int i, j;
	
	//�����̍����ɑ΂���e�x�N�g���Ƃ��̓����C��k�����̒������v�Z����
	struct xy_coord_t shadow_length = getShadowLen(sun_angle, BUILD_HIGH);
	
	/* ���������Ɠ�k�����ɂ�����C�����̈ʒu�ɓ͂�����e�̍ŉ��̋��������߂�D */
	sh_tip_pos.x = obs_pos.x - (shadow_length.x);
	//���z���k�����獷�����ޏꍇ(���e���쑤�։��т�)
	if(0 < sun_angle.azim && sun_angle.azim < 180) 
		sh_tip_pos.y = obs_pos.y + (shadow_length.y);
	//���z���쑤���獷�����ޏꍇ(���e�͖k���։��т�)
	else
		sh_tip_pos.y = obs_pos.y - (shadow_length.y);

//�ϑ��n�Ɠ��e�̐L�ѓ��閖�[���W��Ίp���_�Ɏ������`�̎n�[(min.x, min.y)�ƏI�[(max.x, max.y)���Z�o����D
	sh_min = getMinPos(obs_pos, sh_tip_pos);
	sh_max = getMaxPos(obs_pos, sh_tip_pos);
		
	//���W�͑����ΏۃG���A�Ɉʒu���Ȃ����C��������O���b�h�n�}����͂ݏo�������������ΏۂƂ���ׂɁC
	//�O���b�h���o�͈͂𓌐��Ɠ�k�Ɍ�����������������D
	sh_min.x = ADD(sh_min.x, -build_width);
	sh_min.y = ADD(sh_min.y, -build_width);
	sh_max.x = ADD(sh_max.x, build_width);
	sh_max.y = ADD(sh_max.y, build_width);
	
	//printf("%f, %f -> %f, %f\n", sh_min.x, sh_min.y, sh_max.x, sh_max.y);
	//printf("min (%f, %f) max(%f, %f)\n", build_grid[1][1].min.x, build_grid[1][1].min.y, build_grid[1][1].max.x, build_grid[1][1].max.y);
	
	indx_bx = ((int)sh_min.x - (int)build_grid[1][1].min.x) / (int)g_size.x;
	indx_by = ((int)sh_min.y - (int)build_grid[1][1].min.y) / (int)g_size.y;
	indx_ex = (((int)sh_max.x - (int)build_grid[1][1].min.x) / (int)g_size.x) + 1;
	indx_ey = (((int)sh_max.y - (int)build_grid[1][1].min.y) / (int)g_size.y) + 1;
	
	
	if(indx_bx < 1)
		indx_bx = 1;
	if(indx_by < 1)
		indx_by = 1;
	if((grid_size.width -1) < indx_ex)
		indx_ex = grid_size.width -1;
	if(grid_size.vertical - 1 < indx_ey)
		indx_ey = grid_size.vertical - 1;

		/* �K�؂ȃO���b�h�T�C�Y�Ɋւ�������p�������@ */
	if(GSZ_EXP == 0){
		for(i=1;i<grid_size.vertical;i++){
			for(j=1;j<grid_size.width;j++){
				if(indx_by <= i && i <= indx_ey){
					if(indx_bx <= j && j <= indx_ex){
						glare_state = serchGlareStateFromBuild(sun_angle, obs_pos, build_grid[i][j].head);
						if(glare_state == TRUE)
							return TRUE;
					}
				}
			}
		}
	}
	/* �ʏ�̎����p�������@(�������̕�������) */
	else{
		for(i=indx_by; i <= indx_ey; i++){
			for(j = indx_bx;j<=indx_ex;j++){
				glare_state = serchGlareStateFromBuild(sun_angle, obs_pos, build_grid[i][j].head);
				if(glare_state == TRUE)
					return TRUE;
			}
		}
		return FALSE;
	}
}
