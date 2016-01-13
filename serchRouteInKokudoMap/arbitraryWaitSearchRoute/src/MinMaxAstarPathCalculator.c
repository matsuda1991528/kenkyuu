#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"
#include "define.h"

/* AstarPathCalculator.c */
void initVrtxSet(vertex_t *vrtx, int vrtx_sz, tim_t dptr_tim, int dptr_num,
  tim_expd_edge_t *trgt);
void initList(priority_queue_t *open_lst, priority_queue_t *closed_lst);
void pushEdge2Queue(priority_queue_t *list, tim_expd_edge_t trgt_edge,
  int *lst_sz);
void popCurrEdgeFrmQueue(priority_queue_t *list, tim_expd_edge_t trgt_edge,
  int *lst_sz);
void findTrgtEdgeFrmOpenLst(priority_queue_t *lst_head, tim_expd_edge_t *curr);
void setNeighborVrtxNum(tim_expd_edge_t *neighbor, int orgn_num, int dst_num);
double getEstMateCost(double path_cost, xy_coord_t orgn, xy_coord_t dst);
int neighborInLst(priority_queue_t *lst_head, tim_expd_edge_t neighbor);
  double evlatOptmPath(tim_expd_edge_t curr, vertex_t *vrtx,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz);
void setNeighborTimAndCost(tim_expd_edge_t *neighbor, time_space_list_t *t_ptr);
void setEstAndArrvCost(double *arrv_cost, double *est_cost, double edge_cost,
  double wait_sec, double prev_arrv_cost, double odd_est_cost);
void updateNeighbor(tim_expd_edge_t *neighbor, double est_cost,
  double arrv_cost, tim_expd_edge_t curr, double append_sec);
void updateAstarQueue(time_space_list_t *edge, tim_expd_edge_t neighbor);
void cpyTimExpdEdge(tim_expd_edge_t orgn_edge, tim_expd_edge_t *dst_edge);
void printPriorityQueue(priority_queue_t *head);

/* routeCalculator.c */
void initDijVrtx(vertex_t *vrtx, int vrtx_sz);
void initDptrVrtx(adj_list_t *edge, tim_t dptr);
void exceptTrgtVrtx(tim_expd_edge_t *trgt, int dptr_num, tim_t dptr);
void cpyTim(tim_t orgn, tim_t *dst);
void initTrgtVrtx(tim_expd_edge_t *trgt);
void findTrgtVrtx(tim_expd_edge_t *trgt, vertex_t *vrtx, int vrtx_sz);
int existTimSpac(tim_t trgt, tim_t bgn, tim_t end);
double getWaitTimSec(tim_t dst, tim_t orgn);
void fixTimFormat(tim_t *tim);
void printWaitedPartPath(tim_expd_edge_t curr, vertex_t *vrtx, int dptr_num,
int arrv_num, tim_t dptr);
void printOptmPath(tim_expd_edge_t prev, vertex_t *vrtx, int dptr_num,
int arrv_num, tim_t dptr);

void dtctFileOpn(FILE **fp, char *fname, char *mode);

/* myMath.c */
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);



static void calcMinTimTravCost(double low_cost, tim_t low_tim,
tim_expd_edge_t curr, double odd_est_cost, double *est_cost){

  double wait_sec = getWaitTimSec(low_tim, curr.passed);
  *est_cost = wait_sec + curr.rout_cost + low_cost + odd_est_cost;

  return;
}


static int edgeOccuringGlare(double orgn_cost, double dst_cost){
  if(orgn_cost < dst_cost){
    return TRUE;
  }
  else if(orgn_cost == dst_cost || EMPTY == dst_cost){
    return FALSE;
  }
  else{
    fprintf(stderr, "trvt:%f > cost:%f is consistency\n", orgn_cost, dst_cost);
    exit(1);
  }
}

void MinMaxAstar(vertex_t *vrtx, int vrtx_sz, int dptr_num, int arrv_num, tim_t dptr,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
  priority_queue_t open_lst, closed_lst;
  tim_expd_edge_t curr, neighbor;
  int open_lst_sz = 0, closed_lst_sz = 0, enbl_tim_spac_trans = 0;
  double wait_sec = 0.0f, tmp_est_cost, tmp_rout_cost, neighbor_trv_sec;
  double min_est_cost;
  double odd_est_cost;
  int counter = 0;

  initVrtxSet(vrtx, vrtx_sz, dptr, dptr_num, &curr); //各頂点に対する初期化
  findTrgtVrtx(&curr, vrtx, vrtx_sz);
  exceptTrgtVrtx(&curr, dptr_num, dptr);
  curr.est_cost = getEstMateCost(curr.rout_cost, vrtx[curr.dst_num].pos, vrtx[arrv_num].pos);
  initList(&open_lst, &closed_lst); /* オープンリスト，クローズリストを作成するんじゃ～ */
  pushEdge2Queue(&open_lst, curr, &open_lst_sz); //仮想のEMPTY->dptr_num間の辺をOpenリストにプッシュする．

  while(0 < open_lst_sz){
    findTrgtEdgeFrmOpenLst(open_lst.q_head, &curr); /* オープンリストの中から，最小の推測値を持つ辺を抽出する． */
    if(arrv_num == curr.dst_num){ /* 帰着点を始端とする辺が探索対象辺に選択されたならば，終了 */
      break;
    }
    popCurrEdgeFrmQueue(&open_lst, curr, &open_lst_sz);/* オープンリストから最小の推定値を持つ辺を取り出す． */
    pushEdge2Queue(&closed_lst, curr, &closed_lst_sz);/* 取り出した辺をクローズリストへ移す */
    while(NULL != vrtx[curr.dst_num].ptr){
      setNeighborVrtxNum(&neighbor, curr.dst_num, vrtx[curr.dst_num].ptr->num); /* 隣接辺の始端頂点と終端頂点番号を割り当てる */
      neighbor_trv_sec = vrtx[curr.dst_num].ptr->edge_trvt;
      enbl_tim_spac_trans = FALSE;

      vrtx[curr.dst_num].ptr->t_ptr = vrtx[curr.dst_num].ptr->t_head;
      while(NULL != vrtx[curr.dst_num].ptr->t_ptr){/* 隣接辺の時間空間に対する繰り返し */
        setNeighborTimAndCost(&neighbor, vrtx[curr.dst_num].ptr->t_ptr);
        if(neighborInLst(closed_lst.q_head, neighbor)){ /* 計算済の辺については，計算しない */
          goto SKIP2;
        }
        /* 隣接辺が通過可能かを判定する */
        if(existTimSpac(curr.passed, neighbor.bgn, neighbor.end))
          enbl_tim_spac_trans = TRUE;
        if(enbl_tim_spac_trans){/* 時空間を持つ隣接辺を通過する為の待ち時間を求める．*/
          wait_sec = getWaitTimSec(neighbor.bgn, curr.passed); //待ち時間の算出
          odd_est_cost = getDist(vrtx[neighbor.dst_num].pos, vrtx[arrv_num].pos) / vel;
          setEstAndArrvCost(&tmp_rout_cost, &tmp_est_cost, vrtx[curr.dst_num].ptr->t_ptr->high_cost,
          wait_sec, curr.rout_cost, odd_est_cost);

          if(edgeOccuringGlare(vrtx[curr.dst_num].ptr->edge_trvt,
          vrtx[curr.dst_num].ptr->t_ptr->high_cost)){ //西日が眩しい経路を通過するならば
            /* 極小値を持つ時刻に移動した時の推定値を求める */
            calcMinTimTravCost(vrtx[curr.dst_num].ptr->t_ptr->low_cost,
            vrtx[curr.dst_num].ptr->t_ptr->arch_low_cost_tim, curr, odd_est_cost
            , &min_est_cost);

            if(min_est_cost < tmp_est_cost){
              wait_sec = getWaitTimSec(vrtx[curr.dst_num].ptr->t_ptr->arch_low_cost_tim,
              curr.passed);//待ち時間の算出
              setEstAndArrvCost(&tmp_rout_cost, &tmp_est_cost, vrtx[curr.dst_num].ptr->t_ptr->low_cost,
              wait_sec, curr.rout_cost, odd_est_cost);
              counter++;
            }
          }

          if(!neighborInLst(open_lst.q_head, neighbor)){
            updateNeighbor(&neighbor, tmp_est_cost, tmp_rout_cost, curr, wait_sec + neighbor_trv_sec);
            updateAstarQueue(vrtx[curr.dst_num].ptr->t_ptr, neighbor);
            pushEdge2Queue(&open_lst, neighbor, &open_lst_sz);
            cpyTimExpdEdge(curr, &vrtx[curr.dst_num].ptr->t_ptr->dij_meta.prev);
          }
          else if(neighbor.est_cost > tmp_est_cost){
            updateNeighbor(&neighbor, tmp_est_cost, tmp_rout_cost, curr, wait_sec + neighbor_trv_sec);
            updateAstarQueue(vrtx[curr.dst_num].ptr->t_ptr, neighbor);
            cpyTimExpdEdge(curr, &vrtx[curr.dst_num].ptr->t_ptr->dij_meta.prev);
          }
        }
        SKIP2:
        vrtx[curr.dst_num].ptr->t_ptr = vrtx[curr.dst_num].ptr->t_ptr->next;
      }
      vrtx[curr.dst_num].ptr = vrtx[curr.dst_num].ptr->next;
    }
  }
  if(0 == open_lst_sz){
    fprintf(stdout, "INF, ");
    fprintf(stdout, "\n");
    return;
  }
  //fprintf(stdout, "counter = %d\n", counter);
  printOptmPath(curr, vrtx, dptr_num, arrv_num, dptr);
  printWaitedPartPath(curr, vrtx, dptr_num, arrv_num, dptr);
  fprintf(stdout, "cost %f\n", curr.est_cost);
  //fprintf(stdout, "%d -> %d est:%f arv:%f\n", curr.orgn_num, curr.dst_num, curr.est_cost, curr.rout_cost);
  double accuracy_cost = evlatOptmPath(curr, vrtx, bld_grd, grd_len, grd_cell_sz);
  fprintf(stdout, "final cost:%f\n", accuracy_cost);
  FILE *fp;
  dtctFileOpn(&fp, cost_output_file, "a");
  fprintf(fp, "%f\n", accuracy_cost);
  fclose(fp);
  fprintf(stdout,"\n");

  //printPriorityQueue(closed_lst.q_head);


  return;
}
