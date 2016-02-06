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
*  経路探索用のデータ   *
**********************
**/
/* 経路探索において，直前に通過した頂点を表す構造体 */
typedef struct tim_expd_edge_t{
	int orgn_num;
	int dst_num;
	tim_t bgn; //辺が持つ時間空間の始端時刻
	tim_t end; //辺が持つ時間空間の終端時刻
	tim_t passed; //頂点への到達時刻
	double est_cost; //帰着点までの予測値(A*探索の時に使用)
	double rout_cost; //頂点への到達コスト
	double edge_trvt;
	double edge_cst; //辺のコスト
	double wait_sec;
	struct tim_expd_edge_t *prev;
}tim_expd_edge_t;

/* ダイクストラ法による経路探索用のメタデータ */
typedef struct dij_find_path_t{
	double est_cost;
	double path_cost;
	int find_state;
	tim_expd_edge_t prev;
	tim_t arrv_tim;
}dij_find_path_t;

typedef struct srch_state_t{
	double wait_sec;
	double arrv_cst;
	double odd_cst;
	double est_cst;
}srch_state_t;


/**
**********************
*   交差点関係のデータ     *
**********************
**/
/* 時間空間を表す構造体 */
typedef struct time_space_list_t{
	tim_t bgn_tim; //時間空間Sへ遷移できる始端時刻
	tim_t end_tim; //時間空間Sへ遷移できる終端時刻
	double high_cost; //最悪値
	double low_cost; //最良値
	tim_t arch_low_cost_tim; //最良値に達した時刻
	dij_find_path_t dij_meta;
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

/* 優先度つきキューを表す構造体 */
typedef struct priority_queue_t{
  tim_expd_edge_t edge;
	struct priority_queue_t *next;
}priority_queue_t;

typedef struct priority_queue_set_t{
	priority_queue_t *head;
	priority_queue_t *ptr;
	priority_queue_t *old;
	int sz;
}priority_queue_set_t;


/**
**********************
*   グローバリー変数  *
**********************
**/
double max_bld_wdth;
double lmbd;
int find_path_algrthm;
double vel;
int intrvl_sec;
int bef_indx_x;
int bef_indx_y;
int direction_instance;
char cost_output_file[100];
char ex_network_file[100];
int allct_cost_type;
double final_cost;
int map_type;


void setInptFilesData(vertex_set_t *vrtx_st, build_set_t* bld_st);
build_grid_t **cretGrdMap(build_set_t bld_st, xy_coord_t grd_len,
	grid_size_t *grd_cell_sz);
void cretTimExpdNtwk(vertex_set_t vrtx_st, build_grid_t** bld_grd,
	xy_coord_t grd_len, grid_size_t grd_cell_sz, double kph);
void cretSttcTimExpdNtwk(vertex_set_t vrtx_st, build_grid_t** bld_grd,
  xy_coord_t grd_len, grid_size_t grd_cell_sz, double kph);
void srchRoute(vertex_set_t vrtx_st, int dptr_num, int arrv_num, tim_t dptr,
build_grid_t **bld_grd, xy_coord_t grd_len, grid_size_t grd_cell_sz);

#endif
