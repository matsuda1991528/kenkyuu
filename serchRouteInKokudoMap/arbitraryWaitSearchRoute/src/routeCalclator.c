#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"
#include "define.h"

void dtctFileOpn(FILE **fp, char *fname, char *mode);
void A_star(vertex_t *vrtx, int vrtx_sz, int dptr_num, int arrv_num, tim_t dptr);

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

/**
*時刻を表す構造体の各メンバの中身を空にする
*@param orgn コピー元の時刻
*@param *dst コピー先の時刻
*@return none
*/
static void initTim(tim_t *trgt){
	trgt->hour = EMPTY;
	trgt->min = EMPTY;
	trgt->sec = EMPTY;

	return;
}

/**
 各頂点の初期化
 経路におけるコスト，探索済フラグ，直前に通過したノード番号と時間空間，
 そして，到着時刻を初期化する．
 */
void initDijVrtx(vertex_t *vrtx, int vrtx_sz){
	int i;
	for(i=1;i<vrtx_sz;i++){
		vrtx[i].ptr = vrtx[i].head;
		while(NULL != vrtx[i].ptr){
			vrtx[i].ptr->t_ptr = vrtx[i].ptr->t_head;
			while(NULL != vrtx[i].ptr->t_ptr){
				/* ここに初期化処理を書く */
				vrtx[i].ptr->t_ptr->dij_meta.path_cost = EMPTY;
        vrtx[i].ptr->t_ptr->dij_meta.est_cost = EMPTY;
				vrtx[i].ptr->t_ptr->dij_meta.find_state = FALSE;
				vrtx[i].ptr->t_ptr->dij_meta.prev.orgn_num = EMPTY;
				initTim(&vrtx[i].ptr->t_ptr->dij_meta.prev.bgn);
				initTim(&vrtx[i].ptr->t_ptr->dij_meta.prev.end);
				initTim(&vrtx[i].ptr->t_ptr->dij_meta.arrv_tim);

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
*@param trgt 判定対象時刻
*@param bgn 始端時刻
*@param end 始端時刻
*/
int existTimSpac(tim_t trgt, tim_t bgn, tim_t end){
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
*時刻をそれぞれ60進数に変換する
*@param tim 変換対象時刻
*/
void fixTimFormat(tim_t *tim){
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
*@param *edge 初期化対象の辺
*@param dptr 出発時刻
*@return none
*/
void initDptrVrtx(adj_list_t *edge, tim_t dptr){
	while(NULL != edge->t_ptr){
		if(TRUE == existTimSpac(dptr, edge->t_ptr->bgn_tim, edge->t_ptr->end_tim)){
			edge->t_ptr->dij_meta.path_cost = edge->t_ptr->edge_cost;
			edge->t_ptr->dij_meta.arrv_tim.hour = dptr.hour;
			edge->t_ptr->dij_meta.arrv_tim.min = dptr.min;
			edge->t_ptr->dij_meta.arrv_tim.sec = edge->edge_trvt;
			fixTimFormat(&edge->t_ptr->dij_meta.arrv_tim);
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
void initTrgtVrtx(tim_expd_edge_t *trgt){
	trgt->orgn_num = EMPTY;
	trgt->dst_num = EMPTY;
	trgt->bgn.hour = trgt->bgn.min = trgt->bgn.sec = EMPTY;
	trgt->end.hour = trgt->end.min = trgt->end.sec = EMPTY;
	trgt->passed.hour = trgt->passed.min = trgt->passed.sec = EMPTY;
	trgt->rout_cost =EMPTY;

	return;
}

/**
*未探索状態から探索済状態へ遷移する．
*しかし，最初の繰り返し時は，どの辺も探索済とならない．
*@param 状態遷移対象の辺情報が格納されたもの
*@param edge_head 状態遷移対象の辺
*@return none
*/
static void swtchSrchStatFrmFalse2True(tim_expd_edge_t trgt, adj_list_t *edge_head){
	adj_list_t *edge = edge_head;
	if(EMPTY != trgt.orgn_num){
		while(NULL != edge){
			/*もし，2次元座標における辺が一致したならば
			時間空間において一致する辺を探す */
			if(trgt.dst_num == edge->num){
				edge->t_ptr = edge->t_head;
				while(NULL != edge->t_ptr){
					if(TRUE == existTimSpac(trgt.bgn, edge->t_ptr->bgn_tim, edge->t_ptr->end_tim)){
						edge->t_ptr->dij_meta.find_state = TRUE;
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
void findTrgtVrtx(tim_expd_edge_t *trgt, vertex_t *vrtx, int vrtx_sz){
	vertex_t tmp_vrtx;
	int i;
	double min_cost = EMPTY;
	/* 各頂点に対する繰り返し */
	for(i=1;i<vrtx_sz;i++){
		/* 各頂点iに(2次元の概念において)隣接する頂点を走査していく． */
		tmp_vrtx.ptr = vrtx[i].head;
		while(NULL != tmp_vrtx.ptr){
			/* 頂点iと頂点tmp_vrtx.ptr->numを結ぶ辺の時間空間を走査していく */
			tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
			while(NULL != tmp_vrtx.ptr->t_ptr){
				if(EMPTY != tmp_vrtx.ptr->t_ptr->dij_meta.path_cost && FALSE == tmp_vrtx.ptr->t_ptr->dij_meta.find_state){
					if(tmp_vrtx.ptr->t_ptr->dij_meta.path_cost < min_cost || EMPTY == min_cost){
						if(EMPTY == trgt->orgn_num && EMPTY == trgt->dst_num)
							trgt->dst_num = i;
						else{
							trgt->orgn_num = i;
							trgt->dst_num = tmp_vrtx.ptr->num;
						}
						cpyTim(tmp_vrtx.ptr->t_ptr->bgn_tim, &trgt->bgn);
						cpyTim(tmp_vrtx.ptr->t_ptr->end_tim, &trgt->end);
						cpyTim(tmp_vrtx.ptr->t_ptr->dij_meta.arrv_tim, &trgt->passed);
						min_cost = trgt->rout_cost = tmp_vrtx.ptr->t_ptr->dij_meta.path_cost;
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
	return;
}

/**
*最小コストを持つ頂点に対する例外処理．
*最初の繰り返し時，最小コストを持つ辺は
EMPTY→dptr_numを結ぶ辺とし，
出発時刻が属する空間とする．
また，辺を通過し終わる時刻passedには，
出発時刻が格納される．
*/
void exceptTrgtVrtx(tim_expd_edge_t *trgt, int dptr_num, tim_t dptr){
	if(trgt->orgn_num == EMPTY){
		trgt->dst_num = dptr_num;
		trgt->rout_cost = 0.0f;
		cpyTim(dptr, &trgt->passed);
    trgt->bgn.hour = trgt->bgn.min = 0;
    trgt->bgn.sec = 0.0f;
    trgt->end.hour = 23;
    trgt->end.min = 59;
    trgt->end.sec = 59.9f;
	}
	return;
}

static double cvthour2Sec(int hour){
	return (double)hour * 60.0f * 60.0f;
}

static double cvtMin2Sec(int min){
	return (double)min * 60.0f;
}

double getWaitTimSec(tim_t dst, tim_t orgn){
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
static void updatPathQue(adj_list_t *edge, tim_expd_edge_t trgt, double wait_sec){
  /* 辺の到達コストの更新．直前に通過した辺への到達コストにその辺の移動コストと待ち時間を加算する． */
  edge->t_ptr->dij_meta.path_cost = trgt.rout_cost + edge->t_ptr->edge_cost + wait_sec;

  /* 辺の到達時刻の更新．直前に通過した辺を通過し終わる時刻にその辺の所要時間と待ち時間を加算する． */
  /* その後，時刻を60進数に修正する */
	cpyTim(trgt.passed, &edge->t_ptr->dij_meta.arrv_tim);
  edge->t_ptr->dij_meta.arrv_tim.sec += edge->edge_trvt + wait_sec;
	fixTimFormat(&edge->t_ptr->dij_meta.arrv_tim);

  /* 隣接辺の直前に通過した辺情報を更新する． */
	edge->t_ptr->dij_meta.prev.orgn_num = trgt.orgn_num;
	edge->t_ptr->dij_meta.prev.dst_num = trgt.dst_num;
	cpyTim(trgt.bgn, &edge->t_ptr->dij_meta.prev.bgn);
	cpyTim(trgt.end, &edge->t_ptr->dij_meta.prev.end);
	cpyTim(trgt.passed, &edge->t_ptr->dij_meta.prev.passed);

	return;
}

void printOptmPath(tim_expd_edge_t prev, vertex_t *vrtx, int dptr_num,
int arrv_num, tim_t dptr){
	vertex_t tmp_vrtx;
	FILE *path_vrtx_num_fp, *path_vrtx_pos_fp;
	/* 注意：sprintfに文字列を渡す時，ポインタ型だと，segmentation faultを引き起こす． */
	char path_vrtx_num_fname[100], path_vrtx_pos_fname[100];
	/* ファイル名の生成 */
	sprintf(path_vrtx_pos_fname, "..\\outputfile\\optimumPath\\DAT\\%d_%d_%d_%d.dat",
	dptr_num, arrv_num, dptr.hour, dptr.min);
	sprintf(path_vrtx_num_fname, "..\\outputfile\\optimumPath\\TEXT\\%d_%d_%d_%d.txt",
	dptr_num, arrv_num, dptr.hour, dptr.min);
	/* ファイルを開く */
	dtctFileOpn(&path_vrtx_num_fp, path_vrtx_num_fname, "w");
	dtctFileOpn(&path_vrtx_pos_fp, path_vrtx_pos_fname, "w");

	while(EMPTY != prev.orgn_num){
    if(arrv_num != prev.orgn_num){
  		fprintf(path_vrtx_num_fp, "%5d <- %5d time %2d:%2d:%2.0f<--%2d:%2d:%2.0f  arrv %2d:%2d:%2.0f\n",prev.dst_num,
  		prev.orgn_num, prev.end.hour, prev.end.min, prev.end.sec,
  		prev.bgn.hour, prev.bgn.min, prev.bgn.sec, prev.passed.hour, prev.passed.min, prev.passed.sec);
      fprintf(path_vrtx_num_fp, "est cost %f passed cost %f\n", prev.est_cost, prev.rout_cost);
  		fprintf(path_vrtx_pos_fp, "%f %f\n", vrtx[prev.dst_num].pos.x, vrtx[prev.dst_num].pos.y);
    }
		/* 直前に通過した辺の始端点が終端点となる辺に対する繰り返し */
		tmp_vrtx.ptr = vrtx[prev.orgn_num].head;
		while(NULL != tmp_vrtx.ptr){
			if(tmp_vrtx.ptr->num == prev.dst_num){
				tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
				while(NULL != tmp_vrtx.ptr->t_ptr){
					if(TRUE == existTimSpac(prev.bgn, tmp_vrtx.ptr->t_ptr->bgn_tim,
					tmp_vrtx.ptr->t_ptr->end_tim)){
						prev = tmp_vrtx.ptr->t_ptr->dij_meta.prev;
						if(EMPTY == prev.orgn_num)
							break;
					}
					tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_ptr->next;
				}
			}
			tmp_vrtx.ptr = tmp_vrtx.ptr->next;
		}
	}
	fprintf(path_vrtx_pos_fp, "%f %f\n", vrtx[dptr_num].pos.x, vrtx[dptr_num].pos.y);
	fclose(path_vrtx_num_fp);
	fclose(path_vrtx_pos_fp);
	return;
}

static tim_expd_edge_t findPrevEdge(tim_expd_edge_t passed, vertex_t *vrtx){
	vertex_t tmp_vrtx;
	tmp_vrtx.ptr = vrtx[passed.orgn_num].head;
	while(NULL != tmp_vrtx.ptr){
		if(tmp_vrtx.ptr->num == passed.dst_num){
			tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_head;
			while(NULL != tmp_vrtx.ptr->t_ptr){
				if(TRUE == existTimSpac(passed.bgn, tmp_vrtx.ptr->t_ptr->bgn_tim, tmp_vrtx.ptr->t_ptr->end_tim)){
					return tmp_vrtx.ptr->t_ptr->dij_meta.prev;
				}
				tmp_vrtx.ptr->t_ptr = tmp_vrtx.ptr->t_ptr->next;
			}
		}
		tmp_vrtx.ptr = tmp_vrtx.ptr->next;
	}
	fprintf(stderr, "this path is wrong\n");
	return tmp_vrtx.ptr->t_ptr->dij_meta.prev;
}

/**
*待ち時間が発生した頂点をファイル出力する．
*@param passed 経路上に存在する辺集合のうち，現在参照している辺
*@param *vrtx 交差点を示す頂点集合
*@param dptr_num 出発地点を示す頂点の番号
*@param arrv_num 到着地点を示す頂点の番号
*@param dptr 出発時刻
*@return none
*/
void printWaitedPartPath(tim_expd_edge_t curr, vertex_t *vrtx, int dptr_num,
int arrv_num, tim_t dptr){
	FILE *wait_vrtx_pos_fp;
	char wait_vrtx_pos_fname[100];
	sprintf(wait_vrtx_pos_fname, "..\\outputfile\\waitedPartialPath\\DAT\\%d_%d_%d_%d.dat",
	dptr_num, arrv_num, dptr.hour, dptr.min);
	dtctFileOpn(&wait_vrtx_pos_fp, wait_vrtx_pos_fname, "w");

	tim_expd_edge_t prev;
	double diff_sec, curr_sec, prev_sec;

	/* まず，経路に含まれる辺集合の中で，現在参照している辺と，その直後に通過した辺を取り出す． */
	/* 次に，現在参照している辺の時間空間の通過し終わった時刻が，その直後に通過した時間拡大辺の */
	/* 始端時刻に達していないならば，何分か待った後に，移動した事が分かる為，待ち時間が発生した頂点座標を出力する． */
	for(;;){
		prev = findPrevEdge(curr, vrtx);
		if(EMPTY == prev.orgn_num)
			break;
		curr_sec = curr.bgn.hour * 60.0f * 60.0f + curr.bgn.min * 60.0f + curr.bgn.sec;
		prev_sec = prev.passed.hour * 60.0f * 60.0f + prev.passed.min * 60.0f + prev.passed.sec;
		diff_sec = curr_sec - prev_sec;
		if(0.0f < diff_sec){
			fprintf(wait_vrtx_pos_fp, "%f %f\n", vrtx[curr.orgn_num].pos.x, vrtx[curr.orgn_num].pos.y);
			//fprintf(wait_vrtx_pos_fp, "%f %f\n\n", vrtx[curr.dst_num].pos.x, vrtx[curr.dst_num].pos.y);
		}
		curr = prev;
	}

	return;
}

/* ダイクストラ法による経路探索を行う． */
void Dijkstra(vertex_t *vrtx, int vrtx_sz, int dptr_num, int arrv_num, tim_t dptr){
	double wait_cost = 0.0f, wait_sec = 0.0f;
  int enbl_tim_spac_trans;

	initDijVrtx(vrtx, vrtx_sz);
	initDptrVrtx(vrtx[dptr_num].ptr, dptr);
	tim_expd_edge_t trgt;
	initTrgtVrtx(&trgt);
	clock_t start = clock();
	while(1){
		/* 出発地から最もコストの少ない辺を探索し， それを探索対象辺とする．*/
		findTrgtVrtx(&trgt, vrtx, vrtx_sz);
		exceptTrgtVrtx(&trgt, dptr_num, dptr);

		/* 出発点からの最小コストを持つ辺を未探索状態から探索済状態にする */
		if(EMPTY != trgt.orgn_num)
			swtchSrchStatFrmFalse2True(trgt, vrtx[trgt.orgn_num].head);
		/* 目的地が始端である辺が探索対象辺であるならな，アルゴリズム終了 */
		if(trgt.dst_num == arrv_num)
			break;

		/* 最小コストを持つ辺の終端頂点から伸びる辺に対する繰り返し */
		vrtx[trgt.dst_num].ptr = vrtx[trgt.dst_num].head;
		while(NULL != vrtx[trgt.dst_num].ptr){
			enbl_tim_spac_trans = FALSE;
			/* 最小コストを持つ辺の終端頂点から伸びる辺の時間空間を走査していく */
			vrtx[trgt.dst_num].ptr->t_ptr = vrtx[trgt.dst_num].ptr->t_head;
			while(NULL != vrtx[trgt.dst_num].ptr->t_ptr){
				/* 最小コストを持つ辺を通過し終わる時刻が位置する時間空間を見つけたならば， */
				/* それ以降の時間空間へは遷移可能とする． */
				if(TRUE == existTimSpac(trgt.passed, vrtx[trgt.dst_num].ptr->t_ptr->bgn_tim,
				vrtx[trgt.dst_num].ptr->t_ptr->end_tim ))
					enbl_tim_spac_trans = TRUE;
				/* もし時間空間へ遷移可能ならば，その時間空間の辺を通過する為の待ち時間wait_secを求める． */
				if(TRUE == enbl_tim_spac_trans){
					wait_sec = getWaitTimSec(vrtx[trgt.dst_num].ptr->t_ptr->bgn_tim, trgt.passed);
					wait_cost = wait_sec;
					/* 現在の到達コストのより更にコストの低い経路を見つけたならば，到達コストを更新する*/
					if(TRUE == condiUpdtRoutCost(vrtx[trgt.dst_num].ptr->t_ptr->dij_meta.path_cost,
					trgt.rout_cost + vrtx[trgt.dst_num].ptr->t_ptr->edge_cost + wait_cost)){
						updatPathQue(vrtx[trgt.dst_num].ptr, trgt, wait_cost);
					}
				}
				vrtx[trgt.dst_num].ptr->t_ptr = vrtx[trgt.dst_num].ptr->t_ptr->next;
			}
			vrtx[trgt.dst_num].ptr = vrtx[trgt.dst_num].ptr->next;
		}
	}
	if(TRUE == PRINT_ROUTE){
		printOptmPath(trgt, vrtx, dptr_num, arrv_num, dptr);
		printWaitedPartPath(trgt, vrtx, dptr_num, arrv_num, dptr);
	}
	fprintf(stdout, "\nOptimum path's cost is %f\n", trgt.rout_cost);
	clock_t end = clock();
	fprintf(stdout, "process time %lf[ms]", (double)(end - start));
  return;
}

void srchRoute(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr){
	if(DIJKSTRA == find_path_algrthm){
  	Dijkstra(vrtx_st.indx, vrtx_st.sz, dptr_num, arrv_num, dptr);
	}
	else if(A_STAR == find_path_algrthm){
		A_star(vrtx_st.indx, vrtx_st.sz, dptr_num, arrv_num, dptr);
	}
	else{
		fprintf(stderr, "Usage: find_path_algrthm -> 0 find path by dijkstra\n");
		fprintf(stderr, "     : find_path_algrthm -> 1 find path by a_star\n");
	}
}
