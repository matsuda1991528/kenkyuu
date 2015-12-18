#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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

/* myMath.c */
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);

/* 優先度つきキューを表す構造体 */
typedef struct priority_queue_t{
  tim_expd_edge_t edge;
  int indx;
  struct priority_queue_t *q_head;
  struct priority_queue_t *q_ptr;
  struct priority_queue_t *q_old;
  struct priority_queue_t *next;
}priority_queue_t;

typedef struct neighbor_edge_t{
  int orgn_num;
  int dst_num;
  tim_t bgn;
  tim_t end;
  double edge_cost;
  double est_cost;
}neighbor_edge_t;

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
  cpyTim(orgn_edge.curr, &dst_edge->curr);
  dst_edge->orgn_num = orgn_edge.orgn_num;
  dst_edge->dst_num = orgn_edge.dst_num;
  dst_edge->rout_cost = orgn_edge.rout_cost;
  dst_edge->est_cost = orgn_edge.est_cost;

  return;
}

static void setNeighborVrtxNum(tim_expd_edge_t *neighbor, int orgn_num, int dst_num){
  neighbor->orgn_num = orgn_num;
  neighbor->dst_num = dst_num;

  return;
}

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
   fprintf(stdout, "list size = %d\n", *lst_sz);
  // fprintf(stderr, "ptr = %p head = %p\n", list->q_ptr, list->q_head);
  if(0 < *lst_sz){
    list->q_ptr = list->q_head;
    while(NULL != list->q_ptr->next){
      list->q_ptr = list->q_ptr->next;
    }
    list->q_old = list->q_ptr;
    list->q_ptr = list->q_ptr->next; //NULLが挿入される．
    fprintf(stdout, "before append address:%p\n", list->q_ptr);
    list->q_ptr = priorityQueueMalloc();
    fprintf(stdout, "after append address:%p\n", list->q_ptr);
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
  if(orgn.orgn_num == terget.orgn_num && orgn.dst_num && terget.dst_num){
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
      //dprintf("poped edge : %d -> %d\n", trgt_edge.orgn_num, trgt_edge.dst_num);
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

static void findTrgtEdgeFrmOpenLst(priority_queue_t *lst_head, tim_expd_edge_t *curr){
  priority_queue_t *lst_ptr = lst_head;
  double min_est_cost = EMPTY;
  while(NULL != lst_ptr){
    fprintf(stdout, "mincost:%f trgtcost:%f\n", min_est_cost, lst_ptr->edge.est_cost);
    if(min_est_cost > lst_ptr->edge.est_cost || EMPTY == min_est_cost){
      //dprintf("mincost %f -> %f\n", min_est_cost, lst_ptr->edge.est_cost);
      //dprintf("mincost edge : %d -> %d\n", lst_ptr->edge.orgn_num, lst_ptr->edge.dst_num);
      cpyTimExpdEdge(lst_ptr->edge, curr);
      min_est_cost = lst_ptr->edge.est_cost;
    }
    lst_ptr = lst_ptr->next;
  }

}

static void PrintTimExpdEdge(tim_expd_edge_t trgt_edge){
  fprintf(stdout, "%d -> %d\n", trgt_edge.orgn_num, trgt_edge.dst_num);
  fprintf(stdout, "bgn:%2d:%2d:%3.1f\t", trgt_edge.bgn.hour, trgt_edge.bgn.min, trgt_edge.bgn.sec);
  fprintf(stdout, "end:%2d:%2d:%3.1f\t", trgt_edge.end.hour, trgt_edge.end.min, trgt_edge.end.sec);
  fprintf(stdout, "passed:%2d:%2d:%3.1f\n", trgt_edge.curr.hour, trgt_edge.curr.min, trgt_edge.curr.sec);
  fprintf(stdout, "passed cost = %f\tpassed e_cost = %f\n", trgt_edge.rout_cost, trgt_edge.est_cost);

}

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
    ptr->edge.curr.hour, ptr->edge.curr.min, ptr->edge.curr.sec, ptr->edge.est_cost, ptr->edge.rout_cost);
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
  return dist + path_cost;
}
/**
*A starアルゴリズムによる経路探索を行う．
*/
void A_star(vertex_t *vrtx, int vrtx_sz, int dptr_num, int arrv_num, tim_t dptr){
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
  //PrintTimExpdEdge(curr);
  initList(&open_lst, &closed_lst); /* オープンリスト，クローズリストを作成するんじゃ～ */

  //fprintf(stderr, "before first push\n");
  //printPriorityQueue(open_lst.q_head);
  pushEdge2Queue(&open_lst, curr, &open_lst_sz); //仮想のEMPTY->dptr_num間の辺をOpenリストにプッシュする．
  //fprintf(stderr, "after first push\n");
  //printPriorityQueue(open_lst.q_head);
  //fprintf(stderr, "before a star curr %d -> %d cost %f\n", curr.orgn_num, curr.dst_num, curr.est_cost);
  /* Openリストが空ならば，探索を終了する． */
  //while(0 < open_lst_sz){
  int i;
  for(i=0;i<5;i++){
    findTrgtEdgeFrmOpenLst(open_lst.q_head, &curr); /* オープンリストの中から，最小の推測値を持つ辺を抽出する． */
    fprintf(stderr, "\n--------------------------------------------------------------------------\n");
    fprintf(stdout, "curr %d -> %d\tbgn:%2d:%2d:%3.1f\tend:%2d:%2d:%3.1f\tpassed:%2d:%2d:%3.1f\n",
    curr.orgn_num, curr.dst_num, curr.bgn.hour, curr.bgn.min, curr.bgn.sec, curr.end.hour, curr.end.min,
    curr.end.sec, curr.curr.hour, curr.curr.min, curr.curr.sec);
    fprintf(stderr, "--------------------------------------------------------------------------\n\n");
    if(arrv_num == curr.orgn_num){ /* 帰着点を始端とする辺が探索対象辺に選択されたならば，終了 */
      fprintf(stdout, "finish\n");
      break;
    }
    popCurrEdgeFrmQueue(&open_lst, curr, &open_lst_sz);
    printPriorityQueue(open_lst.q_head);
    pushEdge2Queue(&closed_lst, curr, &closed_lst_sz);

    fprintf(stderr, "\n\nsah, korekara,tansaku start nanozya!\n");


    while(NULL != vrtx[curr.dst_num].ptr){ /* 隣接辺に対する繰り返し */
      setNeighborVrtxNum(&neighbor, curr.dst_num, vrtx[curr.dst_num].ptr->num);
      neighbor_trv_sec = vrtx[curr.dst_num].ptr->edge_trvt;
      dprintf("neighbor %d -> %d\n", neighbor.orgn_num, neighbor.dst_num);
      enbl_tim_spac_trans = FALSE;

      vrtx[curr.dst_num].ptr->t_ptr = vrtx[curr.dst_num].ptr->t_head;
      while(NULL != vrtx[curr.dst_num].ptr->t_ptr){ /* 隣接辺の時間空間に対する繰り返し */
        setNeighborTimAndCost(&neighbor, vrtx[curr.dst_num].ptr->t_ptr);
        fprintf(stderr, "bgn:%d:%d:%3.1f\tend:%d:%d:%3.1f\t cost:%f\n", neighbor.bgn.hour, neighbor.bgn.min, neighbor.bgn.sec
        , neighbor.end.hour, neighbor.end.min, neighbor.end.sec, neighbor.est_cost);

        if(neighborInLst(closed_lst.q_head, neighbor)){ /* 計算済の辺については，計算しない */
          goto SKIP;
        }
        if(existTimSpac(curr.curr, neighbor.bgn, neighbor.end))
          enbl_tim_spac_trans = TRUE;

        fprintf(stdout, "curr:%2d:%2d:%3.1f\n", curr.curr.hour, curr.curr.min, curr.curr.sec);
        fprintf(stdout, "enbl_tim_spac_trans : %d\n", enbl_tim_spac_trans);
        if(enbl_tim_spac_trans){ /* 時空間を持つ隣接辺を通過する為の待ち時間を求める． */
          wait_sec = getWaitTimSec(neighbor.bgn, curr.curr);
          neighbor_edge_cost = vrtx[curr.dst_num].ptr->t_ptr->edge_cost;
          tmp_rout_cost = curr.rout_cost + wait_sec + neighbor_edge_cost;
          tmp_est_cost = tmp_rout_cost + getDist(vrtx[neighbor.dst_num].pos, vrtx[arrv_num].pos); /* 推定値の計算 */

          if(!neighborInLst(open_lst.q_head, neighbor)){
            neighbor.est_cost = tmp_est_cost;
            neighbor.rout_cost = tmp_rout_cost;
            cpyTim(curr.curr, &neighbor.curr);
            neighbor.curr.sec += (wait_sec + neighbor_trv_sec);
            fixTimFormat(&neighbor.curr);
            pushEdge2Queue(&open_lst, neighbor, &open_lst_sz);
            printPriorityQueue(open_lst.q_head);
            fprintf(stdout, "\n\n");
          }
          else if(neighbor.est_cost > tmp_est_cost)
            vrtx[curr.dst_num].ptr->t_ptr->dij_meta.est_cost = tmp_est_cost;
        }
        SKIP:
        vrtx[curr.dst_num].ptr->t_ptr = vrtx[curr.dst_num].ptr->t_ptr->next;
      }
      vrtx[curr.dst_num].ptr = vrtx[curr.dst_num].ptr->next;
    }
  }
  return;
}
