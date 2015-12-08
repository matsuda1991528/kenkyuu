#ifndef DEFINE_H
#define DEFINE_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ADD(a, b) (a + b)
#define ABS(a) ((a) < 0 ? - (a) : (a))

#define MAX_LEN 256
#define EMPTY -1
#define INF -1
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 30

#define BUILD_HIGH 5 //建物の高さ[m]
#define BUILD_WIDTH 5 //建物の幅[m]


#define PRINT_ROUTE 1 //0->経路を表示しない　1->経路を表示する
#define PRINT_GLARE_ROUTE 1 //0->眩しさを感じた部分経路を表示する．1->表示する
#define BUILD_MODE 1 // 0->建物を考慮しない　1->建物を考慮する
#define MEASURE_PROCESS_TIME_MODE 0 //0->処理時間を計測しない 1->処理時間を計測する
#define PRINT_GRID_MAP 0 //0->グリッド地図をファイル書き込みしない　1→ファイル書き込みする
#define BUILD_WIDTH_MODE 1 // 0->走査対象グリッド選定にて，取り扱う影幅を定数で取り扱う 1->最大建物幅で取り扱う．
#define GSZ_EXP 1 //適切なグリッドサイズを探す時→0　そうでない場合→1

#define TEST_GRID 1 //1→グリッド分割のテスト

#define TRUE 1
#define FALSE 0

#define YEAR 2015
#define MONTH 6
#define DAY 21

#endif
