#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "header.h"
#include "define.h"

/** 全体地図における全ての建物の位置座標から，
*最東端，最西端，最南端，最北端の位置座標を抽出する
*/
static void setMapEndPos(build_set_t bld_st, xy_coord_t *whle_bgn_pos, xy_coord_t *whle_end_pos){
  bld_st.ptr = bld_st.head;
  bld_st.ptr = bld_st.ptr->next;

/* 初期化 */
  whle_bgn_pos->x = whle_end_pos->x = bld_st.ptr->pos.x;
  whle_bgn_pos->y = whle_end_pos->y = bld_st.ptr->pos.y;
  bld_st.ptr = bld_st.ptr->next;

/* 全体地図における全ての建物リストを走査していく． */
  while(bld_st.ptr != NULL){
    whle_bgn_pos->x = MIN(whle_bgn_pos->x, bld_st.ptr->pos.x);
    whle_bgn_pos->y = MIN(whle_bgn_pos->y, bld_st.ptr->pos.y);
    whle_end_pos->x = MAX(whle_end_pos->x, bld_st.ptr->pos.x);
    whle_end_pos->y = MAX(whle_end_pos->y, bld_st.ptr->pos.y);

    bld_st.ptr = bld_st.ptr->next;
  }
  return;
}

/**
*各グリッド地図に始端座標(最小xy座標)と終端座標(最大xy座標)を割り当てる．
*/
void assgnGrdArea(build_grid_t **bld_grd, xy_coord_t grd_len,
  xy_coord_t whle_bgn_pos, xy_coord_t whle_end_pos, grid_size_t grd_sz){

  bld_grd[0][0].bgn.x = bld_grd[0][0].bgn.y = 0.0f;
  bld_grd[0][0].end.x = whle_bgn_pos.x;
  bld_grd[0][0].end.y = whle_bgn_pos.y;

  int i, j;
  for(i=1;i<grd_sz.sn;i++){
    for(j=1;j<grd_sz.ew;j++){
      bld_grd[i][j].bgn.x = whle_bgn_pos.x + grd_len.x * (j - 1);
      bld_grd[i][j].bgn.y = whle_bgn_pos.y + grd_len.y * (i - 1);
      bld_grd[i][j].end.x = whle_bgn_pos.x + grd_len.x * j;
      bld_grd[i][j].end.y = whle_bgn_pos.y + grd_len.y * i;

      if(i == grd_sz.sn -1)
        bld_grd[i][j].end.y = whle_end_pos.y * 2;
      if(j == grd_sz.ew - 1)
        bld_grd[i][j].end.x = whle_end_pos.x * 2;
    }
  }
  return;
}

/* struct build_pos_t型分のメモリを確保し，struct build_pos_t*　へキャストする */
static struct build_pos_t *buildMalloc(void){
	return (struct build_pos_t *)malloc(sizeof(struct build_pos_t));
}

/**
*各グリッド地図における建物の先頭リスト用の記憶領域を取得する
*/
static void initBldLstOnGrd(build_grid_t **bld_grd, grid_size_t grd_sz){
  int i, j;
	for(i=1;i<grd_sz.sn;i++){
		for(j=1;j<grd_sz.ew;j++){
			bld_grd[i][j].head = buildMalloc();
			bld_grd[i][j].old = bld_grd[i][j].head;
		}
	}
	return;
}

void cpyBld(build_pos_t* trgt_bld, build_pos_t* orgn_bld){
  trgt_bld->pos.x = orgn_bld->pos.x;
  trgt_bld->pos.y = orgn_bld->pos.y;
  trgt_bld->wdth = orgn_bld->wdth;
}

/**
全体地図内の全ての建物を各々のグリッド地図に割り当てる．
詳細は以下の通り：
まず，各建物の位置座標に基づいて，各建物が所属するグリッド地図の索引を割り当てる
次に，割り当てられた索引番号を持つグリッド地図の建物リストの末尾に建物を追加する．
これらの操作を全ての建物に適用する事で，各々のグリッド地図に建物を割り当てる．
*/
static void allctBldToGrd(build_set_t bld_st, build_grid_t **bld_grd,
  grid_size_t grd_sz, xy_coord_t grd_len){

  int indx_sn, indx_ew;
  int i, j;
/* 各グリッド地図における建物リストの初期化 */
  initBldLstOnGrd(bld_grd, grd_sz);

/* 建物の先頭リストはダミーなのじゃ！ */
  bld_st.ptr = bld_st.head;
  bld_st.ptr = bld_st.ptr->next;

  while(bld_st.ptr != NULL){
    indx_sn = (int)(bld_st.ptr->pos.y - (int)bld_grd[1][1].bgn.y) / (int)grd_len.y;
    indx_ew = (int)(bld_st.ptr->pos.x - (int)bld_grd[1][1].bgn.y) / (int)grd_len.x;
    if(indx_sn < 1)
      indx_sn = 1;
    if(indx_ew < 1)
      indx_ew = 1;

    bld_grd[indx_sn][indx_ew].ptr = buildMalloc();
    cpyBld(bld_grd[indx_sn][indx_ew].ptr, bld_st.ptr);
    bld_grd[indx_sn][indx_ew].old->next = bld_grd[indx_sn][indx_ew].ptr;
    bld_grd[indx_sn][indx_ew].old = bld_grd[indx_sn][indx_ew].ptr;
    bld_st.ptr = bld_st.ptr->next;
  }
  /* 各グリッド地図における建物リストの末尾にNULLを挿入する */
  for(i=1;i<grd_sz.sn;i++){
    for(j=1;j<grd_sz.ew;j++){
      bld_grd[i][j].old->next = NULL;
    }
  }
  return;
}

build_grid_t **cretGrdMap(build_set_t bld_st, xy_coord_t grd_len){
  xy_coord_t whle_bgn_pos, whle_end_pos;
  grid_size_t grd_sz;
  build_grid_t **bld_grd = NULL;

/* 全体地図の始端(最小xy座標)と終端(最大xy座標)を求める */
  setMapEndPos(bld_st, &whle_bgn_pos, &whle_end_pos);
/* 南北と東西方向におけるグリッド地図の要素数を求める */
  grd_sz.sn = (int)(floor((whle_end_pos.y - whle_bgn_pos.y) / grd_len.y) + 2);
  grd_sz.ew = (int)(floor((whle_end_pos.x - whle_bgn_pos.x) / grd_len.x) + 2);

/* グリッド地図の枚数分の記憶領域を取得する */
  //bld_grd = (build_grid_t *)malloc(sizeof(build_grid_t) * grd_sz.sn);
  bld_grd = (build_grid_t **)malloc(sizeof(build_grid_t) * grd_sz.sn);
  if(NULL == bld_grd){
    fprintf(stderr, "bld_grd memory allocation failed\n");
    exit(1);
  }
  int i;
  for(i=0;i<grd_sz.sn;i++){
    bld_grd[i] = (build_grid_t *)malloc(sizeof(build_grid_t) * grd_sz.ew);
    if(NULL == bld_grd[i]){
      fprintf(stderr, "bld_grid[%d] memory allocation failed\n", i);
    }
  }

  assgnGrdArea(bld_grd, grd_len, whle_bgn_pos, whle_end_pos, grd_sz);
  allctBldToGrd(bld_st, bld_grd, grd_sz, grd_len);

  return bld_grd;
}
