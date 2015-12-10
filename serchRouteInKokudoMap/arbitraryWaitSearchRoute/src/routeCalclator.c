#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"
#include "define.h"

/* 時間空間を表す構造体(ダイクストラ法による経路探索用) */
typedef struct dij_time_space_list_t{
	tim_t bgn_tim; //時間空間Sへ遷移出来る最も早い時刻
	tim_t end_tim; //時間空間Sへ遷移出来る最も遅い時刻
	double edge_cost;
  double rout_cost;
	struct dij_time_space_list_t *next;
}dij_time_space_list_t;

/* 隣接リストを表す構造体(ダイクストラ法による経路探索用) */
typedef struct dij_adj_list_t{
	int num;
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

static void printDijVertex(dij_vertex_t* vertex, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		//printf("%5d %lf %lf |", vertex[i].node.num, vertex[i].node.pos.x, vertex[i].node.pos.y);
		fprintf(stdout,"|%5d| -> ", vertex[i].num);
		vertex[i].ptr = vertex[i].head;
		while(vertex[i].ptr != NULL){
			fprintf(stdout,"|%d|", vertex[i].ptr->num);
			vertex[i].ptr = vertex[i].ptr->next;
		}
		fprintf(stdout,"\n");
	}
	return;
}

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
  return;
}

static void printDijVertexTimSpce(dij_vertex_t* vertex, int node_size){
	int i;
	for(i=1;i<node_size;i++){
		//printf("%5d %lf %lf |", vertex[i].node.num, vertex[i].node.pos.x, vertex[i].node.pos.y);
		vertex[i].ptr = vertex[i].head;
		while(vertex[i].ptr != NULL){
			fprintf(stdout,"|%5d| -> ", vertex[i].num);
			fprintf(stdout,"|%5d|\n", vertex[i].ptr->num);
			vertex[i].ptr->t_ptr = vertex[i].ptr->t_head;
			while(NULL != vertex[i].ptr->t_ptr){
				fprintf(stdout, "bgn:%2d:%2d\t", vertex[i].ptr->t_ptr->bgn_tim.hour, vertex[i].ptr->t_ptr->bgn_tim.min);
				fprintf(stdout, "bgn:%2d:%2d\t", vertex[i].ptr->t_ptr->end_tim.hour, vertex[i].ptr->t_ptr->end_tim.min);
				fprintf(stdout, "cost = %f\n", vertex[i].ptr->t_ptr->edge_cost);

				vertex[i].ptr->t_ptr = vertex[i].ptr->t_ptr->next;
			}
			fprintf(stdout,"\n");
			vertex[i].ptr = vertex[i].ptr->next;
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

	printDijVertexTimSpce(dst, vrtx_sz);
  return;

}

static void cpyVrtx(vertex_set_t orgn_vrtx_st, dij_vertex_t *trgt_vrtx, int *trgt_vrtx_sz){
  *trgt_vrtx_sz = orgn_vrtx_st.sz;
  trgt_vrtx = (dij_vertex_t *)malloc(sizeof(dij_vertex_t) * *trgt_vrtx_sz);
  if(NULL == trgt_vrtx){
    fprintf(stderr, "cannnot malloc dij_vertex_t memory\n");
    exit(1);
  }

  cpyVrtxIndxAndPos(orgn_vrtx_st.indx, trgt_vrtx, *trgt_vrtx_sz);
  cpyVrtxAdjList(orgn_vrtx_st.indx, trgt_vrtx, *trgt_vrtx_sz);
  cpyVrtxTimSpceList(orgn_vrtx_st.indx, trgt_vrtx, *trgt_vrtx_sz);

}

void Dijkstra(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr){
  dij_vertex_t *vrtx = NULL;
  int vrtx_sz;
  cpyVrtx(vrtx_st, vrtx, &vrtx_sz);

  return;
}

void srchRoute(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr){

  Dijkstra(vrtx_st, dptr_num, arrv_num, dptr);
}
