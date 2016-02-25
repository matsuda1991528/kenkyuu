#ifndef DEFINE_H
#define DEFINE_H

/* デバッグ用マクロ */
#define dprintf(s,...) fprintf(stderr, "%s:%d: " s, __FILE__, __LINE__,__VA_ARGS__)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ADD(a, b) (a + b)
#define ABS(a) ((a) < 0 ? - (a) : (a))

#define MAX_LEN 255

#define EMPTY -1
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 30

#define BUILD_HIGH 5 //建物の高さ[m]

#define EAST2WEST 0
#define WEST2EAST 1
#define SOUTH2NORTH 2
#define NORTH2SOUTH 3


/* 経路探索するアルゴリズム */

#define WRSTA_STAR 1
#define NA_STAR 2
#define MOA_STAR 3


#define TIME_EXPAND_TYPE 0
#define STATIC 0
#define GLARE 1

#define ALLOC_MAX 0
#define ALLOC_MIN 1
#define ALLOC_AVG 2
#define ALLOC_BOTH 3

/* 実験に使う地図の種類 */
#define MAP_TYPE 0
#define HIROSHIMA 0
#define SAMPLE1 1
#define SAMPLE2 2
#define SAMPLE1000 1000
#define SAMPLE2000 2000
#define SAMPLE3000 3000
#define SAMPLE4000 4000
#define SAMPLE5000 5000
#define SAMPLE10000 10000

#define PRINT_EXPENDED_EDGE 1 //時間拡大ネットワークの各辺を出力するか
#define PRINT_ROUTE 1 //0->経路を表示しない　1->経路を表示する
#define PRINT_GLARE_ROUTE 1 //0->眩しさを感じた部分経路を表示しない．1->表示する

#define MONITOR_PRIORITY_LIST 0 //プライオリティキューを監視する→1　監視しない→0

#define TRUE 1
#define FALSE 0

/* 経路探索当該日 */
#define YEAR 2015
#define MONTH 6
#define DAY 21

//#define EXTRA_ILLUMI 70000 //太陽法線照度
#define EXTRA_ILLUMI 134000 //太陽法線照度
#define ATMOS_TRANS 0.60    //大気透過率
#define SUN_LENGTH 149597870000 //太陽との距離
#define BACKGROUND_LUMINANCE 1500

#endif
