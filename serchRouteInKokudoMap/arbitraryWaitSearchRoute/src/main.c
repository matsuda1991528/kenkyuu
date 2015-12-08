#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"
#include "define.h"

typedef struct input_t{
  int dptr_vrtx; //出発地を表す頂点
  int arrv_vrtx; //帰着地を表す頂点
  tim_t dptr_tim;//出発時刻
  int kph; //移動速度(km/h)
  int lambda; //不快さを表すパラメータ
  int rt_mode; //経路探索モード
  xy_coord_t grd_sz; //建物グリッドサイズ
}input_t;

/**
*入力データを格納する変数の初期化
*/
void initInptData(input_t* inpt){
  inpt->dptr_vrtx = EMPTY;
  inpt->arrv_vrtx = EMPTY;
  inpt->dptr_tim.hour = EMPTY;
  inpt->dptr_tim.min = EMPTY;
  inpt->kph = EMPTY;
  inpt->lambda = EMPTY;
  inpt->rt_mode = EMPTY;
  inpt->grd_sz.y = EMPTY;
  inpt->grd_sz.x = EMPTY;
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
		else if(strcmp(*argv, "-kph") == 0){
			inpt->kph = atoi(*++argv);
		}
		else if(strcmp(*argv, "-gx") == 0){
			inpt->grd_sz.x = atof(*++argv);
		}
		else if(strcmp(*argv, "-gy") == 0){
			inpt->grd_sz.y = atof(*++argv);
		}
		else if(strcmp(*argv, "-lam") == 0){
			inpt->lambda = atoi(*++argv);
		}
		else if(strcmp(*argv, "-mode")==0){
			inpt->rt_mode = atoi(*++argv);
		}else{
			fprintf(stderr,"invailed command\n");
			exit(1);
		}
  }
	return;
}

int main(int argc, char **argv) {
  input_t inpt;
  initInptData(&inpt);
  setInptData(argc, argv, &inpt);

/* 入力ファイルから交差点データセットと建物データセットを読み込む */
  vertex_set_t vrtx_st;
  build_set_t bld_st;
  setInptFilesData(&vrtx_st, &bld_st);
/* グリッド地図を生成する */
  build_grid_t **bld_grd;
  bld_grd = cretGrdMap(bld_st, inpt.grd_sz);


  return 0;
}
