#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#include <direct.h>
#include "header.h"
#include "define.h"

sun_angle_t getSunAngle(double time);
int fndGlrOccred(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_sz);
double getGlrVal(sun_angle_t sun, xy_coord_t orgn_pos, xy_coord_t dst_pos);
void dtctFileOpn(FILE **fp, char *fname, char *mode);
/* myMath.c */
double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos);


static void printTim(tim_t tim, char *str){
  fprintf(stdout, "%s:%2d:%2d:%3.1f\n", str, tim.hour, tim.min, tim.sec);
}

static void PrintTimExpdEdge(tim_expd_edge_t *trgt){
  fprintf(stdout, "***************************\n");
  fprintf(stdout, "%d->%d\n", trgt->orgn_num, trgt->dst_num);
  printTim(trgt->bgn, "bgn");
  printTim(trgt->end, "end");
  printTim(trgt->passed, "passed");
  fprintf(stdout, "trvt:%f\n", trgt->edge_trvt);
  fprintf(stdout, "est_cost = %f\n", trgt->est_cost);
  fprintf(stdout, "wait_sec = %f\n", trgt->wait_sec);
  fprintf(stdout, "edge_cst = %f\n", trgt->edge_cst);
  fprintf(stdout, "curr adrrs:%p\n", trgt);
  fprintf(stdout, "prev adrrs:%p\n", trgt->prev);
  fprintf(stdout, "***************************\n");

}

static void printPriorityQueue(priority_queue_t *head){
  priority_queue_t *ptr = head;
  fprintf(stdout, "***************************\n");
  fprintf(stdout, "following is priority queue\n");
  while(NULL != ptr){
    if(ptr->edge.orgn_num == 11540 && ptr->edge.dst_num == 11722)
      fprintf(stdout, "%5d->%5d bgn:%2d:%2d:%3.1f end:%2d:%2d:%3.1f\tpassed:%2d:%2d:%3.1f est cost:%f rout cost:%f\n", ptr->edge.orgn_num,
    ptr->edge.dst_num, ptr->edge.bgn.hour, ptr->edge.bgn.min, ptr->edge.bgn.sec,
    ptr->edge.end.hour, ptr->edge.end.min, ptr->edge.end.sec,
    ptr->edge.passed.hour, ptr->edge.passed.min, ptr->edge.passed.sec, ptr->edge.est_cost, ptr->edge.rout_cost);
    ptr = ptr->next;
  }
  fprintf(stdout, "***************************\n");
  return;
}

/**
*時刻を表す構造体の各メンバの中身を空にする
*@param orgn コピー元の時刻
*@param *dst コピー先の時刻
*@return none
*/
void initTim(tim_t *trgt){
	trgt->hour = EMPTY;
	trgt->min = EMPTY;
	trgt->sec = EMPTY;

	return;
}


static double cvthour2Sec(int hour){
	return (double)hour * 60.0f * 60.0f;
}

static double cvtMin2Sec(int min){
	return (double)min * 60.0f;
}

static double getWaitTimSec(tim_t dst, tim_t orgn){
	double wait_sec = 0.0f;
	double dst_sec = cvthour2Sec(dst.hour) + cvtMin2Sec(dst.min) + dst.sec;
	double orgn_sec = cvthour2Sec(orgn.hour) + cvtMin2Sec(orgn.min) + orgn.sec;
	wait_sec = dst_sec - orgn_sec;
	if(wait_sec <= 0.0f)
		wait_sec = 0.0f;

	return wait_sec;
}

/**
*時刻を表す構造体の各メンバをコピーする．
*@param orgn コピー元の時刻
*@param *dst コピー先の時刻
*@return none
*/
void cpyTim(tim_t orgn, tim_t *dst){
  dst->hour = orgn.hour;
  dst->min = orgn.min;
	dst->sec = orgn.sec;
  return;
}


/* 優先度つきキューに頂点を格納する為の記憶領域の取得 */
static struct priority_queue_t *priorityQueueMalloc(void){
  return (priority_queue_t *)malloc(sizeof(priority_queue_t));
}

static void initList(priority_queue_set_t *open_lst, priority_queue_set_t *closed_lst){
  open_lst->old = open_lst->ptr = open_lst->head = NULL;
  closed_lst->old = closed_lst->ptr = closed_lst->head = NULL;

  // open_lst->old = open_lst->ptr = open_lst->head = priorityQueueMalloc();
  // closed_lst->old = closed_lst->ptr = closed_lst->head = priorityQueueMalloc();
  open_lst->sz = closed_lst->sz = 0;

  return;
}

/**
*時刻をそれぞれ60進数に変換する
*@param tim 変換対象時刻
*/
static void fixTimFormat(tim_t *tim){
	if(EMPTY == tim->hour)
		tim->hour = 0;
	if(EMPTY == tim->min)
		tim->min = 0;
	if(EMPTY == tim->sec)
		tim->sec = 0;

	if(60.0f <= tim->sec){
		tim->min += (int)(tim->sec / 60.0f);
		tim->sec = fmod(tim->sec, 60.0f);
		if(60 <= tim->min){
			tim->hour += tim->min / 60;
			tim->min = tim->min % 60;
		}
		if(24 <= tim->hour){
			fprintf(stderr, "over 24 hour\n");
		}
	}
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
  dst_edge->edge_cst = orgn_edge.edge_cst;
  dst_edge->edge_trvt = orgn_edge.edge_trvt;
  dst_edge->wait_sec = orgn_edge.wait_sec;
  dst_edge->prev = orgn_edge.prev;

  return;
}



/**
*オープンリスト内から探索対象(最小の推測値を持つ)辺を抽出する．
*@param lst_head 先頭リスト
*@param curr 探索対象辺
*/
static void findTrgtEdgeFrmOpenLst(priority_queue_t *head, tim_expd_edge_t *curr){
  priority_queue_t *ptr = head;
  double min_est_cost = EMPTY;
  /* オープンリストに格納された全ての辺に対する繰り返しにおいて， */
  /* 最小コストが空，あるいは更に低い推定値を持つ辺が見つかった場合， */
  /* 探索対象辺currと最小コストmin_est_costを更新する */
  while(NULL != ptr){
    if(min_est_cost > ptr->edge.est_cost || EMPTY == min_est_cost){
      cpyTimExpdEdge(ptr->edge, curr);
      min_est_cost = ptr->edge.est_cost;
    }
    ptr = ptr->next;
  }

}


/* 最悪コストで通過した場合における探索状態を生成する． */
static void setWrstSrchStt(tim_expd_edge_t *neighbor, tim_expd_edge_t curr,
vertex_t *vrtx, int arrv_num, double wrst_cst){
  /* 待ち時間の算出(neighborの隣接辺の始端時刻が早い場合は0が出力される) */
  neighbor->wait_sec = getWaitTimSec(neighbor->bgn, curr.passed);

  // if(neighbor->orgn_num == 16952 && neighbor->dst_num == 15314){
  //   fprintf(stdout, "wrst:%f\n", wrst_cst);
  // }

  neighbor->edge_cst = wrst_cst;
  /* 残り推定コストの算出 */
  double odd_cst = getDist(vrtx[neighbor->dst_num].pos, vrtx[arrv_num].pos) / vel;
  /* 到達コストの算出=探索対象辺の到達コスト+辺の最悪コスト+待ち時間 */
  neighbor->rout_cost = curr.rout_cost + neighbor->edge_cst + neighbor->wait_sec;
  /* 推定経路コストの算出 */
  neighbor->est_cost = neighbor->rout_cost + odd_cst;

  /* 隣接辺の到達時刻の算出 */
  /* 探索対象辺の到達時刻+辺の移動時刻+待ち時間 */
  cpyTim(curr.passed, &neighbor->passed);
  neighbor->passed.sec += (neighbor->edge_trvt + neighbor->wait_sec);
  fixTimFormat(&neighbor->passed);

  return;
}


/* 辺の両端を示す頂点番号が同じかを判定する */
static int isEqualBothEndVrtx(tim_expd_edge_t orgn, tim_expd_edge_t dst){
  if(orgn.orgn_num == dst.orgn_num && orgn.dst_num == dst.dst_num)
    return TRUE;
  else
    return FALSE;
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
*優先度つきキューに蓄えられている探索状態を取り出す．
*@param lst_ptr
*@param lst_old
*@param lst_head
*@param trgt_edge
*@param lst_sz
*/
static void deletTrgtQueue(priority_queue_set_t *list, tim_expd_edge_t trgt_edge){

  list->old = list->ptr = list->head;
  /* リストを辿っていく． */
  while(NULL != list->ptr){
    /* 辺の始点および終点が一致するかの判定 */
    if(isEqualBothEndVrtx(list->ptr->edge, trgt_edge)){
      /* 辺の時間領域が一致するかの判定 */
      if(isEqualTim(list->ptr->edge.bgn, trgt_edge.bgn) && isEqualTim(list->ptr->edge.end, trgt_edge.end)){
        /* 辺コストが一致するか(最悪コストもしくは最良コスト)の判定 */
        if(list->ptr->edge.edge_cst == trgt_edge.edge_cst){
          /* 探索状態を取り出す． */
          if(list->ptr == list->head){
            if(1 < list->sz)
              list->head = list->ptr->next;
            else{
              free(list->head);
              list->head = list->ptr = NULL;
            }
          }
          else
            list->old->next = list->ptr->next;
          list->sz -= 1;
          return;
        }
      }
    }
    list->old = list->ptr;
    list->ptr = list->ptr->next;
  }
}


static void deletTrgtQueue2(priority_queue_set_t *list, tim_expd_edge_t trgt_edge){

  list->old = list->ptr = list->head;
  /* リストを辿っていく． */
  while(NULL != list->ptr){
    /* 辺の始点および終点が一致するかの判定 */
    if(isEqualBothEndVrtx(list->ptr->edge, trgt_edge)){
      /* 辺の時間領域が一致するかの判定 */
      if(isEqualTim(list->ptr->edge.bgn, trgt_edge.bgn) && isEqualTim(list->ptr->edge.end, trgt_edge.end)){
        /* 辺コストが一致するか(最悪コストもしくは最良コスト)の判定 */
        if(list->ptr->edge.edge_cst == trgt_edge.edge_cst && isEqualTim(list->ptr->edge.passed, trgt_edge.passed)){
          /* 探索状態を取り出す． */
          if(list->ptr == list->head){
            if(1 < list->sz)
              list->head = list->ptr->next;
            else{
              free(list->head);
              list->head = list->ptr = NULL;
            }
          }
          else
            list->old->next = list->ptr->next;
          list->sz -= 1;
          return;
        }
      }
    }
    list->old = list->ptr;
    list->ptr = list->ptr->next;
  }
}


/* キューの中に探索した辺が存在するかを判定する */
/* 存在しないならばEMPTYを返し，存在するならばその推定経路コストを返す． */
static double isExistInQueue(priority_queue_t *head, tim_expd_edge_t neighbor, tim_t *tim, double *max_est_cost){
  priority_queue_t *ptr = head;
  int min_est_cost = EMPTY;
  while(NULL != ptr){
    /* 辺の始点と終点が一致するかを判定する */
    if(isEqualBothEndVrtx(ptr->edge, neighbor)){
      /* 時間領域が一致するかを判定(多分，どちらか一方の比較でいいかも) */
      if(isEqualTim(ptr->edge.bgn, neighbor.bgn) && isEqualTim(ptr->edge.end, neighbor.end)){
        /* 辺のコスト(最悪コストか最良コストか)が一致するか判定 */
        //if(ptr->edge.edge_cst == neighbor.edge_cst){
          if(EMPTY != min_est_cost && min_est_cost < ptr->edge.est_cost){
            *max_est_cost = ptr->edge.est_cost;
            return min_est_cost;
            //break;
          }
          if(EMPTY == min_est_cost || min_est_cost > ptr->edge.est_cost){
            cpyTim(ptr->edge.passed, tim);
            min_est_cost = ptr->edge.est_cost;
            //return ptr->edge.est_cost;
          }
        //}
      }
    }
    ptr = ptr->next;
  }
  //return EMPTY;
  return min_est_cost;
}

static void insrtQueue(priority_queue_set_t *list, tim_expd_edge_t trgt_edge){

  /* リストの末尾までリストを辿る． */
  if(NULL != list->head){
    list->ptr = list->head;
    while(NULL != list->ptr->next){
      list->ptr = list->ptr->next;
    }
    list->old = list->ptr;
    list->ptr = list->ptr->next; //NULLが挿入される．
    list->ptr = priorityQueueMalloc();
  }
  else if(NULL == list->head){
    list->old = list->ptr = list->head = priorityQueueMalloc();
  }
  cpyTimExpdEdge(trgt_edge, &list->ptr->edge);
  list->old->next = list->ptr;
  list->old = list->ptr;
  list->old->next = NULL;
  list->sz += 1;
  // PrintTimExpdEdge(&list->ptr->edge);
  return;
}

static double getDiffTim(tim_t t1, tim_t t2){
  double t1_sec = t1.hour * 3600 + t1.min * 60 + t1.sec;
  double t2_sec = t2.hour * 3600 + t2.min * 60 + t2.sec;

  return t1_sec - t2_sec;
}

/* 時刻を優先し，最悪コストで通過した場合における隣接辺情報の更新 */
static int updtDmntQueue(priority_queue_set_t *open_lst, priority_queue_set_t *closed_lst,
tim_expd_edge_t *neighbor){
  int exec_flag = FALSE;
  int is_exist = 0;
  tim_t tim;
  double max = EMPTY;
  /* キューの中に探索した辺が存在するかを判定する */
  /* 存在しないならばEMPTYを返し，存在するならばその推定経路コストを返す． */
  double curr_est_cst = isExistInQueue(closed_lst->head, *neighbor, &tim, &max);
  if(EMPTY != curr_est_cst)
    is_exist = 1;
  else{
    curr_est_cst = isExistInQueue(open_lst->head, *neighbor, &tim, &max);
    if(EMPTY != curr_est_cst)
      is_exist = 2;
  }
  double diff_sec = getDiffTim(neighbor->passed, tim);

  /* 隣接辺が未探索状態であった場合 */
  if(0 == is_exist){
    /* キューへ探索状態を追加する． */
    insrtQueue(open_lst, *neighbor);
    exec_flag = TRUE;
  }
  else if (diff_sec < 0.0f && (curr_est_cst + diff_sec) > neighbor->est_cost){
    /* neighborと一致する辺を持つ探索状態を削除する． */
    if(2 == is_exist){
      deletTrgtQueue(open_lst, *neighbor);
      insrtQueue(open_lst, *neighbor);
    }
    exec_flag = TRUE;
  }
  else if(curr_est_cst + diff_sec > neighbor->est_cost){
    /* 類似したコストは省くために，(fabs(neighbor->est_cost - curr_est_cst) > 100)してる． */
    /* 凝れしないと，計算に一日以上要する．最適解は保証されないが，一周回って広島市内だと上手くいく．．． */
    /* クローズリストを計算済と取り扱うと，後から逆転する辺を探索できない． */
    if((max == EMPTY || max > neighbor->est_cost) && (fabs(neighbor->est_cost - curr_est_cst) > 100)){
    /* 一番それらしい．．． */
    //if((max == EMPTY || max > neighbor->est_cost)){
    /* 厳密にやるとき．（時間は一日以上かかるかも．．．） */
    //if(max != neighbor->est_cost && curr_est_cst != neighbor->est_cost){
      insrtQueue(open_lst, *neighbor);
      exec_flag = TRUE;
    }
  }
  return exec_flag;
}

/* 時刻を優先し，最悪コストで通過した場合における隣接辺情報の更新 */
static int updtQueue(priority_queue_set_t *open_lst, priority_queue_set_t *closed_lst,
tim_expd_edge_t *neighbor){
  int exec_flag = FALSE;
  int is_exist = 0;
  double max = EMPTY;
  tim_t tim;
  /* キューの中に探索した辺が存在するかを判定する */
  /* 存在しないならばEMPTYを返し，存在するならばその推定経路コストを返す． */
  double curr_est_cst = isExistInQueue(closed_lst->head, *neighbor, &tim, &max);
  if(EMPTY != curr_est_cst)
    is_exist = 1;
  else{
    curr_est_cst = isExistInQueue(open_lst->head, *neighbor, &tim, &max);
    if(EMPTY != curr_est_cst)
      is_exist = 2;
  }

  /* 隣接辺が未探索状態であった場合 */
  if(0 == is_exist){
    /* キューへ探索状態を追加する． */
    insrtQueue(open_lst, *neighbor);
    exec_flag = TRUE;
  }
  /* 隣接辺が探索されているが， 新たにコストの少ない探索状態が見つかった場合*/
  else if(curr_est_cst > neighbor->est_cost){
    if(1 == is_exist){
      deletTrgtQueue(closed_lst, *neighbor);
    }
    else if(2 == is_exist){
      deletTrgtQueue(open_lst, *neighbor);
    }
    insrtQueue(open_lst, *neighbor);
    exec_flag = TRUE;
  }
  return exec_flag;
}

/**
*最良コストで移動した場合における探索状態を生成する．
*/
static void setBestSrchStt(tim_expd_edge_t *neighbor, tim_expd_edge_t curr, int arrv_num,
time_space_list_t *ptr, vertex_t *vrtx){
  /* 待ち時間の算出 */
  neighbor->wait_sec = getWaitTimSec(ptr->arch_low_cost_tim, curr.passed);

  /* 辺コストの格納 */
  neighbor->edge_cst = ptr->low_cost;

  /* 残り推定コストの算出 */
  double odd_cst = getDist(vrtx[neighbor->dst_num].pos, vrtx[arrv_num].pos) / vel;
  /* 到達コストの算出 */
  neighbor->rout_cost = curr.rout_cost + ptr->low_cost + neighbor->wait_sec;
  /* 推定経路コストの算出 */
  neighbor->est_cost = neighbor->rout_cost + odd_cst;


  cpyTim(curr.passed, &neighbor->passed);
  neighbor->passed.sec += (neighbor->edge_trvt + neighbor->wait_sec);
  fixTimFormat(&neighbor->passed);

  return;
}

/**
*最良コストで移動した場合における推定経路コストが，
*/
static int isNonDominated(tim_expd_edge_t wrst, tim_expd_edge_t best){
  if(best.wait_sec < wrst.wait_sec)
    fprintf(stderr, "okass %f < %f\n", best.wait_sec, wrst.wait_sec);
  if(best.est_cost < wrst.est_cost - 1.0f)
    return TRUE;
  else
    return FALSE;
}

static void initSrchState(tim_expd_edge_t *curr, tim_t dptr_tim, int dptr_num, int arrv_num,
vertex_t *vrtx){
  curr->orgn_num = EMPTY;
  curr->dst_num = dptr_num;
  curr->rout_cost = 0.0f;
  curr->est_cost = getDist(vrtx[dptr_num].pos, vrtx[arrv_num].pos) / vel;
  cpyTim(dptr_tim, &curr->bgn);
  cpyTim(dptr_tim, &curr->end);
  cpyTim(dptr_tim, &curr->passed);
  curr->prev = NULL;

  return;
}


static void insrtClosedQueue(priority_queue_set_t *list, tim_expd_edge_t **curr,
tim_expd_edge_t tmp){
  /* リストの末尾までリストを辿る． */
  if(NULL != list->head){
    list->ptr = list->head;
    while(NULL != list->ptr->next){
      list->ptr = list->ptr->next;
    }
    list->old = list->ptr;
    list->ptr = list->ptr->next; //NULLが挿入される．
    list->ptr = priorityQueueMalloc();
  }
  else if(NULL == list->head){
    list->old = list->ptr = list->head = priorityQueueMalloc();
  }
  /* リストの末尾に，オープンリストで推定経路コストが最少の探索状態をコピーする． */
  cpyTimExpdEdge(tmp, &list->ptr->edge);
  list->old->next = list->ptr;
  list->old = list->ptr;
  list->old->next = NULL;
  list->sz += 1;

  *curr = &list->ptr->edge;
  return;
}


static void deletMinQueue(priority_queue_set_t *list, tim_expd_edge_t *tmp){
  /* オープンリストの中から，推定経路コストが最少の探索状態をtmpにコピーする． */
  findTrgtEdgeFrmOpenLst(list->head, tmp);
  /* オープンリストの中から取り出したものと同じ辺(到達時刻および両端の頂点番号で判定)を取り出す */
  deletTrgtQueue2(list, *tmp);

  return;
}

static int isTraversable(tim_t curr_tim, tim_t end_tim){
  double curr_sec = curr_tim.hour * 3600 + curr_tim.min * 60 + curr_tim.sec;
  double end_sec = end_tim.hour * 3600 + end_tim.min * 60 + end_tim.sec;

  if(end_sec >= curr_sec)
    return TRUE;
  else
    return FALSE;
}

static void setNeighbor(tim_expd_edge_t *neighbor, tim_expd_edge_t *curr, adj_list_t *ptr){
  //辺の始点および終点番号の格納
  neighbor->orgn_num = curr->dst_num;
  neighbor->dst_num = ptr->num;
  /* 直前に通過した辺のアドレスを格納 */
  neighbor->prev = curr;
  /* 辺の時間領域の格納 */
  cpyTim(ptr->t_ptr->bgn_tim, &neighbor->bgn);
  cpyTim(ptr->t_ptr->end_tim, &neighbor->end);
  /* 辺の移動に対する所要時間の格納 */
  neighbor->edge_trvt = ptr->edge_trvt;

  return;
}

static int occredGlare(tim_expd_edge_t edge){
  if(edge.edge_trvt == edge.edge_cst)
    return FALSE;
  else if(edge.edge_trvt < edge.edge_cst){
    return TRUE;
  }
  else{
    fprintf(stderr, "okassiii\n");
    exit(1);
  }
}

/**
*新規探索状態を生成し，探索中キューへ追加する．
*/
static void updatePriorityQueue(priority_queue_set_t *open_lst, priority_queue_set_t *closed_lst,
tim_expd_edge_t *neighbor, tim_expd_edge_t curr, vertex_t *vrtx, int arrv_num, time_space_list_t *ptr){
  tim_expd_edge_t best_neighbor;

  cpyTimExpdEdge(*neighbor, &best_neighbor);
/* 最悪コストにおける探索状態の生成 */
  setWrstSrchStt(neighbor, curr, vrtx, arrv_num, ptr->high_cost);

  /* Worst-A*による経路探索を行う場合 */
  if(WRSTA_STAR == find_path_algrthm){
    setBestSrchStt(&best_neighbor, curr, arrv_num, ptr, vrtx);
    updtQueue(open_lst, closed_lst, neighbor);
    // updtQueue(open_lst, closed_lst, neighbor);
  }
  /* Normal-A*による経路探索を行う場合 */
  else if(NA_STAR == find_path_algrthm){
    setBestSrchStt(&best_neighbor, curr, arrv_num, ptr, vrtx);
    updtQueue(open_lst, closed_lst, &best_neighbor);
    // /* 時刻優先および不快度優先のどちらかの探索状態を探索中キューへ追加する． */
    // if(occredGlare(*neighbor) && neighbor->est_cost > best_neighbor.est_cost){
    //   updtQueue(open_lst, closed_lst, &best_neighbor);
    // }
    // else{
    //   updtQueue(open_lst, closed_lst, neighbor);
    // }
  }
  /* Multi-Objective-A*による経路探索を行う場合 */
  else if(MOA_STAR == find_path_algrthm){
    /* 最悪コストにおける探索状態を探索中キューへ追加する． */
    // setBestSrchStt(&best_neighbor, curr, arrv_num, ptr, vrtx);
    updtDmntQueue(open_lst, closed_lst, neighbor);
      /* 最良コストにおける探索状態を生成する． */
    if(neighbor->edge_cst != neighbor->edge_trvt){
      setBestSrchStt(&best_neighbor, curr, arrv_num, ptr, vrtx);
      /* 相互支配判定 */
      if(isNonDominated(*neighbor, best_neighbor)){
        // insrtQueue(open_lst, best_neighbor);
        updtDmntQueue(open_lst, closed_lst, &best_neighbor);
      }
    }
  }
  /* アルゴリズム設定パラメータにおける例外処理 */
  else{
    fprintf(stderr, "Usage:find_path_algrthm -> 3 find path by Worst-Astar\n");
    fprintf(stderr, "Usage:find_path_algrthm -> 2 find path by Normal-Astar\n");
    fprintf(stderr, "Usage:find_path_algrthm -> 1 find path by Multi-Objective-Astar\n");
    exit(1);
  }
  return;
}

/**
*辺を通過し終わった時刻と辺の移動に対する所要時間に基づき，
*(待ち時間を加味している)辺の出発時刻を算出する．
*/
static void calcDptrTim(tim_expd_edge_t *curr, tim_t *tim){
  double curr_sec = curr->passed.hour * 3600 + curr->passed.min * 60 + curr->passed.sec;
  curr_sec = curr_sec - curr->edge_trvt;
  tim->hour = (int)curr_sec / 3600;
  double tmp = fmod(curr_sec, 3600.0f);
  tim->min = (int)tmp / 60;
  tim->sec = fmod(tmp, 60.0f);

  if(tim->sec > 59.0f && curr->wait_sec > 0.0f){
    tim->sec = 0.0f;
    tim->min += 1;
    fixTimFormat(tim);
  }

  return;
}

/**
*算出した経路を一分単位時間拡大ネットワーク上において，評価する．
*/
static double evlatRealCost(tim_expd_edge_t *curr, vertex_t *vrtx, build_grid_t **bld_grd,
xy_coord_t grd_len, grid_size_t grd_cell_sz){
  double edge_cst, tim_h, sum_trvt, sum_cost, sum_glr_sec, sum_wait;
  tim_t tim;
  sun_angle_t sun;
  sum_trvt = sum_cost = sum_glr_sec = sum_wait = 0.0f;

  while(NULL != curr->prev){
    /* 各辺の(待ち時間を加味した)出発時刻の算出 */
    calcDptrTim(curr, &tim);
    /* 太陽の角度を算出する */
    tim_h = (double)tim.hour + (double)tim.min / 60.0f;
    sun = getSunAngle(tim_h);
    /* 各辺のコストを算出する． */
    if(fndGlrOccred(sun, vrtx[curr->orgn_num].pos, vrtx[curr->dst_num].pos, bld_grd,
    grd_len, grd_cell_sz)){
      edge_cst = getGlrVal(sun, vrtx[curr->orgn_num].pos, vrtx[curr->dst_num].pos)
       * curr->edge_trvt + curr->wait_sec;
       sum_glr_sec += curr->edge_trvt;
    }
    else{
      edge_cst = curr->edge_trvt + curr->wait_sec;
    }
    if(curr->wait_sec > 0.0f)
      sum_wait = curr->wait_sec;

    sum_trvt += (curr->edge_trvt + curr->wait_sec);
    sum_cost += edge_cst;

    curr = curr->prev;
  }
  fprintf(stdout, "cost:%f, trvsec:%f, glaresec:%f, wait:%f\n", sum_cost, sum_trvt, sum_glr_sec, sum_wait);
  return sum_cost;
}

/**
*出発地，目的地，時刻，各アルゴリズムに対するディレクトリを作成する．
*/
static void creatDirect(char dir_url[MAX_LEN], int dptr_num, int arrv_num, tim_t dptr){

  sprintf(dir_url, "..\\outputfile\\optimumPath\\dptr%d_arrv%d", dptr_num, arrv_num);
  _mkdir(dir_url);
  sprintf(dir_url, "..\\outputfile\\optimumPath\\dptr%d_arrv%d\\dptr_tim%d_%d", dptr_num, arrv_num, dptr.hour, dptr.min);
  _mkdir(dir_url);
  sprintf(dir_url, "..\\outputfile\\optimumPath\\dptr%d_arrv%d\\dptr_tim%d_%d\\srchAlg%d", dptr_num, arrv_num, dptr.hour, dptr.min, find_path_algrthm);
  _mkdir(dir_url);

  return;
}

/**
*算出した経路を構成する頂点の座標およびそれに付随する情報をファイルに書き込む．
*/

static void filePrintPath(tim_expd_edge_t *curr, int dptr_num, int arrv_num, tim_t dptr,
vertex_t *vrtx){
  FILE *path_vrtx_num_fp, *path_vrtx_pos_fp, *wait_pos_fp;
  char path_vrtx_num_fname[MAX_LEN], path_vrtx_pos_fname[MAX_LEN], wait_pos_fname[MAX_LEN],
  dir_url[MAX_LEN];
  /* ディレクトリの作成 */
  creatDirect(dir_url, dptr_num, arrv_num, dptr);
  /* ファイル名の生成 */
  strcpy(path_vrtx_pos_fname, dir_url);
  strcat(path_vrtx_pos_fname, "\\path_pos.dat");
  strcpy(path_vrtx_num_fname, dir_url);
  strcat(path_vrtx_num_fname, "\\path_info.txt");
  strcpy(wait_pos_fname, dir_url);
  strcat(wait_pos_fname, "\\wait_pos.dat");

  /* ファイルを開く */
  dtctFileOpn(&path_vrtx_num_fp, path_vrtx_num_fname, "wt");
  dtctFileOpn(&path_vrtx_pos_fp, path_vrtx_pos_fname, "w");
  dtctFileOpn(&wait_pos_fp, wait_pos_fname, "w");

  while(NULL != curr->prev){
    fprintf(path_vrtx_pos_fp, "%f %f\n", vrtx[curr->dst_num].pos.x, vrtx[curr->dst_num].pos.y);

    if(curr->wait_sec > 0.0f)
      fprintf(wait_pos_fp, "%f %f %f\n", vrtx[curr->dst_num].pos.x, vrtx[curr->dst_num].pos.y, curr->wait_sec);

    fprintf(path_vrtx_num_fp, "|%5d| -> |%5d| time %2d:%2d:%2.0f<--%2d:%2d:%2.0f  arrv %2d:%2d:%2.0f\t",curr->orgn_num,
    curr->dst_num, curr->end.hour, curr->end.min, curr->end.sec,
    curr->bgn.hour, curr->bgn.min, curr->bgn.sec, curr->passed.hour, curr->passed.min, curr->passed.sec);
    fprintf(path_vrtx_num_fp, "est cost %f passed cost %f\n", curr->est_cost, curr->rout_cost);

    curr = curr->prev;
  }
  fclose(path_vrtx_num_fp);
  fclose(path_vrtx_pos_fp);
  fclose(wait_pos_fp);

  return;
}

/**
*経路の不快度をファイルに書き込む
  */
static void filePrintCost(double real_cost, int dptr_num, int arrv_num){
  FILE *fp;
  dtctFileOpn(&fp, cost_output_file, "a");
  if(WRSTA_STAR == find_path_algrthm)
    fprintf(fp, "%d,%d,", dptr_num, arrv_num);

  if(MOA_STAR == find_path_algrthm)
    fprintf(fp, "%f\n", real_cost);
  else
    fprintf(fp, "%f,", real_cost);

  fclose(fp);
}

/* 優先度つきキューに頂点を格納する為の記憶領域の取得 */
struct tim_expd_edge_t *timExpdEdgeMalloc(void){
  return (tim_expd_edge_t *)malloc(sizeof(tim_expd_edge_t));
}

void MultiObjectiveAstar(vertex_t *vrtx, int dptr_num, int arrv_num, tim_t dptr,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
  priority_queue_set_t open_lst, closed_lst;
  tim_expd_edge_t *curr, neighbor, tmp;
  int depth = 0;
  int counter = 0;

  clock_t start = clock();

  curr = timExpdEdgeMalloc();
  /* 出発地を終点とする辺を生成し，オープンリストへ格納 */
  initSrchState(curr, dptr, dptr_num, arrv_num, vrtx);
  initList(&open_lst, &closed_lst);
  insrtQueue(&open_lst, *curr);

  while(0 < open_lst.sz){
    deletMinQueue(&open_lst, &tmp);
    insrtClosedQueue(&closed_lst, &curr, tmp);/* currにはキューのアドレスがそのまま入っている */
    counter++;
    if(arrv_num == curr->dst_num){
      break;
    }
    /* 隣接辺に対する繰り返し */
    vrtx[curr->dst_num].ptr = vrtx[curr->dst_num].head;
    while(NULL != vrtx[curr->dst_num].ptr){
      /* 隣接辺の時間拡大された辺に対する繰り返し */
      vrtx[curr->dst_num].ptr->t_ptr = vrtx[curr->dst_num].ptr->t_head;
      depth = 0;
      while(NULL != vrtx[curr->dst_num].ptr->t_ptr){
        /* 隣接辺が通過可能かを判定し，通行可能ならば，辺情報をneighborにコピーする． */
        if(isTraversable(curr->passed, vrtx[curr->dst_num].ptr->t_ptr->end_tim)){
          depth++;
          if(depth == 3)
            break;
          setNeighbor(&neighbor, curr, vrtx[curr->dst_num].ptr);
          /* 探索状態の生成およびオープンリストの更新(アルゴリズムに応じて変わる部分) */
          updatePriorityQueue(&open_lst, &closed_lst, &neighbor, *curr, vrtx, arrv_num,
          vrtx[curr->dst_num].ptr->t_ptr);
        }
        vrtx[curr->dst_num].ptr->t_ptr = vrtx[curr->dst_num].ptr->t_ptr->next;
      }
      vrtx[curr->dst_num].ptr = vrtx[curr->dst_num].ptr->next;
    }
  }
  if(0 == open_lst.sz){
    fprintf(stderr, "INF\n");
    return;
  }
  fprintf(stdout, "est:%f\t", curr->est_cost);
  double real_cost = evlatRealCost(curr, vrtx, bld_grd, grd_len, grd_cell_sz);
  filePrintPath(curr, dptr_num, arrv_num, dptr, vrtx);
  filePrintCost(real_cost, dptr_num, arrv_num);

  clock_t end = clock();
  fprintf(stdout, "process time %f[sec]\n\n", (double)(end - start)/1000.0f);

  //printPriorityQueue(open_lst.head);

  return;
}

void srchRoute(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz){
  if(ALLOC_BOTH == allct_cost_type){
    MultiObjectiveAstar(vrtx_st.indx, dptr_num, arrv_num, dptr, bld_grd, grd_len, grd_cell_sz);
  }
	else{
		fprintf(stderr, "Usage: find_path_algrthm -> -1 find path by dijkstra\n");
		fprintf(stderr, "     : find_path_algrthm -> 1 find path by a_star\n");
    fprintf(stderr, "     : find_path_algrthm -> 2 find path by mo-a_star\n");
	}
}
