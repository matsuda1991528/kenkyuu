#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

/* ファイルオープンする際のNULLチェック */
void fileOpenCheck(FILE **fp, char *filename, char *mode){
	if((*fp = fopen(filename, mode)) == NULL){
		printf("%s is failed open\n", filename);
		exit(1);
	}
	return;
}

/* エリートデータの初期化 */
void initElite(int node_size){
	elite.uncom = 9999;
	elite.prev = (int *)malloc(sizeof(int) * node_size);
	return;
}

void updateElite(double target_uncom, double target_wait, int* prev, int node_size){
	int i;
	elite.uncom = target_uncom;
	elite.wait_time = target_wait;
	for(i=1;i<node_size;i++){
		elite.prev[i] = prev[i];
	}
	return;
}
	