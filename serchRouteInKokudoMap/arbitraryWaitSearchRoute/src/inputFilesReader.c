#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"
#include "define.h"

static void dtctFileOpn(FILE **fp, char *fname, char *mode){
  if((*fp = fopen(fname, mode)) == NULL){
    fprintf(stderr, "%s is failed open\n", fname);
    exit(1);
  }
  return;
}

/* 全ての頂点（交差点）を格納する為に必要となる要素サイズをinputfileから読み込む */
static int loadVrtxSz(char *VrtxFile){
  FILE *fp;
  char buf[256];
  int vrtx_sz = 1;

  dtctFileOpn(&fp, VrtxFile, "r");

  fgets(buf, 256, fp);
  while(fgets(buf, 256, fp) != NULL){
    vrtx_sz++;
  }
  fclose(fp);
  return vrtx_sz;
}

static void loadVrtxPos(char *VrtxFile, vertex_t *vrtx){
  FILE *fp;
  char buf[256];
  dtctFileOpn(&fp, VrtxFile, "r");
  fgets(buf, 256, fp);
  int indx = 1;
  while(fscanf(fp, "%d", &vrtx[indx].num) != EOF){
    fscanf(fp, "%s %s", buf, buf);
    fscanf(fp, "%lf %lf", &vrtx[indx].pos.x, &vrtx[indx].pos.y);
    indx++;
  }
  return;
}

/* 隣接リストの記憶領域の取得 */
struct adj_list_t *adjListMalloc(void){
	return (adj_list_t *)malloc(sizeof(adj_list_t));
}

void printVertex(vertex_t* vertex, int node_size){
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

void loadEdgeList(char *EdgeFile, vertex_t *vrtx, int vrtx_sz){
  FILE *fp;
  char buf[256];
  char *token;
  int indx = 1, cnt = 0;
  dtctFileOpn(&fp, EdgeFile, "r");

  while(indx <= vrtx_sz){
    vrtx[indx].head = adjListMalloc();
    vrtx[indx].old = vrtx[indx].ptr = vrtx[indx].head;
    indx++;
  }
  indx = 1;

  while(fgets(buf, 256, fp) != NULL){
    token = strtok(buf, " ");
    cnt = 0;
    while(token != NULL){
      if(0 < cnt && 0 != atoi(token)){
        vrtx[indx].ptr->num = atoi(token);
        vrtx[indx].old->next = vrtx[indx].ptr;
        vrtx[indx].old = vrtx[indx].ptr;
        vrtx[indx].ptr = adjListMalloc();
      }
      token = strtok(NULL, " ");
      cnt++;
    }
    free(vrtx[indx].ptr);
    vrtx[indx].old->next = NULL;
    indx++;
  }
  //printVertex(vrtx, vrtx_sz);
  fclose(fp);
  return;
}

/* build_pos_t型分のメモリを確保し，struct build_pos_t*　へキャストする */
build_pos_t *buildMalloc(void){
  return (build_pos_t *)malloc(sizeof(build_pos_t));
}

/* 2点間の座標を計算する */
static double getDist(xy_coord_t orgn, xy_coord_t dst){
	return sqrt((dst.x - orgn.x) * (dst.x - orgn.x) + (dst.y - orgn.y) * (dst.y - orgn.y));
}

/**
建物データファイルを読み込み，本システムのデータ形式に変換する．
詳細は以下の通り：
まず，ファイルから建物の索引番号と最大および最小位置座標を読み込む．
次に，読み込んだ位置座標に基づき，建物の中心座標を求める．
これらの操作を全ての建物に対し適用する事で，
建物のデータセットを作成する．
 */
void loadBldPos(char *BuildFile, build_set_t *bld_st){
  FILE *fp;
  dtctFileOpn(&fp, BuildFile, "r");
  xy_coord_t min_pos, max_pos;
  bld_st->sz = 1;
  bld_st->head = buildMalloc();
  bld_st->old = bld_st->head;

  while(bld_st->ptr = buildMalloc(), fscanf(fp, "%d", &bld_st->ptr->num) != EOF){
    fscanf(fp, "%lf %lf %lf %lf", &min_pos.x, &min_pos.y, &max_pos.x, &max_pos.y);
    bld_st->ptr->pos.x = (min_pos.x + max_pos.x) / 2;
    bld_st->ptr->pos.y = (min_pos.y + max_pos.y) / 2;
    bld_st->ptr->wdth = getDist(min_pos, max_pos);

    bld_st->old->next = bld_st->ptr;
    bld_st->old = bld_st->ptr;
    bld_st->sz++;
  }
  bld_st->old->next = NULL;
  fprintf(stdout, "build_num = %d\n", bld_st->sz);
  fclose(fp);
}

void setInptFilesData(vertex_set_t *vrtx_st, build_set_t* bld_st){
  
  char *VrtxFile = "../inputfile/node_position.txt";
  char *EdgeFile = "../inputfile/edge_list.txt";
  char *BuildFile = "../inputfile/building_position.txt";

/*
  char *VrtxFile = "../test_inputfile/sample1/node_position.txt";
  char *EdgeFile = "../test_inputfile/sample1/edge_list.txt";
  char *BuildFile = "../test_inputfile/sample1/building_position.txt";
*/
  vrtx_st->sz = loadVrtxSz(VrtxFile);

  vrtx_st->indx = (vertex_t *)malloc(sizeof(vertex_t) * vrtx_st->sz);
  if(vrtx_st->indx == NULL){
    fprintf(stderr, "cannnot malloc vertex size memory");
    exit(1);
  }
  loadVrtxPos(VrtxFile, vrtx_st->indx);
  loadEdgeList(EdgeFile, vrtx_st->indx, vrtx_st->sz);
  loadBldPos(BuildFile, bld_st);

  return;
}
