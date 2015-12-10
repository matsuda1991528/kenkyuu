#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"
#include "define.h"
#include "myMath.h"

/**
*�ዅ�ɑ΂���O���A�����̗L���𒲂ׂ�D
*�����p(�ӂ̕��ʂƑ��z���ʂ̍��p)
*�����p(���z���x)
*�ɂ�����3�����p�x���Z�o����D
*���̊p�x���O���A�����p�x�̏���l��菭�Ȃ��Ȃ�΁C
*�O���A����������Ɣ��肷��D
*/
static int fndGlrOccredFromEye(sun_angle_t sun, double edge_deg){
  /* �����p(�ӂ̕��ʂƑ��z���ʂ̍��p)���v�Z���� */
  double edge_sun_deg = fabs(sun.azim - edge_deg);
/*
*3�����p�x�ɂ����鑾�z�Ǝ������̍��p���O���A��������p�x��菬�����Ȃ�΁C
*�O���A�͔�������D�����łȂ��Ȃ�΁C�O���A�͔������Ȃ�
*/
  //if(30.0f > get3DDeg(sun.elev, edge_sun_deg))
  if(0 <= sun.elev && sun.elev <= 30.0f && edge_sun_deg < 30.0f)
    return TRUE;
  else
    return FALSE;
}

/**
*�ԑ̂ɑ΂���O���A�����̗L���𒲂ׂ�D
*�t�����g�K���X�ɐ������������ނȂ�΁C
*�ԑ̂ɑ΂���O���A����������Ɣ��肷��D
*/
static int fndGlrOccredFromCar(sun_angle_t sun, double edge_deg){
  if(SUN_LOW <= sun.elev && sun.elev <= SUN_HIGH && fabs(sun.azim - edge_deg) < SUN_WIDTH)
    return TRUE;
  else
    return FALSE;
}

/**
*�����ɑ΂���e��xy�����ɂ����钷�������߂�D
*@param sun_angle ���z�̈ʒu
*@param bld_hght �����̍���
*@return shdw_vctr x�Cy�����ɂ�������e�x�N�g���̒���
*/
static xy_coord_t getShdwVctr(sun_angle_t sun, double bld_hght){
  double shdw_len = bld_hght * (1 / tan(getRadian(sun.elev)));
  xy_coord_t shdw_vctr;
  shdw_vctr.y = fabs(shdw_len * sin(getRadian(sun.azim)));
  shdw_vctr.x = fabs(shdw_len * cos(getRadian(sun.azim)));

  return shdw_vctr;
}

/**
���z�p�x��sun�Ō��ݒn��orgn�ɂ����āC���e�̓����y�ѓ�k������shdw_vctr�̎��C
���ݒn�֓��e���͂�����ŉ��̍��W�����߂�D
*/
static xy_coord_t getShdwTailPos(sun_angle_t sun, xy_coord_t orgn, xy_coord_t shdw_vctr){
  xy_coord_t shdw_tail_pos;

  /* ����͐����݂̂���舵���ׁC����������͂����e�݂̂��l������ */
  shdw_tail_pos.x = orgn.x - shdw_vctr.x;

  /* ���z���k���Ɉʒu����ꍇ(���e�͓���։��т�) */
  if(0.0f < sun.azim && sun.azim < 180.0f){
    shdw_tail_pos.y = orgn.y + shdw_vctr.y;
  }
  /* ���z������Ɉʒu����ꍇ(���e�͖k���։��т�) */
  else{
    shdw_tail_pos.y = orgn.y - shdw_vctr.y;
  }
  return shdw_tail_pos;
}

static xy_coord_t getMinPos(xy_coord_t fst_pos, xy_coord_t scn_pos){
  xy_coord_t min;
  min.x = MIN(fst_pos.x, scn_pos.x);
  min.y = MIN(fst_pos.y, scn_pos.y);

  return min;
}

static xy_coord_t getMaxPos(xy_coord_t fst_pos, xy_coord_t scn_pos){
	xy_coord_t max;
	max.x = MAX(fst_pos.x, scn_pos.x);
	max.y = MAX(fst_pos.y, scn_pos.y);

	return max;
}

void setTrvBgnIndx(int *indx_x, int *indx_y, xy_coord_t shdw_bgn,
   xy_coord_t grd_orgn_pos, xy_coord_t grd_len){

  *indx_x = ((int)shdw_bgn.x - (int)grd_orgn_pos.x) / grd_len.x;
  *indx_y = ((int)shdw_bgn.y - (int)grd_orgn_pos.y) / grd_len.y;

  if(*indx_x < 1)
    *indx_x = 1;
  if(*indx_y < 1)
    *indx_y = 1;

  return;
}

void setTrvEndIndx(int *indx_x, int *indx_y, xy_coord_t shdw_end,
   xy_coord_t grd_end_pos, xy_coord_t grd_len, grid_size_t grd_cell_sz){

  *indx_x = (((int)shdw_end.x - (int)grd_end_pos.x) / grd_len.x) + 1;
  *indx_y = (((int)shdw_end.y - (int)grd_end_pos.y) / grd_len.y) + 1;

  if((grd_cell_sz.ew -1) < *indx_x)
    *indx_x = grd_cell_sz.ew -1;
  if((grd_cell_sz.sn - 1) < *indx_y)
    *indx_y = grd_cell_sz.sn - 1;
  else if(*indx_y < 1){
    //fprintf(stderr, "warning : expected trv_end_y has negative value\n");
    *indx_y = 1;
  }

  return;
}


/**
���z�Ɗϑ��n�����Ԓ����̎��ƌ������W�̐����̒��������߂�
y=ax+c�̌`�ŋ��߂�ׁC�����ax+by+c = 0�̌`�ɕϊ������ꍇ�C
ax-y+c=0�ƂȂ�D������C
b�͕K��-1�ƂȂ�ׁC���������߂鎮�͗\�ߒ萔�]1��p���Ă���D

http://www004.upp.so-net.ne.jp/s_honma/urawaza/distance.htm
 */
static double getPerpDist(double azim, xy_coord_t obs_pos, xy_coord_t bld_pos){
	double slope = tan(getRadian(azim)); //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̌X�������߂�D
	double intrcpt = obs_pos.y - obs_pos.x * slope; //���z�Ɗϑ��n�_�Ԃ�ʂ钼���̐ؕЂ����߂�D

  return fabs(bld_pos.y - slope * bld_pos.x - intrcpt) / sqrt(slope * slope + 1);
}

/**
*�e�O���b�h�ɑ΂��錚���ɂ��O���A�������菈��
*
*1. �ϑ��n�_�Ƒ��z�̂Ȃ������̎��ƌ����̈ʒu�Ƃ̐����̋������Z�o����D
*2. �����̕���萂���̋������Z���Ȃ�Ήe�̒������Z�o����D
*�@if �e�̒����������Ƃ̋�����蒷���Ȃ�΁C���z�͉B���Ɣ��f
*   otherwise ���̌������Q�Ƃ��C1�D��
 */
int fndBldHydedSun(sun_angle_t sun, xy_coord_t obs_pos, build_pos_t *head){
	build_pos_t *ptr;
	double perp_dist; //�����̒���
	double shdw_len; //�e�̒���
	double bu_dist;//�ϑ��n�]�����Ԃ̋���

	ptr = head;
	ptr = ptr->next;
	while(ptr != NULL){
		perp_dist = getPerpDist(sun.azim, obs_pos, ptr->pos); //�����̒��������߂�
		if(perp_dist <= ptr->wdth){
		    shdw_len = BUILD_HIGH * (1 / tan(getRadian(sun.elev)))
				    + fabs(cos(getRadian(sun.azim))) * ptr->wdth;  //�e�̒��������߂�
		    bu_dist = getDist(obs_pos, ptr->pos);  //�����Ɗϑ��n�̋��������߂�
        if(shdw_len >= bu_dist) //�����]�ϑ��n�Ԃ̋����Ɖe�̋������r
          return TRUE;
		}
		ptr = ptr->next;  //���̌����f�[�^���Q�Ƃ���
	}
	return FALSE;
}

/**
*�����ɑ΂���O���A�����̗L���𒲂ׂ�D
*�������Օ����錚������������Ȃ�΁C
*�����ɑ΂���O���A����������Ɣ��肷��D
*/
static int fndGlrOccredFromBld(sun_angle_t sun, xy_coord_t orgn_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
/* ���e�x�N�g���̓��������Ɠ�k�����̒������v�Z���� */
  xy_coord_t shdw_vctr = getShdwVctr(sun, BUILD_HIGH);

/* ���ݒn�֓��e���͂�����ŉ��̍��W�����߂�D */
  xy_coord_t shdw_end_pos = getShdwTailPos(sun, orgn_pos, shdw_vctr);
  /* �ϑ��n�Ɠ��e�̓͂����閖�[���W��Ίp���_�Ɏ���
  ��`�̎n�[���W�ƏI�[���W���Z�o���� */
  xy_coord_t shdw_bgn_pos = getMinPos(orgn_pos, shdw_end_pos);
  shdw_end_pos = getMaxPos(orgn_pos, shdw_end_pos);

  /*�ϑ��n�̓����Ɉʒu���邪�C�����Ɍ��ݒn��������ꍇ�C�����͎Օ������D
	���ׁ̈C�������Ɠ�܂��͖k�����Ɍ�����������������D*/
  shdw_end_pos.x = ADD(shdw_end_pos.x, max_bld_wdth);
  /* ���z���k���Ɉʒu����ꍇ(���e�͓���։��т�) */
  if(0.0f < sun.azim && sun.azim < 180.0f){
    shdw_end_pos.y = ADD(shdw_end_pos.y, max_bld_wdth);
  }
  /* ���z������Ɉʒu����ꍇ(���e�͖k���։��т�) */
  else{
    shdw_end_pos.y = ADD(shdw_end_pos.y, -max_bld_wdth);
  }

/* �����ΏۃO���b�h�n�}�̎n�[�C���f�b�N�X�ԍ��ƏI�[�C���f�b�N�X�ԍ������蓖�Ă� */
  int trv_bgn_x, trv_bgn_y, trv_end_x, trv_end_y;
  setTrvBgnIndx(&trv_bgn_x, &trv_bgn_y, shdw_bgn_pos, bld_grd[1][1].bgn, grd_len);
  setTrvEndIndx(&trv_end_x, &trv_end_y, shdw_end_pos,
    bld_grd[1][1].end, grd_len, grd_cell_sz);

/* �����ΏۃO���b�h�n�}�𑖍����Ă����D*/
  int i, j;
  for(i=trv_bgn_y;i<=trv_end_y;i++){
    for(j=trv_bgn_x;j<=trv_end_x;j++){

      if(TRUE == fndBldHydedSun(sun, orgn_pos, bld_grd[i][j].head))
        return FALSE;
    }
  }
  return TRUE;
}

/**
*����tim��orgn_pos�ɂ����āCdst_pos�������������Ƃ��ɁC
*�����W��bld_grd�����n����n�}��ŁC�O���A���������邩�𔻒肷��D
�O���A����������ꍇ��TRUE��Ԃ��C�������Ȃ��ꍇ��FALSE��Ԃ��D
*/
int fndGlrOccred(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){

/* �ӂ̕��ʂ����߂� */
  double edge_deg = getDegBtwn2pos(orgn_pos, dst_pos);
/* �ዅ�ɑ΂���O���A�����̗L���𔻒肷��D */
  if(FALSE == fndGlrOccredFromEye(sun, edge_deg))
    return FALSE;
/* �ԑ̂ɑ΂���O���A�����̗L���𔻒肷��D */
  if(FALSE == fndGlrOccredFromCar(sun, edge_deg))
    return FALSE;
/* �����ɑ΂���O���A�����̗L���𔻒肷��D */
  if(FALSE == fndGlrOccredFromBld(sun, orgn_pos, bld_grd, grd_len, grd_cell_sz))
    return FALSE;

  return TRUE;

}
