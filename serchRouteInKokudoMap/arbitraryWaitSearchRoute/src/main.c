#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"
#include "define.h"

double getDegBtwn2pos(struct xy_coord_t p1, struct xy_coord_t p2);

typedef struct input_t{
  int dptr_vrtx; //出発地を表す頂点
  int arrv_vrtx; //帰着地を表す頂点
  tim_t dptr_tim;//出発時刻
  double kph; //移動速度(km/h)
  double lambda; //不快さを表すパラメータ
  int rt_mode; //経路探索モード
  xy_coord_t grd_len; //建物グリッド地図の長さ
}input_t;

/**
*入力データを格納する変数の初期化
*/
void initInptData(input_t* inpt){
  inpt->dptr_vrtx = EMPTY;
  inpt->arrv_vrtx = EMPTY;
  inpt->dptr_tim.hour = EMPTY;
  inpt->dptr_tim.min = EMPTY;
  inpt->dptr_tim.sec = 0.0f;
  inpt->kph = EMPTY;
  inpt->lambda = EMPTY;
  inpt->rt_mode = EMPTY;
  inpt->grd_len.y = EMPTY;
  inpt->grd_len.x = EMPTY;
  return;
}

/**
実行時で受け取った入力パラメータを各変数に格納していく．
 */
void setInptData(int argc, char **argv, input_t* inpt){
  while(--argc > 0 && **(++argv) == '-'){
    argc--;
    if(strcmp(*argv, "-start") == 0){
      inpt->dptr_vrtx = atoi(*++argv);
    }
    else if(strcmp(*argv, "-goal") == 0){
      inpt->arrv_vrtx = atoi(*++argv);
    }
	  else if(strcmp(*argv, "-dptr_h") == 0){
		    inpt->dptr_tim.hour = atoi(*++argv);
	  }
		else if(strcmp(*argv, "-dptr_m") == 0){
			inpt->dptr_tim.min = atoi(*++argv);
		}
    else if(strcmp(*argv, "-dptr_s") == 0){
      inpt->dptr_tim.sec = atof(*++argv);
    }
		else if(strcmp(*argv, "-kph") == 0){
			inpt->kph = atof(*++argv);
		}
		else if(strcmp(*argv, "-gx") == 0){
			inpt->grd_len.x = atof(*++argv);
		}
		else if(strcmp(*argv, "-gy") == 0){
			inpt->grd_len.y = atof(*++argv);
		}
		else if(strcmp(*argv, "-lam") == 0){
			inpt->lambda = atof(*++argv);
		}
		else if(strcmp(*argv, "-mode")==0){
			inpt->rt_mode = atoi(*++argv);
    }
    else if(strcmp(*argv, "-tim_intrvl") == 0){
      intrvl_sec = atoi(*++argv);
		}
    else if(strcmp(*argv, "-alg") == 0){
      find_path_algrthm = atoi(*++argv);
    }
    else if(strcmp(*argv, "-edc") == 0){
      allct_cost_type = atoi(*++argv);
    }
    else{
			fprintf(stderr,"invailed command\n");
			exit(1);
		}
  }
	return;
}

/* 出発地から目的地までの方位によるインスタンスタイプの識別 */
int dtctDirectionInstance(xy_coord_t dptr_pos, xy_coord_t dst_pos){
  double drctn = getDegBtwn2pos(dptr_pos, dst_pos);

  if((0.0f <= drctn && drctn < 45.0f) || (315.0f <= drctn && drctn < 360.0f))
    return WEST2EAST;
  else if(45.0f <= drctn && drctn < 135.0f)
    return SOUTH2NORTH;
  else if(135.0f <= drctn && drctn < 225.0f)
    return EAST2WEST;
  else if(225.0f <= drctn && drctn < 315.0f)
    return NORTH2SOUTH;
  else{
    fprintf(stderr, "hyded bug maybe...maybe...\n");
    exit(1);
  }
}

void createCostOutputFile(int direction_instance, tim_t dptr){
  if(EAST2WEST == direction_instance)
    sprintf(cost_output_file, "..\\outputfile\\experiment\\VariousInstanceApproximate\\AllocMax\\East2West\\%d_%d.csv",
    dptr.hour, dptr.min);
  else if(WEST2EAST == direction_instance)
    sprintf(cost_output_file, "..\\outputfile\\experiment\\VariousInstanceApproximate\\AllocMax\\West2East\\%d_%d.csv",
    dptr.hour, dptr.min);
  else if(SOUTH2NORTH == direction_instance)
    sprintf(cost_output_file, "..\\outputfile\\experiment\\VariousInstanceApproximate\\AllocMax\\South2North\\%d_%d.csv",
    dptr.hour, dptr.min);
  else if(NORTH2SOUTH == direction_instance)
    sprintf(cost_output_file, "..\\outputfile\\experiment\\VariousInstanceApproximate\\AllocMax\\North2South\\%d_%d.csv",
    dptr.hour, dptr.min);
  else{
    fprintf(stderr, "Usage direction instance 1~3 however direction_instance = %d\n", direction_instance);
  }
}


int main(int argc, char **argv) {
  input_t inpt;
  initInptData(&inpt);
  setInptData(argc, argv, &inpt);
  lmbd = inpt.lambda;
  //find_path_algrthm = DIJKSTRA;
  //find_path_algrthm = A_STAR;

/* 入力ファイルから交差点データセットと建物データセットを読み込む */
  vertex_set_t vrtx_st;
  build_set_t bld_st;
  setInptFilesData(&vrtx_st, &bld_st);

 /* 出発地と帰着地点の方角におけるインスタンス種類を識別する */
  int direction_instance = dtctDirectionInstance(vrtx_st.indx[inpt.dptr_vrtx].pos,
  vrtx_st.indx[inpt.arrv_vrtx].pos);
  createCostOutputFile(direction_instance, inpt.dptr_tim);


/* グリッド地図を生成する */
  grid_size_t grd_cell_sz;
  build_grid_t **bld_grd;
  bld_grd = cretGrdMap(bld_st, inpt.grd_len, &grd_cell_sz);

  bef_indx_x = bef_indx_y = EMPTY;
  /* 時間拡大ネットワークを生成する． */
  if(0 >= intrvl_sec)
    cretTimExpdNtwk(vrtx_st, bld_grd, inpt.grd_len, grd_cell_sz, inpt.kph);
  else
    cretSttcTimExpdNtwk(vrtx_st, bld_grd, inpt.grd_len, grd_cell_sz, inpt.kph);
  fprintf(stdout, "finish\n");
  //exit(1);

  /* 経路を探索する */
  srchRoute(vrtx_st, inpt.dptr_vrtx, inpt.arrv_vrtx, inpt.dptr_tim,
  bld_grd, inpt.grd_len, grd_cell_sz);

  return 0;
}
