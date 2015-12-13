#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"
#include "define.h"

/* 経路探索において，直前に通過した頂点を表す構造体 */
typedef struct tim_expd_edge_t{
	int orgn_num;
	int dst_num;
	tim_t bgn;
	tim_t end;
	tim_t curr; //頂点への到達時刻
	double rout_cost; //頂点への到達コスト
}tim_expd_edge_t;

/* 時間空間を表す構造体(ダイクストラ法による経路探索用) */
typedef struct dij_time_space_list_t{
	tim_t bgn_tim; //時間空間Sへ遷移出来る最も早い時刻
	tim_t end_tim; //時間空間Sへ遷移出来る最も遅い時刻
	double edge_cost; //辺のコスト
  double rout_cost; //経路において，その辺を通過し終わった時のコスト
	int srch_stat; //探索済みかどうかの状態
	tim_expd_edge_t prev;
	tim_t arrv_tim; //辺を通過し終わる時刻
	struct dij_time_space_list_t *next;
}dij_time_space_list_t;

/* 隣接リストを表す構造体(ダイクストラ法による経路探索用) */
typedef struct dij_adj_list_t{
	int num;
	double edge_trvt;
	dij_time_space_list_t *t_head; //時間依存西日グレア状態
	dij_time_space_list_t *t_ptr;
	dij_time_space_list_t *t_old;
	struct dij_adj_list_t *next;
}dij_adj_list_t;

/* 頂点リストを表現する構造体(ダイクストラ法による経路探索用) */
typedef struct dij_vertex_t{
	int num; //インデックス番号
	xy_coord_t pos; //2次元ユークリッド空間における位置座標
	dij_adj_list_t *ptr;
	dij_adj_list_t *head;
	dij_adj_list_t *old;
}dij_vertex_t;


static void cpyVrtxIndxAndPos(vertex_t *orgn, dij_vertex_t *dst, int vrtx_sz){
  int i;
  for(i=1;i<vrtx_sz;i++){
    dst[i].num = orgn[i].num;
    dst[i].pos.x = orgn[i].pos.x;
    dst[i].pos.y = orgn[i].pos.y;
  }
/*
  for(i=1;i<vrtx_sz;i++)
    fprintf(stdout, "%d -> (%f, %f)\n", dst[i].num, dst[i].pos.x, dst[i].pos.y);
*/
  return;
}

/* 隣接リスト(ダイクストラ法による経路探索用)の記憶領域の取得 */
dij_adj_list_t *dijAdjListMalloc(void){
	return (dij_adj_list_t *)malloc(sizeof(dij_adj_list_t));
}

/*
static void printDijVertex(dij_vertex_t* vrtx, int vrtx_sz){
	int i;
	for(i=1;i<vrtx_sz;i++){
		//printf("%5d %lf %lf |", vrtx[i].node.num, vrtx[i].node.pos.x, vrtx[i].node.pos.y);
		fprintf(stdout,"|%5d| -> ", vrtx[i].num);
		vrtx[i].ptr = vrtx[i].head;
		while(vrtx[i].ptr != NULL){
			fprintf(stdout,"|%d|", vrtx[i].ptr->num);
			vrtx[i].ptr = vrtx[i].ptr->next;
		}
		fprintf(stdout,"\n");
	}
	return;
}
*/

static void cpyVrtxAdjList(vertex_t *orgn, dij_vertex_t *dst, int vrtx_sz){
  int indx = 1;
  while(indx < vrtx_sz){
    dst[indx].head = dijAdjListMalloc();
    dst[indx].old = dst[indx].ptr = dst[indx].head;

    orgn[indx].ptr = orgn[indx].head;
    indx++;
  }

  indx = 1;
  while(indx < vrtx_sz){
      while(NULL != orgn[indx].ptr){
        dst[indx].ptr->num = orgn[indx].ptr->num;
				dst[indx].ptr->edge_trvt = orgn[indx].ptr->edge_trvt;
        orgn[indx].ptr = orgn[indx].ptr->next;
        dst[indx].old->next = dst[indx].ptr;
        dst[indx].old = dst[indx].ptr;
        dst[indx].ptr = dijAdjListMalloc();
      }
      free(dst[indx].ptr);
      dst[indx].old->next = NULL;
      indx++;
  }
  //printDijVertex(dst, vrtx_sz);

  return;
}

/**
*時間空間リストの記憶領域を取得する．
@return 時間空間リストサイズ(time_space_list_t)分の記憶領域
*/
static dij_time_space_list_t *dijTimSpcLstMalloc(void){
  return (dij_time_space_list_t *)malloc(sizeof(dij_time_space_list_t));
}

static void cpyTim(tim_t orgn, tim_t *dst){
  dst->hour = orgn.hour;
  dst->min = orgn.min;
	dst->sec = orgn.sec;
  return;
}

static void printDijVertexTimSpce(dij_vertex_t* vrtx, int vrtx_sz){
	int i;
	dij_vertex_t tmp_vrtx;
	for(i=1;i<vrtx_sz;i++){
		tmp_vrtx.ptr = vrtx[i].head;
		tmp_vrtx.num = vrtx[i].num;
		while(tmp_vrtx.ptr != NULL){
			fprintf(stdout,"|%5d| -> ", vrtx[i].num);
			fprintf(stdout,"|%5d|\n", tmp_vrtx.ptr->num);
			tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
			while(NULL != tmp_vrtx.ptr->t_ptr){
				fprintf(stdout, "bgn:%2d:%2d\t", tmp_vrtx.ptr->t_ptr->bgn_tim.hour, tmp_vrtx.ptr->t_ptr->bgn_tim.min);
				fprintf(stdout, "end:%2d:%2d\t", tmp_vrtx.ptr->t_ptr->end_tim.hour, tmp_vrtx.ptr->t_ptr->end_tim.min);
				// fprintf(stdout, "cost = %f\n", tmp_vrtx.ptr->t_ptr->edge_cost);
				fprintf(stdout, "rout_cost = %f\t", tmp_vrtx.ptr->t_ptr->rout_cost);
				fprintf(stdout, "arrv_tim:%d:%d:%f\t", tmp_vrtx.ptr->t_ptr->arrv_tim.hour, tmp_vrtx.ptr->t_ptr->arrv_tim.min, tmp_vrtx.ptr->t_ptr->arrv_tim.sec);
				if(TRUE == tmp_vrtx.ptr->t_ptr->srch_stat)
					fprintf(stdout, "srch state:TRUE\n");
				else
					fprintf(stdout, "srch state:FALSE\n");


				tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_ptr->next;
			}
			fprintf(stdout,"\n");
			tmp_vrtx.ptr = tmp_vrtx.ptr->next;
		}
	}
	return;
}
/**
*TODO:コード量を少なくし，可読性を高める事．
*特に，ポインタの初期化処理辺りが冗長
*/
static void cpyVrtxTimSpceList(vertex_t *orgn, dij_vertex_t *dst, int vrtx_sz){
  int indx = 1;
  while(indx < vrtx_sz){
    orgn[indx].ptr = orgn[indx].head;
    dst[indx].ptr = dst[indx].head;
		//fprintf(stdout, "start\n");
    while(NULL != orgn[indx].ptr){
      dst[indx].ptr->t_head = dijTimSpcLstMalloc();
      dst[indx].ptr->t_old = dst[indx].ptr->t_ptr = dst[indx].ptr->t_head;
      //orgn[indx].ptr->t_ptr = orgn[indx].ptr->t_head;
			orgn[indx].ptr = orgn[indx].ptr->next;
			dst[indx].ptr = dst[indx].ptr->next;
    }
		orgn[indx].ptr = orgn[indx].head;
		//fprintf(stdout, "finish\n");
    indx++;
  }
	indx = 1;
	while(indx < vrtx_sz){
		while(NULL != orgn[indx].ptr){
			orgn[indx].ptr->t_ptr = orgn[indx].ptr->t_head;
			orgn[indx].ptr = orgn[indx].ptr->next;
		}
		orgn[indx].ptr = orgn[indx].head;
		indx++;
	}
	indx = 1;
	while(indx < vrtx_sz){
		// while(NULL != dst[indx].ptr){
		// 	dst[indx].ptr->t_ptr = dst[indx].ptr->t_head;
		// 	dst[indx].ptr = dst[indx].ptr->next;
		// }
		dst[indx].ptr = dst[indx].head;
		indx++;
	}



  indx = 1;
  while(indx < vrtx_sz){
      while(NULL != dst[indx].ptr){
        while(NULL != orgn[indx].ptr->t_ptr){
          cpyTim(orgn[indx].ptr->t_ptr->bgn_tim, &dst[indx].ptr->t_ptr->bgn_tim);
          cpyTim(orgn[indx].ptr->t_ptr->end_tim, &dst[indx].ptr->t_ptr->end_tim);
          dst[indx].ptr->t_ptr->edge_cost = orgn[indx].ptr->t_ptr->cost;

					orgn[indx].ptr->t_ptr = orgn[indx].ptr->t_ptr->next;

					dst[indx].ptr->t_old->next = dst[indx].ptr->t_ptr;
					dst[indx].ptr->t_old = dst[indx].ptr->t_ptr;
					dst[indx].ptr->t_ptr = dijTimSpcLstMalloc();
        }
				free(dst[indx].ptr->t_ptr);
				dst[indx].ptr->t_old->next = NULL;

        orgn[indx].ptr = orgn[indx].ptr->next;
				dst[indx].ptr = dst[indx].ptr->next;
      }
			indx++;
  }

	//printDijVertexTimSpce(dst, vrtx_sz);
  return;

}

static dij_vertex_t *cpyVrtx(vertex_set_t orgn_vrtx_st, dij_vertex_t *trgt_vrtx, int *trgt_vrtx_sz){
  *trgt_vrtx_sz = orgn_vrtx_st.sz;
  trgt_vrtx = (dij_vertex_t *)malloc(sizeof(dij_vertex_t) * *trgt_vrtx_sz);
  if(NULL == trgt_vrtx){
    fprintf(stderr, "cannnot malloc dij_vertex_t memory\n");
    exit(1);
  }

  cpyVrtxIndxAndPos(orgn_vrtx_st.indx, trgt_vrtx, *trgt_vrtx_sz);
  cpyVrtxAdjList(orgn_vrtx_st.indx, trgt_vrtx, *trgt_vrtx_sz);
  cpyVrtxTimSpceList(orgn_vrtx_st.indx, trgt_vrtx, *trgt_vrtx_sz);

	return trgt_vrtx;
}

/**
 各頂点の初期化
 経路におけるコスト，探索済フラグ，直前に通過したノード番号と時間空間，
 そして，到着時刻を初期化する．
 */
void initDijVrtx(dij_vertex_t *vrtx, int vrtx_sz){
	int i;
	for(i=1;i<vrtx_sz;i++){
		vrtx[i].ptr = vrtx[i].head;
		while(NULL != vrtx[i].ptr){
			vrtx[i].ptr->t_ptr = vrtx[i].ptr->t_head;
			while(NULL != vrtx[i].ptr->t_ptr){
				/* ここに初期化処理を書く */
				vrtx[i].ptr->t_ptr->rout_cost = EMPTY;
				vrtx[i].ptr->t_ptr->srch_stat = FALSE;
				vrtx[i].ptr->t_ptr->prev.orgn_num = EMPTY;
				vrtx[i].ptr->t_ptr->prev.bgn.hour = EMPTY;
				vrtx[i].ptr->t_ptr->prev.bgn.min = EMPTY;
				vrtx[i].ptr->t_ptr->prev.end.hour = EMPTY;
				vrtx[i].ptr->t_ptr->prev.end.min = EMPTY;
				vrtx[i].ptr->t_ptr->arrv_tim.hour = EMPTY;
				vrtx[i].ptr->t_ptr->arrv_tim.min = EMPTY;

				vrtx[i].ptr->t_ptr = vrtx[i].ptr->t_ptr->next;
			}
			vrtx[i].ptr->t_ptr = vrtx[i].ptr->t_head;
			vrtx[i].ptr = vrtx[i].ptr->next;
		}
		vrtx[i].ptr = vrtx[i].head;
	}
	return;
}

/**
*時刻trgtが時刻bgnから時刻endの間であるかを判定する．
*/
static int existTimSpac(tim_t trgt, tim_t bgn, tim_t end){
	int trgt_min = trgt.hour * 60 + trgt.min;
	int bgn_min = bgn.hour * 60 + bgn.min;
	int end_min = end.hour * 60 + end.min;

	if(bgn_min <= trgt_min && trgt_min < end_min){
		return TRUE;
	}else{
		return FALSE;
	}
}

/**
*時刻を単位[]
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

/**
*出発点に対する初期化
*到達するまでのコストに0を格納し，
*到達時刻に出発時刻を格納する．
*/
static void initDptrVrtx(dij_adj_list_t *edge, tim_t dptr){
	while(NULL != edge->t_ptr){
		if(TRUE == existTimSpac(dptr, edge->t_ptr->bgn_tim, edge->t_ptr->end_tim)){
			edge->t_ptr->rout_cost = edge->t_ptr->edge_cost;
			edge->t_ptr->arrv_tim.hour = dptr.hour;
			edge->t_ptr->arrv_tim.min = dptr.min;
			edge->t_ptr->arrv_tim.sec += edge->edge_trvt;
			fixTimFormat(&edge->t_ptr->arrv_tim);
			break;
		}
		edge->t_ptr = edge->t_ptr->next;
	}
	return;
}

/**
*探索済みの辺の終端を示す頂点の初期化
@param *trgt 探索済みの辺の終端を示す頂点のポインタ
@return none
*/
static void initTrgtVrtx(tim_expd_edge_t *trgt){
	trgt->orgn_num = EMPTY;
	trgt->dst_num = EMPTY;
	trgt->bgn.hour = trgt->bgn.min = trgt->bgn.sec = EMPTY;
	trgt->end.hour = trgt->end.min = trgt->end.sec = EMPTY;
	trgt->curr.hour = trgt->curr.min = trgt->curr.sec = EMPTY;
	trgt->rout_cost =EMPTY;

	return;
}

/**
*未探索状態から探索済状態へ遷移する．
*しかし，最初の繰り返し時は，どの辺も探索済とならない．
*/
static void swtchSrchStatFrmFalse2True(tim_expd_edge_t trgt, dij_adj_list_t *edge_head){
	dij_adj_list_t *edge = edge_head;
	if(EMPTY != trgt.orgn_num){
		while(NULL != edge){
			/*もし，2次元座標における辺が一致したならば
			時間空間において一致する辺を探す */
			if(trgt.dst_num == edge->num){
				edge->t_ptr = edge->t_head;
				while(NULL != edge->t_ptr){
					if(TRUE == existTimSpac(trgt.bgn, edge->t_ptr->bgn_tim, edge->t_ptr->end_tim)){
						edge->t_ptr->srch_stat = TRUE;
					}
					edge->t_ptr = edge->t_ptr->next;
				}
			}
			edge = edge->next;
		}
	}
	return;
}

/* 出発地から最もコストの少ない頂点を探索し， */
/* その頂点を探索対象頂点とする． */
static void findTrgtVrtx(tim_expd_edge_t *trgt, dij_vertex_t *vrtx, int vrtx_sz){
	//dij_adj_list_t edge;
	dij_vertex_t tmp_vrtx;
	int i;
	double min_cost = EMPTY;
	static int counter = 1;
	/* 各頂点に対する繰り返し */
	for(i=1;i<vrtx_sz;i++){
		/* 各頂点iに(2次元の概念において)隣接する頂点を走査していく． */
		tmp_vrtx.ptr = vrtx[i].head;
		while(NULL != tmp_vrtx.ptr){
			/* 頂点iと頂点tmp_vrtx.ptr->numを結ぶ辺の時間空間を走査していく */
			tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
			while(NULL != tmp_vrtx.ptr->t_ptr){
				if(EMPTY != tmp_vrtx.ptr->t_ptr->rout_cost && FALSE == tmp_vrtx.ptr->t_ptr->srch_stat){
					if(tmp_vrtx.ptr->t_ptr->rout_cost < min_cost || EMPTY == min_cost){
						if(EMPTY == trgt->orgn_num && EMPTY == trgt->dst_num)
							trgt->dst_num = i;
						else{
							trgt->orgn_num = i;
							trgt->dst_num = tmp_vrtx.ptr->num;
						}
						cpyTim(tmp_vrtx.ptr->t_ptr->bgn_tim, &trgt->bgn);
						cpyTim(tmp_vrtx.ptr->t_ptr->end_tim, &trgt->end);
						cpyTim(tmp_vrtx.ptr->t_ptr->arrv_tim, &trgt->curr);
						min_cost = trgt->rout_cost = tmp_vrtx.ptr->t_ptr->rout_cost;
					}
				}
				tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_ptr->next;
			}
			tmp_vrtx.ptr = tmp_vrtx.ptr->next;
		}
	}
	if(EMPTY == min_cost){
		fprintf(stderr, "%d are not connected graph\n", trgt->orgn_num);
		exit(1);
	}
	counter++;
	return;
}

/**
*最小コストを持つ頂点に対する例外処理．
*最初の繰り返し時，最小コストを持つ辺は
EMPTY→dptr_numを結ぶ辺とし，
出発時刻が属する空間とする．
また，辺を通過し終わる時刻currには，
出発時刻が格納される．
*/
static void exceptTrgtVrtx(tim_expd_edge_t *trgt, int dptr_num, tim_t dptr){
	if(trgt->orgn_num == EMPTY){
		trgt->dst_num = dptr_num;
		trgt->rout_cost = 0.0f;
		cpyTim(dptr, &trgt->curr);
	}
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

/* 頂点の到達コストの更新条件を満たすかを判定する */
static int condiUpdtRoutCost(double befr_cost, double aftr_cost){
	if(EMPTY == befr_cost || befr_cost > aftr_cost)
		return TRUE;
	else
		return FALSE;
}

/* キューにおけるコスト，到達時刻，通過頂点を保存する． */
static void updatPathQue(dij_adj_list_t *edge, tim_expd_edge_t trgt, double wait_sec){
	edge->t_ptr->rout_cost = trgt.rout_cost + edge->t_ptr->edge_cost + wait_sec;
	cpyTim(trgt.curr, &edge->t_ptr->arrv_tim);
	edge->t_ptr->arrv_tim.sec += edge->edge_trvt + wait_sec;
	fixTimFormat(&edge->t_ptr->arrv_tim);
	edge->t_ptr->prev.orgn_num = trgt.orgn_num;
	cpyTim(trgt.bgn, &edge->t_ptr->prev.bgn);
	cpyTim(trgt.end, &edge->t_ptr->prev.end);
	cpyTim(trgt.curr, &edge->t_ptr->prev.curr);

	return;
}


/* ダイクストラ法による経路探索を行う． */
void Dijkstra(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr){
	double wait_cost = 0.0f, wait_sec = 0.0f;
	dij_vertex_t *vrtx = NULL;
  int vrtx_sz, enbl_tim_spac_trans;
  vrtx = cpyVrtx(vrtx_st, vrtx, &vrtx_sz);
	initDijVrtx(vrtx, vrtx_sz);
	initDptrVrtx(vrtx[dptr_num].ptr, dptr);

	tim_expd_edge_t trgt;
	initTrgtVrtx(&trgt);
	while(1){
		/* 出発地から最もコストの少ない辺を探索し， それを探索対象辺とする．*/
		findTrgtVrtx(&trgt, vrtx, vrtx_sz);
		exceptTrgtVrtx(&trgt, dptr_num, dptr);
		/* 目的地が始端である辺が探索対象辺であるならな，アルゴリズム終了 */
		if(trgt.orgn_num == arrv_num)
			break;
		/* 出発点からの最小コストを持つ辺を未探索状態から探索済状態にする */
		swtchSrchStatFrmFalse2True(trgt, vrtx[trgt.orgn_num].head);
		
		/* 最小コストを持つ辺の終端頂点から伸びる辺に対する繰り返し */
		vrtx[trgt.dst_num].ptr = vrtx[trgt.dst_num].head;
		while(NULL != vrtx[trgt.dst_num].ptr){
			enbl_tim_spac_trans = FALSE;
			/* 最小コストを持つ辺の終端頂点から伸びる辺の時間空間を走査していく */
			vrtx[trgt.dst_num].ptr->t_ptr = vrtx[trgt.dst_num].ptr->t_head;
			while(NULL != vrtx[trgt.dst_num].ptr->t_ptr){
				/* 最小コストを持つ辺を通過し終わる時刻が位置する時間空間を見つけたならば， */
				/* それ以降の時間空間へは遷移可能とする． */
				if(TRUE == existTimSpac(trgt.curr, vrtx[trgt.dst_num].ptr->t_ptr->bgn_tim,
				vrtx[trgt.dst_num].ptr->t_ptr->end_tim ))
					enbl_tim_spac_trans = TRUE;
				/* もし時間空間へ遷移可能ならば，その時間空間の辺を通過する為の待ち時間wait_secを求める． */
				if(TRUE == enbl_tim_spac_trans){
					wait_sec = getWaitTimSec(vrtx[trgt.dst_num].ptr->t_ptr->bgn_tim, trgt.curr);
					wait_cost = wait_sec;
					/* 現在の到達コストのより更にコストの低い経路を見つけたならば，到達コストを更新する*/
					if(TRUE == condiUpdtRoutCost(vrtx[trgt.dst_num].ptr->t_ptr->rout_cost,
					trgt.rout_cost + wait_cost))
						updatPathQue(vrtx[trgt.dst_num].ptr, trgt, wait_cost);
				}
				vrtx[trgt.dst_num].ptr->t_ptr = vrtx[trgt.dst_num].ptr->t_ptr->next;
			}
			vrtx[trgt.dst_num].ptr = vrtx[trgt.dst_num].ptr->next;
		}
		fprintf(stdout, "***************************************************\n");
		printDijVertexTimSpce(vrtx, vrtx_sz);
		fprintf(stdout, "***************************************************\n\n\n");
		fprintf(stdout, "%f\n", trgt.rout_cost);
	}
  return;
}

void srchRoute(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr){

  Dijkstra(vrtx_st, dptr_num, arrv_num, dptr);
}
