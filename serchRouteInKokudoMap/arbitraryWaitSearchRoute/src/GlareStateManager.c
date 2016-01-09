#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"
#include "define.h"

double getRadian(double deg);
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);
double getDegBtwn2pos(struct xy_coord_t p1, struct xy_coord_t p2);

/**
*眼球に対するグレア発生の有無を調べる．
*水平角(辺の方位と太陽方位の差角)
*鉛直角(太陽高度)
*における3次元角度を算出する．
*この角度がグレア発生角度の上限値より少ないならば，
*グレアが発生すると判定する．
*/
static int fndGlrOccredFromEye(sun_angle_t sun, double edge_deg){
  /* 水平角(辺の方位と太陽方位の差角)を計算する */
  double edge_sun_deg = fabs(sun.azim - edge_deg);
  /*
  *3次元角度における太陽と視線軸の差角がグレア発生上限角度より小さいならば，
  *グレアは発生する．そうでないならば，グレアは発生しない
  */
  //if(30.0f > get3DDeg(sun.elev, edge_sun_deg))
  if(0.0f <= sun.elev && sun.elev <= 30.0f && edge_sun_deg < 30.0f)
    return TRUE;
  else
    return FALSE;
}

/**
*車体に対するグレア発生の有無を調べる．
*フロントガラスに西日が差しこむならば，
*車体に対するグレアが発生すると判定する．
*/
static int fndGlrOccredFromCar(sun_angle_t sun, double edge_deg){
  if(SUN_LOW <= sun.elev && sun.elev <= SUN_HIGH && fabs(sun.azim - edge_deg) < SUN_WIDTH)
    return TRUE;
  else
    return FALSE;
}

/**
*建物に対する影のxy方向における長さを求める．
*@param sun_angle 太陽の位置
*@param bld_hght 建物の高さ
*@return shdw_vctr x，y方向における日影ベクトルの長さ
*/
static xy_coord_t getShdwVctr(sun_angle_t sun, double bld_hght){
  double shdw_len = bld_hght * (1 / tan(getRadian(sun.elev)));
  xy_coord_t shdw_vctr;
  shdw_vctr.y = fabs(shdw_len * sin(getRadian(sun.azim)));
  shdw_vctr.x = fabs(shdw_len * cos(getRadian(sun.azim)));

  return shdw_vctr;
}

/**
太陽角度がsunで現在地がorgnにおいて，日影の東西及び南北成分がshdw_vctrの時，
現在地へ日影が届き得る最遠の座標を求める．
*/
static xy_coord_t getShdwTailPos(sun_angle_t sun, xy_coord_t orgn, xy_coord_t shdw_vctr){
  xy_coord_t shdw_tail_pos;

  /* 今回は西日のみを取り扱う為，東方向から届く日影のみを考慮する */
  shdw_tail_pos.x = orgn.x - shdw_vctr.x;

  /* 太陽が北方に位置する場合(日影は南方へ延びる) */
  if(0.0f < sun.azim && sun.azim < 180.0f){
    shdw_tail_pos.y = orgn.y + shdw_vctr.y;
  }
  /* 太陽が南方に位置する場合(日影は北方へ延びる) */
  else{
    shdw_tail_pos.y = orgn.y - shdw_vctr.y;
  }
  return shdw_tail_pos;
}

/**
*二つの位置座標から最小のxy座標を求める．
*@param fst_pos 比較対象位置座標
*@param scn_pos 比較対象位置座標
*@return min 二つの比較対象位置座標から最小のxy座標を抽出した座標
*/
static xy_coord_t getMinPos(xy_coord_t fst_pos, xy_coord_t scn_pos){
  xy_coord_t min;
  min.x = MIN(fst_pos.x, scn_pos.x);
  min.y = MIN(fst_pos.y, scn_pos.y);

  return min;
}

/**
*二つの位置座標から最大のxy座標を求める．
*@param fst_pos 比較対象位置座標
*@param scn_pos 比較対象位置座標
*@return min 二つの比較対象位置座標から最大のxy座標を抽出した座標
*/
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
太陽と観測地を結ぶ直線の式と建物座標の垂線の長さを求める
y=ax+cの形で求める為，これをax+by+c = 0の形に変換した場合，
ax-y+c=0となる．これより，
bは必ず-1となる為，垂線を求める式は予め定数‐1を用いている．
http://www004.upp.so-net.ne.jp/s_honma/urawaza/distance.htm
 */
static double getPerpDist(double azim, xy_coord_t obs_pos, xy_coord_t bld_pos){
	double slope = tan(getRadian(azim)); //太陽と観測地点間を通る直線の傾きを求める．
	double intrcpt = obs_pos.y - obs_pos.x * slope; //太陽と観測地点間を通る直線の切片を求める．

  return fabs(bld_pos.y - slope * bld_pos.x - intrcpt) / sqrt(slope * slope + 1);
}

/**
*各グリッドに対する建物によるグレア発生判定処理
*
*1. 観測地点と太陽のなす直線の式と建物の位置との垂線の距離を算出する．
*2. 建物の幅より垂線の距離が短いならば影の長さを算出する．
*　if 影の長さが建物との距離より長いならば，太陽は隠れると判断
*   otherwise 次の建物を参照し，1．へ
*@param sun 太陽角度
*@param obs_pos 観測地
*@param *head 建物集合の先頭リスト
*@return TRUE：西日が遮られる．　FALSE：西日が遮られない
 */
int fndBldHydedSun(sun_angle_t sun, xy_coord_t obs_pos, build_pos_t *head){
	build_pos_t *ptr;
	double perp_dist; //垂線の長さ
	double shdw_len; //影の長さ
	double bu_dist;//観測地‐建物間の距離

	ptr = head;
	ptr = ptr->next;
	while(ptr != NULL){
    bu_dist = getDist(obs_pos, ptr->pos);  //建物と観測地の距離を求める
    if(bu_dist < ptr->wdth) //現在地が建物で覆われるならば，西日は遮蔽される．
      return TRUE;

		perp_dist = getPerpDist(sun.azim, obs_pos, ptr->pos); //垂線の長さを求める
		if(perp_dist <= ptr->wdth){
		  shdw_len = BUILD_HIGH * (1 / tan(getRadian(sun.elev)))
				+ fabs(cos(getRadian(sun.azim))) * ptr->wdth;  //影の長さを求める

      if(shdw_len >= bu_dist){ //建物‐観測地間の距離と影の距離を比較
        return TRUE;
      }
		}
		ptr = ptr->next;  //次の建物データを参照する
	}
	return FALSE;
}

/**
*建物に対するグレア発生の有無を調べる．
*西日を遮蔽する建物が一つも無いならば，
*建物に対するグレアが発生すると判定する．
*/
static int fndGlrOccredFromBld(sun_angle_t sun, xy_coord_t orgn_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
/* 日影ベクトルの東西成分と南北成分の長さを計算する */
  xy_coord_t shdw_vctr = getShdwVctr(sun, BUILD_HIGH);

  if(bef_indx_x != EMPTY){
    if(fndBldHydedSun(sun, orgn_pos, bld_grd[bef_indx_y][bef_indx_x].head)){
      return FALSE;
    }
  }

/* 現在地へ日影が届き得る最遠の座標を求める． */
  xy_coord_t shdw_end_pos = getShdwTailPos(sun, orgn_pos, shdw_vctr);
  /* 観測地と日影の届き得る末端座標を対角頂点に持つ
  矩形の始端座標と終端座標を算出する */
  xy_coord_t shdw_bgn_pos = getMinPos(orgn_pos, shdw_end_pos);
  shdw_end_pos = getMaxPos(orgn_pos, shdw_end_pos);

  /*観測地の東側に位置するが，建物に現在地が覆われる場合，西日は遮蔽される．
	その為，東方向と南または北方向に建物幅だけ長くする．*/
  shdw_end_pos.x = ADD(shdw_end_pos.x, max_bld_wdth);
  /* 太陽が北方に位置する場合(日影は南方へ延びる) */
  if(0.0f < sun.azim && sun.azim < 180.0f){
    shdw_end_pos.y = ADD(shdw_end_pos.y, max_bld_wdth);
  }
  /* 太陽が南方に位置する場合(日影は北方へ延びる) */
  else{
    shdw_end_pos.y = ADD(shdw_end_pos.y, -max_bld_wdth);
  }

/* 走査対象グリッド地図の始端インデックス番号と終端インデックス番号を割り当てる */
  int trv_bgn_x, trv_bgn_y, trv_end_x, trv_end_y;
  setTrvBgnIndx(&trv_bgn_x, &trv_bgn_y, shdw_bgn_pos, bld_grd[1][1].bgn, grd_len);
  setTrvEndIndx(&trv_end_x, &trv_end_y, shdw_end_pos,
    bld_grd[1][1].end, grd_len, grd_cell_sz);

/* 走査対象グリッド地図を走査していく．*/
  int i, j;
  for(i=trv_bgn_y;i<=trv_end_y;i++){
    for(j=trv_bgn_x;j<=trv_end_x;j++){

      if(fndBldHydedSun(sun, orgn_pos, bld_grd[i][j].head)){
        bef_indx_x = j;
        bef_indx_y = i;
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
*時刻timにorgn_posにおいて，dst_pos方向を向いたときに，
*建物集合bld_grdが立地する地図上で，グレアが発生するかを判定する．
グレアが発生する場合はTRUEを返し，発生しない場合はFALSEを返す．
*/
int fndGlrOccred(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){

/* 辺の方位を求める */
  double edge_deg = getDegBtwn2pos(orgn_pos, dst_pos);
/* 眼球に対するグレア発生の有無を判定する． */
  if(FALSE == fndGlrOccredFromEye(sun, edge_deg))
    return FALSE;
/* 車体に対するグレア発生の有無を判定する． */
  if(FALSE == fndGlrOccredFromCar(sun, edge_deg))
    return FALSE;
/* 建物に対するグレア発生の有無を判定する． */
  if(FALSE == fndGlrOccredFromBld(sun, orgn_pos, bld_grd, grd_len, grd_cell_sz))
    return FALSE;

  return TRUE;

}
