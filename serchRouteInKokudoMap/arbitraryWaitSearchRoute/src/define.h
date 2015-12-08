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

#define BUILD_HIGH 5 //�����̍���[m]
#define BUILD_WIDTH 5 //�����̕�[m]


#define PRINT_ROUTE 1 //0->�o�H��\�����Ȃ��@1->�o�H��\������
#define PRINT_GLARE_ROUTE 1 //0->ῂ����������������o�H��\������D1->�\������
#define BUILD_MODE 1 // 0->�������l�����Ȃ��@1->�������l������
#define MEASURE_PROCESS_TIME_MODE 0 //0->�������Ԃ��v�����Ȃ� 1->�������Ԃ��v������
#define PRINT_GRID_MAP 0 //0->�O���b�h�n�}���t�@�C���������݂��Ȃ��@1���t�@�C���������݂���
#define BUILD_WIDTH_MODE 1 // 0->�����ΏۃO���b�h�I��ɂāC��舵���e����萔�Ŏ�舵�� 1->�ő匚�����Ŏ�舵���D
#define GSZ_EXP 1 //�K�؂ȃO���b�h�T�C�Y��T������0�@�����łȂ��ꍇ��1

#define TEST_GRID 1 //1���O���b�h�����̃e�X�g

#define TRUE 1
#define FALSE 0

#define YEAR 2015
#define MONTH 6
#define DAY 21

#endif
