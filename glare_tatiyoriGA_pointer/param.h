#ifndef Param_H
#define Param_H


#define START 497
#define GOAL 498
#define MOVE_SPEED 600 //[km/h]
#define UNIT_OF_TRAVTIME 60   /* �ړ����ԒP��[h]�ɂ������ꍇ -> 1,  [minuite]�ɂ������ꍇ -> 60 */
#define DATABASE_SIZE 25         /* step size:30[min] -> 24*2=48    step size:1[hour] -> 24*1=24 */
#define DATABASE_INTERVAL 2.0
#define MUTATION_METHOD 1 /* 0->�ʏ�̓ˑR�ψ�  1->���������ˑR�ψ� */

#define INF 9999   //map3�̒n�}������ꍇ��999�ł�邱�ƁD
#define NO_EXIST -1
#define MAX_WAIT_TIME 200

#define DROP_NODE_STYLE 1     /* 1->�������n���t�@�C������ǂݍ���  2->�������n�����_������ */
#define DROP_NODE_NUM 5         /* DROP_NODE_STYLE=0�̍ۂɗ������n���̑I�� */

/* �������˔͈� */
#define SUN_HIGH 25
#define SUN_LOW 0
#define SUN_WIDTH 50

#define START_HOUR 17
#define START_MIN 0
#define GENERATION 1000
#define IND_NUM 5

#endif