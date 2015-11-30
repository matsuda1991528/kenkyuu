#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"header.h"

#define PAI 3.14159265358979323846
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ADD(a, b) (a + b)
#define ABS(a) ((a) < 0 ? - (a) : (a))


static double getRadian(double deg){
	return deg * M_PI / 180.0f;
}

/** 
太陽と観測地を結ぶ直線の式と建物座標の垂線の長さを求める
y=ax+cの形で求める為，これをax+by+c = 0の形に変換した場合，
ax-y+c=0となる．これより，
bは必ず-1となる為，垂線を求める式は予め定数‐1を用いている．

http://www004.upp.so-net.ne.jp/s_honma/urawaza/distance.htm
 */
static double getPerpendicularDist(double azimuth, struct xy_coord_t observe_pos, struct xy_coord_t build_pos){
	double slope;  //傾き
	double intercept;  //切片
	
	slope = tan(getRadian(azimuth)); //太陽と観測地点間を通る直線の傾きを求める．
	intercept = observe_pos.y - observe_pos.x * slope; //太陽と観測地点間を通る直線の切片を求める．	
	return abs(build_pos.y - slope * build_pos.x - intercept) / sqrt(slope * slope + 1);
	
}


/** 
*2点間の距離を求める 
*@param origin_pos 原点座標
*@param target_pos 目的座標
*/
static double getDist(struct xy_coord_t origin_pos, struct xy_coord_t target_pos){
	return sqrt((target_pos.x - origin_pos.x) * (target_pos.x - origin_pos.x) + (target_pos.y - origin_pos.y) * (target_pos.y - origin_pos.y));
}

/**
*建物に対する影の長さを求める．
*@param sun_angle 太陽の位置
*@param build_height 建物の高さ
*@return xy_shadow_len x，y方向における日影ベクトルの長さ
*/
static struct xy_coord_t getShadowLen(struct sun_angle_t sun_angle, double build_height){
	double shadow_len;
	struct xy_coord_t xy_shadow_len;
	double tmp;
	
	shadow_len = build_height * (1 / tan(getRadian(sun_angle.elev)));
	tmp = shadow_len * sin(getRadian(sun_angle.azim));
	xy_shadow_len.y = ABS(tmp);

	tmp = shadow_len * cos(getRadian(sun_angle.azim));
	xy_shadow_len.x = ABS(tmp);

	return xy_shadow_len;
}

static struct xy_coord_t getMinPos(struct xy_coord_t fst_pos, struct xy_coord_t scn_pos){
	struct xy_coord_t min;
	
	min.x = MIN(fst_pos.x, scn_pos.x);
	min.y = MIN(fst_pos.y, scn_pos.y);
	
	return min;
}

static struct xy_coord_t getMaxPos(struct xy_coord_t fst_pos, struct xy_coord_t scn_pos){
	struct xy_coord_t max;
	
	max.x = MAX(fst_pos.x, scn_pos.x);
	max.y = MAX(fst_pos.y, scn_pos.y);
	
	return max;
}

/**
*src_bgn と src_endを対角頂点に持つ長方形に，trg_bgn, trg_endを対角頂点に持つ長方形が
*重複するかを判定する関数
*/
int dtctOvrRpWithRect(struct xy_coord_t src_bgn, struct xy_coord_t src_end, struct xy_coord_t trg_bgn, struct xy_coord_t trg_end){
	
	//srcの端点が左上(bgn)と右下(end)でとられた場合．
	if(src_bgn.x <= trg_end.x && trg_bgn.x <= src_end.x && 
		src_bgn.y <=  trg_end.y && trg_bgn.y <= src_end.y){
		return TRUE;
	}
	else
		return FALSE;
}
	

/**
*各グリッドに対する建物によるグレア発生判定処理
*
*1. 観測地点と太陽のなす直線の式と建物の位置との垂線の距離を算出する． 
*2. 建物の幅より垂線の距離が短いならば影の長さを算出する．
*　if 影の長さが建物との距離より長いならば，太陽は隠れると判断
*   otherwise 次の建物を参照し，1．へ
 */
 int serchGlareStateFromBuild(struct sun_angle_t sun_angle, struct xy_coord_t obs_pos, struct build_pos_t *head){
	struct build_pos_t *p;
	double perp_dist; //垂線の長さ
	double sh_dist; //影の長さ
	double bu_dist;//観測地‐建物間の距離

	p = head;
	p = p->next;
	while(p != NULL){
		perp_dist = getPerpendicularDist(sun_angle.azim, obs_pos, p->pos); //垂線の長さを求める
		//if(perp_dist >= p->width){
		sh_dist = BUILD_HIGH * (1 / tan(getRadian(sun_angle.elev)))
				+ fabs(cos(getRadian(sun_angle.azim))) * p->width;  //影の長さを求める
		bu_dist = getDist(obs_pos, p->pos);  //建物と観測地の距離を求める
		//if((sh_dist >= bu_dist || p->width >= bu_dist)){ //建物‐観測地間の距離と影の距離を比較
		if(sh_dist >= bu_dist && perp_dist <= p->width){
			return TRUE;
		}
		p = p->next;  //次の建物データを参照する
	}
	return FALSE;
}
 
 /**
* グリッド地図から建物に対するグレア発生を検出する関数
*@param sun_angle 太陽の角度
*@param obs_pos  観測地（自動車の現在地）
*@param build_grid : グリッド地図データセット
*@param grid_size : 南北方向と東西方向におけるグリッドのセル数
*@return グレアを遮蔽したならばTRUE それ以外はFALSE
 */
int getSunStateWithBuildFromGrid(struct sun_angle_t sun_angle, struct xy_coord_t obs_pos, struct build_grid_t** build_grid, struct grid_size_t grid_size){
	int glare_state; //グレア状態 TRUE -> グレアは発生しない	FALSE -> グレア発生
	struct xy_coord_t sh_tip_pos;  //影の先端座標 
	struct xy_coord_t sh_min, sh_max;
	int indx_bx, indx_by, indx_ex, indx_ey; //走査対象グリッドの始端及び終端インデックス番号
	int i, j;
	
	//建物の高さに対する影ベクトルとその東西，南北成分の長さを計算する
	struct xy_coord_t shadow_length = getShadowLen(sun_angle, BUILD_HIGH);
	
	/* 東西方向と南北方向における，自分の位置に届き得る影の最遠の距離を求める． */
	sh_tip_pos.x = obs_pos.x - (shadow_length.x);
	//太陽が北側から差し込む場合(日影が南側へ延びる)
	if(0 < sun_angle.azim && sun_angle.azim < 180) 
		sh_tip_pos.y = obs_pos.y + (shadow_length.y);
	//太陽が南側から差し込む場合(日影は北側へ延びる)
	else
		sh_tip_pos.y = obs_pos.y - (shadow_length.y);

//観測地と日影の伸び得る末端座標を対角頂点に持つ長方形の始端(min.x, min.y)と終端(max.x, max.y)を算出する．
	sh_min = getMinPos(obs_pos, sh_tip_pos);
	sh_max = getMaxPos(obs_pos, sh_tip_pos);
		
	//座標は走査対象エリアに位置しないが，所属するグリッド地図からはみ出た建物も走査対象とする為に，
	//グリッド抽出範囲を東西と南北に建物幅だけ長くする．
	sh_min.x = ADD(sh_min.x, -build_width);
	sh_min.y = ADD(sh_min.y, -build_width);
	sh_max.x = ADD(sh_max.x, build_width);
	sh_max.y = ADD(sh_max.y, build_width);
	
	//printf("%f, %f -> %f, %f\n", sh_min.x, sh_min.y, sh_max.x, sh_max.y);
	//printf("min (%f, %f) max(%f, %f)\n", build_grid[1][1].min.x, build_grid[1][1].min.y, build_grid[1][1].max.x, build_grid[1][1].max.y);
	
	indx_bx = ((int)sh_min.x - (int)build_grid[1][1].min.x) / (int)g_size.x;
	indx_by = ((int)sh_min.y - (int)build_grid[1][1].min.y) / (int)g_size.y;
	indx_ex = (((int)sh_max.x - (int)build_grid[1][1].min.x) / (int)g_size.x) + 1;
	indx_ey = (((int)sh_max.y - (int)build_grid[1][1].min.y) / (int)g_size.y) + 1;
	
	
	if(indx_bx < 1)
		indx_bx = 1;
	if(indx_by < 1)
		indx_by = 1;
	if((grid_size.width -1) < indx_ex)
		indx_ex = grid_size.width -1;
	if(grid_size.vertical - 1 < indx_ey)
		indx_ey = grid_size.vertical - 1;

		/* 適切なグリッドサイズに関する実験用走査方法 */
	if(GSZ_EXP == 0){
		for(i=1;i<grid_size.vertical;i++){
			for(j=1;j<grid_size.width;j++){
				if(indx_by <= i && i <= indx_ey){
					if(indx_bx <= j && j <= indx_ex){
						glare_state = serchGlareStateFromBuild(sun_angle, obs_pos, build_grid[i][j].head);
						if(glare_state == TRUE)
							return TRUE;
					}
				}
			}
		}
	}
	/* 通常の実験用走査方法(こっちの方が速い) */
	else{
		for(i=indx_by; i <= indx_ey; i++){
			for(j = indx_bx;j<=indx_ex;j++){
				glare_state = serchGlareStateFromBuild(sun_angle, obs_pos, build_grid[i][j].head);
				if(glare_state == TRUE)
					return TRUE;
			}
		}
		return FALSE;
	}
}
