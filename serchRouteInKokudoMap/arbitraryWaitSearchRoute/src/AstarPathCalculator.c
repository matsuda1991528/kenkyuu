#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "header.h"
#include "define.h"

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

/* myMath.c */
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);

sun_angle_t getSunAngle(double time);

double getGlrVal(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos);

int fndGlrOccred(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_sz);

void dtctFileOpn(FILE **fp, char *fname, char *mode);

/* 優先度つきキューを表す構造体 */
typedef struct priority_queue_t{
  tim_expd_edge_t edge;
  int indx;
  struct priority_queue_t *q_head;
  struct priority_queue_t *q_ptr;
  struct priority_queue_t *q_old;
  struct priority_queue_t *next;
}priority_queue_t;

/* 優先度つきキューに頂点を格納する為の記憶領域の取得 */
static struct priority_queue_t *priorityQueueMalloc(void){
  return (priority_queue_t *)malloc(sizeof(priority_queue_t));
}

static void initList(priority_queue_t *open_lst, priority_queue_t *closed_lst){
  open_lst->q_old = open_lst->q_ptr = open_lst->q_head = priorityQueueMalloc();
  closed_lst->q_old = closed_lst->q_ptr = closed_lst->q_head = priorityQueueMalloc();

  return;
}

/* 時間空間毎に区別された辺をコピーする． */
static void cpyTimExpdEdge(tim_expd_edge_t orgn_edge, tim_expd_edge_t *dst_edge){
  /* 辺の始端時刻，終端時刻，通過し終わった時刻をコピーする． */
  cpyTim(orgn_edge.bgn, &dst_edge->bgn);
  cpyTim(orgn_edge.end, &dst_edge->end);
  cpyTim(orgn_edge.passed, &dst_edge->passed);
  dst_edge->orgn_num = orgn_edge.orgn_num;
  dst_edge->dst_num = orgn_edge.dst_num;
  dst_edge->rout_cost = orgn_edge.rout_cost;
  dst_edge->est_cost = orgn_edge.est_cost;

  return;
}

/**
*隣接辺の始端頂点番号と終端頂点番号を割り当てる．
*@param *neighbor 隣接辺データが格納されているアドレスを指すポインタ
*@param orgn_num 始端頂点番号
*@param dst_num 終端頂点番号
*@return none
*/
static void setNeighborVrtxNum(tim_expd_edge_t *neighbor, int orgn_num, int dst_num){
  neighbor->orgn_num = orgn_num;
  neighbor->dst_num = dst_num;

  return;
}

/**
*隣接辺の始端時刻と終端時刻，そして推定コストを割り当てる．
*@param *neighbor 隣接辺データが格納されているアドレスを指すポインタ
*@param *t_ptr コピー元データが格納されているアドレスを指すポインタ
*@return none
*/
static void setNeighborTimAndCost(tim_expd_edge_t *neighbor, time_space_list_t *t_ptr){
  cpyTim(t_ptr->bgn_tim, &neighbor->bgn);
  cpyTim(t_ptr->end_tim, &neighbor->end);
  //neighbor->edge_cost = t_ptr->edge_cost;
  neighbor->est_cost = t_ptr->dij_meta.est_cost;

  return;
}

/*  */
static void pushEdge2Queue(priority_queue_t *list, tim_expd_edge_t trgt_edge,
int *lst_sz){

  /* リストの末尾までリストを辿る． */
  if(0 < *lst_sz){
    list->q_ptr = list->q_head;
    while(NULL != list->q_ptr->next){
      list->q_ptr = list->q_ptr->next;
    }
    list->q_old = list->q_ptr;
    list->q_ptr = list->q_ptr->next; //NULLが挿入される．
    list->q_ptr = priorityQueueMalloc();
  }
  else if(NULL == list->q_head){
    list->q_old = list->q_ptr = list->q_head = priorityQueueMalloc();
  }
  cpyTimExpdEdge(trgt_edge, &list->q_ptr->edge);
  list->q_old->next = list->q_ptr;
  list->q_old = list->q_ptr;
  list->q_old->next = NULL;

  *lst_sz+=1;
  return;
}

/**
*二つの時刻が等しいかを判定する．
*/
static int isEqualTim(tim_t orgn, tim_t trgt){
  if(orgn.hour == trgt.hour && orgn.min == trgt.min && orgn.sec == trgt.sec)
    return TRUE;
  else
    return FALSE;
}

/**
*二つの辺が等しいかを判定する．
*/
static int isEqualPairOfEdge(tim_expd_edge_t orgn, tim_expd_edge_t terget){
  if(orgn.orgn_num == terget.orgn_num && orgn.dst_num == terget.dst_num){
    if(isEqualTim(orgn.bgn, terget.bgn) && isEqualTim(orgn.end, terget.end)){
      return TRUE;
    }
  }
  return FALSE;
}

/**
*優先度つきキューにストアされている辺をポップする．
*@param lst_ptr
*@param lst_old
*@param lst_head
*@param trgt_edge
*@param lst_sz
*/
static void popCurrEdgeFrmQueue(priority_queue_t *list, tim_expd_edge_t trgt_edge,
int *lst_sz){

  list->q_old = list->q_ptr = list->q_head;
  while(NULL != list->q_ptr){
    if(isEqualPairOfEdge(list->q_ptr->edge, trgt_edge)){
      if(list->q_ptr == list->q_head){
        if(1 < *lst_sz)
          list->q_head = list->q_ptr->next;
        else{
          free(list->q_head);
          list->q_head = list->q_ptr = NULL;
        }
      }
      else
        list->q_old->next = list->q_ptr->next;
      *lst_sz -= 1;
      return;
    }
    list->q_old = list->q_ptr;
    list->q_ptr = list->q_ptr->next;
  }
}

/**
*オープンリスト内から探索対象(最小の推測値を持つ)辺を抽出する．
*@param lst_head 先頭リスト
*@param curr 探索対象辺
*/
static void findTrgtEdgeFrmOpenLst(priority_queue_t *lst_head, tim_expd_edge_t *curr){
  priority_queue_t *lst_ptr = lst_head;
  double min_est_cost = EMPTY;
  /* オープンリストに格納された全ての辺に対する繰り返しにおいて， */
  /* 最小コストが空，あるいは更に低い推定値を持つ辺が見つかった場合， */
  /* 探索対象辺currと最小コストmin_est_costを更新する */
  while(NULL != lst_ptr){
    if(min_est_cost > lst_ptr->edge.est_cost || EMPTY == min_est_cost){
      cpyTimExpdEdge(lst_ptr->edge, curr);
      min_est_cost = lst_ptr->edge.est_cost;
    }
    lst_ptr = lst_ptr->next;
  }

}

/*
static void PrintTimExpdEdge(tim_expd_edge_t trgt_edge){
  fprintf(stdout, "%d -> %d\n", trgt_edge.orgn_num, trgt_edge.dst_num);
  fprintf(stdout, "bgn:%2d:%2d:%3.1f\t", trgt_edge.bgn.hour, trgt_edge.bgn.min, trgt_edge.bgn.sec);
  fprintf(stdout, "end:%2d:%2d:%3.1f\t", trgt_edge.end.hour, trgt_edge.end.min, trgt_edge.end.sec);
  fprintf(stdout, "passed:%2d:%2d:%3.1f\n", trgt_edge.passed.hour, trgt_edge.passed.min, trgt_edge.passed.sec);
  fprintf(stdout, "passed cost = %f\tpassed e_cost = %f\n", trgt_edge.rout_cost, trgt_edge.est_cost);

}
*/

static int neighborInLst(priority_queue_t *lst_head, tim_expd_edge_t neighbor){
  priority_queue_t *lst_ptr = lst_head;
  while(NULL != lst_ptr){
    if(lst_ptr->edge.orgn_num  == neighbor.orgn_num &&
    lst_ptr->edge.dst_num == neighbor.dst_num){
      if(isEqualTim(lst_ptr->edge.bgn, neighbor.bgn) &&
      isEqualTim(lst_ptr->edge.end, neighbor.end))
        return TRUE;
    }
    lst_ptr = lst_ptr->next;
  }
  return FALSE;
}

static void printPriorityQueue(priority_queue_t *head){
  priority_queue_t *ptr = head;
  fprintf(stderr, "***************************\n");
  fprintf(stderr, "following is priority queue\n");
  while(NULL != ptr){
    fprintf(stderr, "%2d->%2d bgn:%2d:%2d:%3.1f end:%2d:%2d:%3.1f\tpassed:%2d:%2d:%3.1f est cost:%f rout cost:%f\n", ptr->edge.orgn_num,
    ptr->edge.dst_num, ptr->edge.bgn.hour, ptr->edge.bgn.min, ptr->edge.bgn.sec,
    ptr->edge.end.hour, ptr->edge.end.min, ptr->edge.end.sec,
    ptr->edge.passed.hour, ptr->edge.passed.min, ptr->edge.passed.sec, ptr->edge.est_cost, ptr->edge.rout_cost);
    ptr = ptr->next;
  }
  fprintf(stderr, "***************************\n");
  return;
}

/**
*経路のコストの推定値f(n)を計算する．
*ここで，経路のコストの推定値は以下の式から求める事が出来る．
*f(n) = g(n) + h(n)
ここで，g(n)は，出発点から頂点nまでの最小コスト，
*h(n)は，頂点nから帰着点までの最小コストの推定値を表す．
*本プログラムにおいて，推定値h(n)は,2次元ユークリッド空間上の
頂点nと帰着点までの直線距離を用いる．
@param path_cost 出発地点から頂点nまでの最小コスト
@param orgn 頂点nの位置座標
@param dst 帰着点の位置座標
@return 経路のコストの推定値f(n)
*/
static double getEstMateCost(double path_cost, xy_coord_t orgn, xy_coord_t dst){
  double dist = getDist(orgn, dst);
  return dist / vel + path_cost;
}

/* 隣接辺のコスト，通過終了時刻，を更新する． */
static void updateNeighbor(tim_expd_edge_t *neighbor, double est_cost,
double arrv_cost, tim_expd_edge_t curr, double append_sec){
  /* 推定コスト，到達コストの更新 */
  neighbor->est_cost = est_cost;
  neighbor->rout_cost = arrv_cost;
  /* 直前に通過した辺を通過終了時刻に隣接辺の所要時間と待ち時間を加算する． */
  cpyTim(curr.passed, &neighbor->passed);
  neighbor->passed.sec += append_sec;
  fixTimFormat(&neighbor->passed);
  return;
}

static void updateAstarQueue(time_space_list_t *edge, tim_expd_edge_t neighbor){
  edge->dij_meta.est_cost = neighbor.est_cost;
  edge->dij_meta.path_cost = neighbor.rout_cost;
  cpyTim(neighbor.passed, &edge->dij_meta.arrv_tim);

  return;
}

static double calcWaitSec(tim_expd_edge_t prev, tim_expd_edge_t curr){
  if(existTimSpac(prev.passed, curr.bgn, curr.end)){
    return 0.0f;
  }
  else{
    double curr_dptr_sec = curr.bgn.hour * 60.0f * 60.0f + curr.bgn.min * 60.0f + curr.bgn.sec;
    double prev_passed_sec = prev.passed.hour * 60.0f * 60.0f + prev.passed.min * 60.0f + prev.passed.sec;
    return curr_dptr_sec - prev_passed_sec;
  }
}

static double calcEdgeCost(tim_expd_edge_t prev, tim_expd_edge_t curr,
double edge_trvt, vertex_t *vrtx, build_grid_t **bld_grd, xy_coord_t grd_len,
grid_size_t grd_cell_sz, double wait_sec){
  /* 探索対象辺の出発時刻(単位:hour)を求める */
  // double tim_h = (double)prev.passed.hour + (double)prev.passed.min/60.0f
  // + (prev.passed.sec + wait_sec) /3600.0f;
  prev.passed.sec += wait_sec;
  fixTimFormat(&prev.passed);
  if(prev.passed.sec >= 59.9999){
    prev.passed.sec = 0.0f;
    prev.passed.min += 1;
  }
  double tim_h = (double)prev.passed.hour + (double)prev.passed.min/60.0f;

  /* 時刻tim_h における太陽角度を求める */
  sun_angle_t sun = getSunAngle(tim_h);

  /* グレアが発生する場合，グレア値を加味した不快度を計算する． */
  /* そうでない場合は，辺の移動に対する所要時間が不快度となる． */
  if(fndGlrOccred(sun, vrtx[curr.orgn_num].pos, vrtx[curr.dst_num].pos, bld_grd,
  grd_len, grd_cell_sz)){
    //fprintf(stdout, "glare value = %f\t", getGlrVal(sun, vrtx[curr.orgn_num].pos, vrtx[curr.dst_num].pos));
    return getGlrVal(sun, vrtx[curr.orgn_num].pos, vrtx[curr.dst_num].pos) * edge_trvt;
  }
  else{
    return edge_trvt;
  }
}

/* 経路の不快度を動的道路網において，評価する． */
double evlatOptmPath(tim_expd_edge_t curr, vertex_t *vrtx,
  build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
  vertex_t tmp_vrtx;
  tim_expd_edge_t prev;
  tim_t dptr;
  double edge_trvt, edge_cost, sum_cost = 0.0f;
  double wait_sec;

  while(EMPTY != curr.orgn_num){
    /* 直前に通過した辺を算出する */
    tmp_vrtx.ptr = vrtx[curr.orgn_num].head;
    while(NULL != tmp_vrtx.ptr){
      if(tmp_vrtx.ptr->num == curr.dst_num){
        edge_trvt = tmp_vrtx.ptr->edge_trvt;
        tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
        while(NULL != tmp_vrtx.ptr->t_ptr){
          if(existTimSpac(curr.bgn, tmp_vrtx.ptr->t_ptr->bgn_tim, tmp_vrtx.ptr->t_ptr->end_tim)){
            prev = tmp_vrtx.ptr->t_ptr->dij_meta.prev;
            // fprintf(stdout, "%2d:%2d:%3.1f <- %2d:%2d:%3.1f\n", curr.passed.hour,
            // curr.passed.min, curr.passed.sec, prev.passed.hour, prev.passed.min, prev.passed.sec);
            goto OUT;
            if(EMPTY == prev.orgn_num)
              break;
          }
          tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_ptr->next;
        }
      }
      tmp_vrtx.ptr = tmp_vrtx.ptr->next;
    }
    OUT:
    /* 待ち時間の計算 */
    wait_sec = calcWaitSec(prev, curr);
    edge_cost = calcEdgeCost(prev, curr, edge_trvt, vrtx, bld_grd, grd_len,
    grd_cell_sz, wait_sec);

    cpyTim(prev.passed, &dptr);
    dptr.sec += wait_sec;
    fixTimFormat(&dptr);

    // if(wait_sec != 0.0f)
    //   fprintf(stdout, "wait_sec = %f\t", wait_sec);
    // fprintf(stdout, "%d <- %d \t", curr.dst_num, curr.orgn_num);
    // fprintf(stdout, "prev passed time:%2d:%2d:%3.1f\t", prev.passed.hour, prev.passed.min, prev.passed.sec);
    // fprintf(stdout, "curr dptr time:%2d:%2d:%3.12f\n", dptr.hour, dptr.min, dptr.sec);

    //// fprintf(stdout, "%d <- %d \t", prev.dst_num, prev.orgn_num);
    //// fprintf(stdout, "%f + %f + %f -> ", sum_cost, edge_cost, wait_sec);
    sum_cost += (edge_cost + wait_sec);
    ////fprintf(stdout, "%f\t %f\n", sum_cost, curr.rout_cost);

    // if((int)(edge_cost + wait_sec) <= (int)(curr.rout_cost - prev.rout_cost))
    //   fprintf(stdout, "edge cost is equal %lf \t %lf\n", edge_cost + wait_sec, curr.rout_cost - prev.rout_cost);
    // else
    //   fprintf(stdout, "\t\t\tedge cost is not equal \t %lf \t %lf\n", edge_cost + wait_sec, curr.rout_cost - prev.rout_cost);
    curr = prev;
  }
  fprintf(stdout, "final cost = %f\n", sum_cost);
  return sum_cost;
}

/**
*A starアルゴリズムによる経路探索を行う．
*/
void A_star(vertex_t *vrtx, int vrtx_sz, int dptr_num, int arrv_num, tim_t dptr,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
  priority_queue_t open_lst; //計算中の辺を格納しておくための優先度つきキュー
  priority_queue_t closed_lst; //計算済の辺を格納しておく為の優先度つきキュー
  tim_expd_edge_t curr; //現在探索中の辺
  tim_expd_edge_t neighbor;
  int open_lst_sz = 0, closed_lst_sz = 0, enbl_tim_spac_trans = 0;
  double wait_sec = 0.0f, tmp_est_cost, neighbor_edge_cost, tmp_rout_cost, neighbor_trv_sec;
  initDijVrtx(vrtx, vrtx_sz);/* 各辺に対する経路探索用のメタデータを全て空にする． */
  initDptrVrtx(vrtx[dptr_num].ptr, dptr);
  initTrgtVrtx(&curr);   /* 探索対象辺の中身を空にする． */

  findTrgtVrtx(&curr, vrtx, vrtx_sz);
  exceptTrgtVrtx(&curr, dptr_num, dptr);
  curr.est_cost = getEstMateCost(curr.rout_cost, vrtx[curr.dst_num].pos, vrtx[arrv_num].pos);

  initList(&open_lst, &closed_lst); /* オープンリスト，クローズリストを作成するんじゃ～ */
  pushEdge2Queue(&open_lst, curr, &open_lst_sz); //仮想のEMPTY->dptr_num間の辺をOpenリストにプッシュする．

  clock_t start = clock();

  int iterater = 0;
  /* Openリストが空ならば，探索を終了する． */
  while(0 < open_lst_sz){
    iterater++;
    findTrgtEdgeFrmOpenLst(open_lst.q_head, &curr); /* オープンリストの中から，最小の推測値を持つ辺を抽出する． */
    if(arrv_num == curr.dst_num){ /* 帰着点を始端とする辺が探索対象辺に選択されたならば，終了 */
      break;
    }

    /* オープンリストから最小の推定値を持つ辺を取り出す． */
    popCurrEdgeFrmQueue(&open_lst, curr, &open_lst_sz);
    /* 取り出した辺をクローズリストへ移す */
    pushEdge2Queue(&closed_lst, curr, &closed_lst_sz);

    if(MONITOR_PRIORITY_LIST){
        printPriorityQueue(open_lst.q_head);
        printPriorityQueue(closed_lst.q_head);
    }

    while(NULL != vrtx[curr.dst_num].ptr){ /* 隣接辺に対する繰り返し */
      /* 隣接辺の始端頂点と終端頂点番号を割り当てる */
      setNeighborVrtxNum(&neighbor, curr.dst_num, vrtx[curr.dst_num].ptr->num);
      neighbor_trv_sec = vrtx[curr.dst_num].ptr->edge_trvt;
      enbl_tim_spac_trans = FALSE;

      vrtx[curr.dst_num].ptr->t_ptr = vrtx[curr.dst_num].ptr->t_head;
      while(NULL != vrtx[curr.dst_num].ptr->t_ptr){ /* 隣接辺の時間空間に対する繰り返し */
        /* 隣接辺の始端時刻と終端時刻，そして推定コストを割り当てる． */
        setNeighborTimAndCost(&neighbor, vrtx[curr.dst_num].ptr->t_ptr);

        if(neighborInLst(closed_lst.q_head, neighbor)){ /* 計算済の辺については，計算しない */
          goto SKIP;
        }
        /* 隣接辺が通過可能かを判定する */
        if(existTimSpac(curr.passed, neighbor.bgn, neighbor.end))
          enbl_tim_spac_trans = TRUE;

        if(enbl_tim_spac_trans){ /* 時空間を持つ隣接辺を通過する為の待ち時間を求める． */
          wait_sec = getWaitTimSec(neighbor.bgn, curr.passed); //待ち時間の算出
          neighbor_edge_cost = vrtx[curr.dst_num].ptr->t_ptr->high_cost; //エッジのコスト
          tmp_rout_cost = curr.rout_cost + wait_sec + neighbor_edge_cost; //隣接辺を通過した時のコストg(n)
          tmp_est_cost = tmp_rout_cost + (getDist(vrtx[neighbor.dst_num].pos, vrtx[arrv_num].pos) / vel); /* 推定値f(n)の計算 */

          /* 未探索の辺を探索した場合,オープンリストに追加する．この時，辺の到達コスト，直前に通過した辺も更新する． */
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
        SKIP:
        vrtx[curr.dst_num].ptr->t_ptr = vrtx[curr.dst_num].ptr->t_ptr->next;
      }
      vrtx[curr.dst_num].ptr = vrtx[curr.dst_num].ptr->next;
    }
  }
  if(0 == open_lst_sz)
    fprintf(stdout, "failed arrive goal\n");
  else
    fprintf(stdout, "goal to success\n");
  printOptmPath(curr, vrtx, dptr_num, arrv_num, dptr);
  printWaitedPartPath(curr, vrtx, dptr_num, arrv_num, dptr);
  fprintf(stdout, "after goal\n");
  //fprintf(stdout, "%d -> %d est:%f arv:%f\n", prev.orgn_num, prev.dst_num, prev.est_cost, prev.rout_cost);
  fprintf(stdout, "%d -> %d est:%f arv:%f\n", curr.orgn_num, curr.dst_num, curr.est_cost, curr.rout_cost);
  clock_t end = clock();
  fprintf(stdout, "process time %lf[ms]\n", (double)(end - start));
  double accuracy_cost = evlatOptmPath(curr, vrtx, bld_grd, grd_len, grd_cell_sz);

  FILE *fp;
  dtctFileOpn(&fp, cost_output_file, "a");
  fprintf(fp, "%d, %d, %f, %f\n", dptr_num, arrv_num, curr.rout_cost, accuracy_cost);
  fclose(fp);

  return;
}
