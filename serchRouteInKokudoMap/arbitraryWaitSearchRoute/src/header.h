#ifndef HEADER_H
#define HEADER_H

/* xy座標を表現する構造体 */
typedef struct xy_coord_t{
	double x;  //x座標
	double y;  //y座標
}xy_coord_t;

/* 時刻を表す構造体 */
typedef struct tim_t{
	int hour;
	int min;
	double sec;
}tim_t;

/* 太陽角度を表す構造体 */
typedef struct sun_angle_t{
	double elev;   //高度
	double azim;  //方位
}sun_angle_t;

/**
**********************
*   交差点関係のデータ     *
**********************
**/
/* 時間空間を表す構造体 */
typedef struct time_space_list_t{
	tim_t bgn_tim; //時間空間Sへ遷移できる始端時刻
	tim_t end_tim; //時間空間Sへ遷移できる終端時刻
	double cost;
	struct time_space_list_t *next;
}time_space_list_t;


/* 隣接リストを表す構造体 */
typedef struct adj_list_t{
	int num;
	double edge_trvt;
	time_space_list_t *t_head; //時間依存グレア状態
	time_space_list_t *t_ptr;
	time_space_list_t *t_old;
	struct adj_list_t *next;
}adj_list_t;


/* 頂点リストを表す構造体*/
typedef struct vertex_t{
	int num; //索引番号
	xy_coord_t pos; //2次元ユークリッド空間における位置座標
	adj_list_t *ptr;
	adj_list_t *head;
	adj_list_t *old;
}vertex_t;

typedef struct vertex_set_t{
	int sz; //頂点の総数
	vertex_t *indx; //地図上の交差点の総数(配列の宣言で定義すべきサイズ)
}vertex_set_t;

/**
**********************
*  建物関係のデータ      *
**********************
**/

/* 建物を表す構造体 */
typedef struct build_pos_t{
	int num;       //建物のインデックス番号
	double wdth; //建物幅
	xy_coord_t pos; //建物の中心位置
	struct build_pos_t *next;
}build_pos_t;

typedef struct build_set_t{
	build_pos_t *head; //先頭リストはダミー
	build_pos_t *ptr;
	build_pos_t *old;
	int sz; //地図上の建物の総数(配列の宣言で定義すべきサイズ)
}build_set_t;

typedef struct grid_size_t{
	int sn; //南北方向におけるグリッドサイズ
	int ew; //東西方向におけるグリッドサイズ
}grid_size_t;

/* 建物データグリッドマップを表す構造体 */
typedef struct build_grid_t{
	xy_coord_t bgn;  //グリッド地図の始端(最小)xy座標
	xy_coord_t end; //グリッド地図の終端(最大)xy座標
	build_pos_t *head;  //建物リスト(先頭リストはダミー)
	build_pos_t *ptr;
	build_pos_t *old;
}build_grid_t;

/**
**********************
*   グローバリー変数  *
**********************
**/
double max_bld_wdth;
double lmbd;

void setInptFilesData(vertex_set_t *vrtx_st, build_set_t* bld_st);
build_grid_t **cretGrdMap(build_set_t bld_st, xy_coord_t grd_len,
	grid_size_t *grd_cell_sz);
void cretTimExpdNtwk(vertex_set_t vrtx_st, build_grid_t** bld_grd,
	xy_coord_t grd_len, grid_size_t grd_cell_sz, double kph);
void srchRoute(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr);

#endif
