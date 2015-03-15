#ifndef Param_H
#define Param_H


#define START 497
#define GOAL 498
#define MOVE_SPEED 600 //[km/h]
#define UNIT_OF_TRAVTIME 60   /* 移動時間単位[h]にしたい場合 -> 1,  [minuite]にしたい場合 -> 60 */
#define DATABASE_SIZE 25         /* step size:30[min] -> 24*2=48    step size:1[hour] -> 24*1=24 */
#define DATABASE_INTERVAL 2.0
#define MUTATION_METHOD 1 /* 0->通常の突然変異  1->西日特化突然変異 */

#define INF 9999   //map3の地図がある場合は999でやること．
#define NO_EXIST -1
#define MAX_WAIT_TIME 200

#define DROP_NODE_STYLE 1     /* 1->立ち寄り地をファイルから読み込み  2->立ち寄り地ランダム生成 */
#define DROP_NODE_NUM 5         /* DROP_NODE_STYLE=0の際に立ち寄り地数の選定 */

/* 西日直射範囲 */
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 50

#define START_HOUR 17
#define START_MIN 0
#define GENERATION 1000
#define IND_NUM 5

#endif