#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"
#include "define.h"

sun_angle_t getSunAngle(double time);
int fndGlrOccred(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_sz);
double getGlrVal(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos);
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);
void dtctFileOpn(FILE **fp, char *fname, char *mode);

double getDegBtwn2pos(struct xy_coord_t p1, struct xy_coord_t p2);

/**
*時間空間リストの記憶領域を取得する．
@return 時間空間リストサイズ(time_space_list_t)分の記憶領域
*/
static struct time_space_list_t *timSpcLstMalloc(void){
  return (time_space_list_t *)malloc(sizeof(time_space_list_t));
}

/**
*辺の時間空間リストに対する初期化
*辺の時間空間リストの先頭にアドレスを割り当てる．
*その後，辺の時間空間の先頭リストの始端時刻に0:00を格納する．
*@param *edge 辺リストのアドレスを参照するポインタ
*@return none
*/
static void initVrtxTimSpce(adj_list_t *edge){
  edge->t_head = timSpcLstMalloc();
  edge->t_old = edge->t_ptr = edge->t_head;
  edge->t_ptr->bgn_tim.hour = 0;
  edge->t_ptr->bgn_tim.min = 0;
  edge->t_ptr->bgn_tim.sec = 0.0f;

  return;
}

/**
*速度kphで頂点orgn_posから頂点dst_posへの移動における所要時間を計算する
*@param kph 移動速度[km/hour]
*@param orgn_pos 移動元の頂点
*@param dst_pos 移動先の頂点
*@return 所要時間[sec]
 */
static double getEdgeTrvSec(double kph, xy_coord_t orgn_pos, xy_coord_t dst_pos){
  /* 時速km/h から 秒速m/secに変換する． */
  double mps = kph / 3.6f;
  vel = mps;
  double dist = getDist(orgn_pos, dst_pos);
  return dist / mps;
}

/**
*移動コストの最大値を保存する
*@param *cost これまでの移動コストの最大値が格納されているアドレスを指すポインタ
*@param trgt_cost 新しく取得した移動コスト
*/
static void setMaxCost(double *cost, double trgt_cost){
  *cost = MAX(*cost, trgt_cost);
  return;
}

/**
 *時間拡大ネットワークの終端時刻を格納する．
 *@param *edge 辺リストのアドレスを参照するポインタ
 *@param tim 時刻
 *@return none
 */
static void setTimSpceEndTim(adj_list_t *edge, tim_t tim){
    edge->t_ptr->end_tim.hour = tim.hour;
    edge->t_ptr->end_tim.min = tim.min;
    edge->t_ptr->end_tim.sec = 0.0f;
  return;
}

/**
*辺の時間空間リストを追加する
*@param *edge 辺リストのアドレスを参照するポインタ
*@return none
 */
static void appndTimSpceLst(adj_list_t *edge){
  edge->t_old->next = edge->t_ptr;
  edge->t_old = edge->t_ptr;
  edge->t_ptr = timSpcLstMalloc();

  return;
}

/**
 *時間拡大ネットワークの始端時刻を格納する．
 *@param *edge 辺リストのアドレスを参照するポインタ
 *@param tim 時刻
 *@return none
 */
static void setTimSpceBgnTim(adj_list_t *edge, tim_t tim){
  edge->t_ptr->bgn_tim.hour = tim.hour;
  edge->t_ptr->bgn_tim.min = tim.min;
  edge->t_ptr->bgn_tim.sec = 0.0f;

  return;
}

/**
*グレア発生状態からグレア未発生状態へ切り替わったを判定する．
*もし，グレア発生状態からグレア未発生状態へ切り替わったならば，TRUEを返す．
*そうでないならば，FALSEを返す．
*@param bfre_glr_stat 直前のグレア状態
*@param curr_glr_stat 現在のグレア状態
*@return グレア発生状態からグレア未発生状態へ切り替わったか否か
*/
int swtchStatFrmGlr2NonGlr(int bfre_glr_stat, int curr_glr_stat){
  if(TRUE == bfre_glr_stat && FALSE == curr_glr_stat)
    return TRUE;
  else
    return FALSE;
}

/**
*グレア未発生状態からグレア発生状態へ切り替わったを判定する．
*もし，グレア未発生状態からグレア発生状態へ切り替わったならば，TRUEを返す．
*そうでないならば，FALSEを返す．
*@param bfre_glr_stat 直前のグレア状態
*@param curr_glr_stat 現在のグレア状態
*@return グレア未発生状態からグレア発生状態へ切り替わったか否か
*/
int swtchStatFrmNonGlr2Glr(int bfre_glr_stat, int curr_glr_stat){
  if(FALSE == bfre_glr_stat && TRUE == curr_glr_stat)
    return TRUE;
  else
    return FALSE;
}

/**
*辺の時間空間リストの終了処理
*@param *edge 辺リストのアドレスを参照するポインタ
*@param cost 辺リストに格納する移動コスト
*return none
*/
static void tearoffTimSpcelst(adj_list_t *edge, double edge_trvt){
  edge->t_ptr->end_tim.hour = 23;
  edge->t_ptr->end_tim.min = 59;
  edge->t_ptr->high_cost = edge_trvt;
  edge->t_ptr->low_cost = edge_trvt;

  edge->t_old->next = edge->t_ptr;
  edge->t_old = edge->t_ptr;
  edge->t_old->next = NULL;

  return;
}

/**
*辺の全ての時間空間リストにおける始端時刻，終端時刻，コストを表示する．
*@param *edge 辺リストのアドレスを参照するポインタ
*@return none
*/
static void printAllEdge(vertex_t *vrtx, int vrtx_sz){
  FILE *fp;
  char *fname = "..\\outputfile\\time_expanded_edges.txt";
  dtctFileOpn(&fp, fname, "wt");

  vertex_t tmp_vrtx;
  int i;
  for(i=1;i<vrtx_sz;i++){
    tmp_vrtx.ptr = vrtx[i].head;
    while(NULL != tmp_vrtx.ptr){
      fprintf(fp, "\n|%5d| -> |%5d|\t", vrtx[i].num, tmp_vrtx.ptr->num);
      fprintf(fp, "trv_sec:%f\n", tmp_vrtx.ptr->edge_trvt);
      tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
      while(NULL != tmp_vrtx.ptr->t_ptr){
        fprintf(fp, "bgn %2d:%2d\t", tmp_vrtx.ptr->t_ptr->bgn_tim.hour, tmp_vrtx.ptr->t_ptr->bgn_tim.min);
        fprintf(fp, "end %2d:%2d\t", tmp_vrtx.ptr->t_ptr->end_tim.hour, tmp_vrtx.ptr->t_ptr->end_tim.min);
        fprintf(fp, "cost:%f\n", tmp_vrtx.ptr->t_ptr->high_cost);
        tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_ptr->next;
      }
      tmp_vrtx.ptr = tmp_vrtx.ptr->next;
    }
  }
  return;
}

/**
*時間拡大ネットワークを生成する．
*@param vrtx_st 交差点を示す頂点セット
*@param **bld_grd 建物を示す頂点セット(各頂点はグリッド単位で小分けに管理されている)
*/
void cretTimExpdNtwk(vertex_set_t vrtx_st, build_grid_t** bld_grd,
  xy_coord_t grd_len, grid_size_t grd_cell_sz, double kph){
  int i;
  tim_t tim;
  xy_coord_t orgn_pos, dst_pos;
  int curr_glr_stat, bfre_glr_stat;
  double tim_h, curr_glr_val, edge_trvt, tmp_cost, edge_deg;
  sun_angle_t sun;
  clock_t start = clock();
  /* 各頂点に対する繰り返し */
  for(i=1;i<vrtx_st.sz;i++){
    vrtx_st.indx[i].ptr = vrtx_st.indx[i].head;
    orgn_pos = vrtx_st.indx[i].pos;

    /* 頂点orgn_posとの辺を持つ頂点に対する繰り返し */
    while(NULL != vrtx_st.indx[i].ptr){
      curr_glr_stat = bfre_glr_stat = FALSE;
      initVrtxTimSpce(vrtx_st.indx[i].ptr);
      dst_pos = vrtx_st.indx[vrtx_st.indx[i].ptr->num].pos;
      /* 辺の移動における所要時間[sec]を求める． */
      tmp_cost = edge_trvt = getEdgeTrvSec(kph, orgn_pos, dst_pos);
      vrtx_st.indx[i].ptr->edge_trvt = edge_trvt;

      edge_deg = getDegBtwn2pos(vrtx_st.indx[i].pos, dst_pos);
      if((0.0f < edge_deg && edge_deg < 110.0f) || 220.0f < edge_deg)
        goto END;

      /* 時間を進めていく． */
      for(tim.hour=17;tim.hour<20;tim.hour++){
        for(tim.min=0;tim.min<60;tim.min++){
          //for(tim.sec=0;tim.sec<60;tim.sec+=1.0f){
            /* 時刻を単位[hour]に換算する */
            tim_h = (double)tim.hour + (double)tim.min / 60.0f + tim.sec / (60.0f * 60.0f);
            /* 時刻timにおける太陽の角度を算出する． */
            sun = getSunAngle(tim_h);
            /* 時刻timに頂点orgn_posにおいて，頂点dst_posを向いた時のグレア発生の有無を調べる． */
            curr_glr_stat = fndGlrOccred(sun, orgn_pos, dst_pos, bld_grd, grd_len, grd_cell_sz);
            /* もし，グレアが発生するならば，その時の移動コストを計算し， */
            /* 同一の時間空間内で最大ならば，保存する． */
            if(TRUE == curr_glr_stat){
              curr_glr_val = getGlrVal(sun, orgn_pos, dst_pos);
              tmp_cost = curr_glr_val * edge_trvt;
              setMaxCost(&vrtx_st.indx[i].ptr->t_ptr->high_cost, tmp_cost);
            }
            /* グレア発生状態からグレア未発生状態に切り替わったならば， */
            /* 終端時刻の格納と，時間空間リストを末尾に追加する． */
            /* この時，切り替わった原因が日没ならば，時刻に対する繰り返しを終了する */
            if(swtchStatFrmGlr2NonGlr(bfre_glr_stat, curr_glr_stat)){
              setTimSpceEndTim(vrtx_st.indx[i].ptr, tim);
              appndTimSpceLst(vrtx_st.indx[i].ptr);
              setTimSpceBgnTim(vrtx_st.indx[i].ptr, tim);
            }
            /* グレア未発生状態からグレア状態に切り替わったならば， */
            /* 終端時刻の格納と，時間空間リストを末尾に追加する */
            else if(swtchStatFrmNonGlr2Glr(bfre_glr_stat, curr_glr_stat)){
              vrtx_st.indx[i].ptr->t_ptr->high_cost = edge_trvt;
              setTimSpceEndTim(vrtx_st.indx[i].ptr, tim);
              appndTimSpceLst(vrtx_st.indx[i].ptr);
              setTimSpceBgnTim(vrtx_st.indx[i].ptr, tim);
            }
            bfre_glr_stat = curr_glr_stat;
            if(sun.elev < 0.0f){
             goto END;
            }
          //}
        }
      }
      END:
      /*辺の時間空間リストに対する終了処理*/
      tearoffTimSpcelst(vrtx_st.indx[i].ptr, edge_trvt);
      vrtx_st.indx[i].ptr = vrtx_st.indx[i].ptr->next;
    }
  }
  if(TRUE == PRINT_EXPENDED_EDGE)
    printAllEdge(vrtx_st.indx, vrtx_st.sz);
  clock_t end = clock();
  fprintf(stdout, "netowork process time %lf[ms]\n", (double)(end - start));

  return;
}

/**
*時刻が一定時刻経過したかを判定する．
*@param bgn_tim 原点時刻
*@param curr_tim 現在時刻
*@intrvl_sec 経過閾値時刻
*/
static int elapsedTimIntrvl(tim_t bgn_tim, tim_t curr_tim, int intrvl_sec){
  int bgn_sec = bgn_tim.hour * 60 * 60 + bgn_tim.min * 60 + bgn_tim.sec;
  int curr_sec = curr_tim.hour * 60 * 60 + curr_tim.min * 60 + curr_tim.sec;

  if(intrvl_sec <= curr_sec - bgn_sec)
    return TRUE;
  else
    return FALSE;
}

static double getEdgeCost(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz, double edge_trvt){

  if(fndGlrOccred(sun, orgn_pos, dst_pos, bld_grd, grd_len, grd_cell_sz)){
    return getGlrVal(sun, orgn_pos, dst_pos) * edge_trvt;
  }
  else{
    return edge_trvt;
  }
}

/**
*静的時間拡大ネットワークを生成する．
*@param vrtx_st 交差点を示す頂点セット
*@param **bld_grd 建物を示す頂点セット(各頂点はグリッド単位で小分けに管理されている)
*/
void cretSttcTimExpdNtwk(vertex_set_t vrtx_st, build_grid_t** bld_grd,
  xy_coord_t grd_len, grid_size_t grd_cell_sz, double kph){
  int i;
  tim_t tim;
  tim.sec = 0.0f;
  xy_coord_t orgn_pos, dst_pos;
  double tim_h, tmp_cost, edge_trvt;
  sun_angle_t sun;
  /* 各頂点に対する繰り返し */
  for(i=1;i<vrtx_st.sz;i++){
    vrtx_st.indx[i].ptr = vrtx_st.indx[i].head;
    orgn_pos = vrtx_st.indx[i].pos;

    /* 頂点orgn_posとの辺を持つ頂点に対する繰り返し */
    while(NULL != vrtx_st.indx[i].ptr){
      initVrtxTimSpce(vrtx_st.indx[i].ptr);
      dst_pos = vrtx_st.indx[vrtx_st.indx[i].ptr->num].pos;
      /* 辺の移動における所要時間[sec]を求める． */
      tmp_cost = edge_trvt = getEdgeTrvSec(kph, orgn_pos, dst_pos);
      vrtx_st.indx[i].ptr->edge_trvt = edge_trvt;

      /* 時間を進めていく． */
      for(tim.hour=17;tim.hour<20;tim.hour++){
        for(tim.min=0;tim.min<60;tim.min++){
          /* 時刻を単位[hour]に換算する */
          tim_h = (double)tim.hour + (double)tim.min / 60.0f;
          /* 時刻timにおける太陽の角度を算出する． */
          sun = getSunAngle(tim_h);
          /* 時刻tにおける移動コストを計算し， */
          /* 同一の時間空間内で最大ならば，保存する． */
          tmp_cost = getEdgeCost(sun, orgn_pos, dst_pos, bld_grd, grd_len, grd_cell_sz, edge_trvt);
          setMaxCost(&vrtx_st.indx[i].ptr->t_ptr->high_cost, tmp_cost);
          /* 時間拡大辺の始端時刻から時間間隔分経過したならば， */
          /* 終端時刻の格納と，時間空間リストを末尾に追加する． */
          /* この時，切り替わった原因が日没ならば，時刻に対する繰り返しを終了する */
          if(elapsedTimIntrvl(vrtx_st.indx[i].ptr->t_ptr->bgn_tim, tim, intrvl_sec)){
            setTimSpceEndTim(vrtx_st.indx[i].ptr, tim);
            appndTimSpceLst(vrtx_st.indx[i].ptr);
            setTimSpceBgnTim(vrtx_st.indx[i].ptr, tim);
          }
          if(sun.elev < 0.0f){
           goto END;
          }
        }
      }
      END:
      /*辺の時間空間リストに対する終了処理*/
      tearoffTimSpcelst(vrtx_st.indx[i].ptr, edge_trvt);
      vrtx_st.indx[i].ptr = vrtx_st.indx[i].ptr->next;
    }
  }
  if(PRINT_EXPENDED_EDGE)
    printAllEdge(vrtx_st.indx, vrtx_st.sz);

  return;
}
