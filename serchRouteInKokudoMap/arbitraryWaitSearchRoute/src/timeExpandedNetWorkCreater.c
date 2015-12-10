#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "define.h"

sun_angle_t getSunAngle(double time);
int fndGlrOccred(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_sz);

  /* 時間空間リストの記憶領域を取得する． */
  static struct time_space_list_t *timSpcLstMalloc(void){
  	return (time_space_list_t *)malloc(sizeof(time_space_list_t));
  }

static void initVrtxTimSpce(adj_list_t *edge){
  edge->t_head = timSpcLstMalloc();
  edge->t_old = edge->t_ptr = edge->t_head;
  edge->t_ptr->bgn_tim.hour = 0;
  edge->t_ptr->bgn_tim.min = 0;
}

/* 時間拡大ネットワークの終端時刻を格納する． */
static void setTimSpceEndTim(adj_list_t *edge, tim_t tim){
  if(0 != tim.min){
    edge->t_ptr->end_tim.hour = tim.hour;
    edge->t_ptr->end_tim.min = tim.min - 1;
  }else{
    edge->t_ptr->end_tim.hour = tim.hour - 1;
    edge->t_ptr->end_tim.min = 59;
  }
  return;
}

static void appndTimSpceLst(adj_list_t *edge){
  edge->t_old->next = edge->t_ptr;
  edge->t_old = edge->t_ptr;
  edge->t_ptr = timSpcLstMalloc();

  return;
}

static void setTimSpceBgnTim(adj_list_t *edge, tim_t tim){
  edge->t_ptr->bgn_tim.hour = tim.hour;
  edge->t_ptr->bgn_tim.min = tim.min;

  return;
}

int swtchStatFrmGlr2NonGlr(int bfre_glr_stat, int curr_glr_stat){
  if(TRUE == bfre_glr_stat && FALSE == curr_glr_stat)
    return TRUE;
  else
    return FALSE;
}

int swtchStatFrmNonGlr2Glr(int bfre_glr_stat, int curr_glr_stat){
  if(FALSE == bfre_glr_stat && TRUE == curr_glr_stat)
    return TRUE;
  else
    return FALSE;
}

static void tearoffTimSpcelst(adj_list_t *edge){
  edge->t_ptr->end_tim.hour = 23;
  edge->t_ptr->end_tim.min = 59;

  edge->t_old->next = edge->t_ptr;
  edge->t_old = edge->t_ptr;
  edge->t_old->next = NULL;

  return;
}

static void prntAllTimSpce(adj_list_t *edge){
  edge->t_ptr = edge->t_head;
  while(NULL != edge->t_ptr){
    fprintf(stdout, "bgn %d:%d\n", edge->t_ptr->bgn_tim.hour, edge->t_ptr->bgn_tim.min);
    fprintf(stdout, "end %d:%d\n\n", edge->t_ptr->end_tim.hour, edge->t_ptr->end_tim.min);

    edge->t_ptr = edge->t_ptr->next;
  }

  return;
}

/**
*時間拡大ネットワークを生成する．
*/
void cretTimExpdNtwk(vertex_set_t vrtx_st, build_grid_t** bld_grd,
  xy_coord_t grd_len, grid_size_t grd_cell_sz, int kph){
  int i;
  tim_t tim;
  xy_coord_t orgn_pos, dst_pos;
  int curr_glr_stat, bfre_glr_stat;
  double tim_h, curr_glr_val;
  sun_angle_t sun;

  for(i=1;i<vrtx_st.sz;i++){
    curr_glr_stat = bfre_glr_stat = FALSE;
    vrtx_st.indx[i].ptr = vrtx_st.indx[i].head;
    orgn_pos = vrtx_st.indx[i].pos;

    /* 頂点orgn_posとの辺を持つ頂点に対する繰り返し */
    while(NULL != vrtx_st.indx[i].ptr){
      initVrtxTimSpce(vrtx_st.indx[i].ptr);
      dst_pos = vrtx_st.indx[vrtx_st.indx[i].ptr->num].pos;

      /* 時間を進めていく． */
      for(tim.hour=17;tim.hour<20;tim.hour++){
        for(tim.min=0;tim.min<60;tim.min++){
          /* 時刻を単位[hour]に換算する */
          tim_h = (double)tim.hour + (double)tim.min / 60.0f;
          /* 時刻timにおける太陽の角度を算出する． */
          sun = getSunAngle(tim_h);
          if(sun.elev < 0.0f)
            break;

          /* 時刻timに頂点orgn_posにおいて，頂点dst_posを向いた時のグレア発生の有無を調べる． */
          curr_glr_stat = fndGlrOccred(sun, orgn_pos, dst_pos, bld_grd, grd_len, grd_cell_sz);
          // if(TRUE == curr_glr_stat){
          //   curr_glr_val = getGlrVal();
          // }

          if(TRUE == swtchStatFrmGlr2NonGlr(bfre_glr_stat, curr_glr_stat)){
            setTimSpceEndTim(vrtx_st.indx[i].ptr, tim);
            appndTimSpceLst(vrtx_st.indx[i].ptr);
            setTimSpceBgnTim(vrtx_st.indx[i].ptr, tim);
          }
          else if(TRUE == swtchStatFrmNonGlr2Glr(bfre_glr_stat, curr_glr_stat)){
            setTimSpceEndTim(vrtx_st.indx[i].ptr, tim);
            appndTimSpceLst(vrtx_st.indx[i].ptr);
            setTimSpceBgnTim(vrtx_st.indx[i].ptr, tim);
          }
          bfre_glr_stat = curr_glr_stat;
        }
      }
      /*23:59を入れる*/
      tearoffTimSpcelst(vrtx_st.indx[i].ptr);
      vrtx_st.indx[i].ptr = vrtx_st.indx[i].ptr->next;
    }
  }

  for(i=1;i<vrtx_st.sz;i++){
    vrtx_st.indx[i].ptr = vrtx_st.indx[i].head;
    while(NULL != vrtx_st.indx[i].ptr){
      fprintf(stdout, "*************\n");
      prntAllTimSpce(vrtx_st.indx[i].ptr);
      vrtx_st.indx[i].ptr = vrtx_st.indx[i].ptr->next;
      fprintf(stdout, "*************\n");
    }
  }

  return;
}
