#ifndef GetGene_C
#define getGene_C
#include<stdio.h>
#include<stdlib.h>
#include"extern.h"

void getGene(){
	int i, j;
	int num;
	char dummy[255];
	char *fname = "map3/visitPos.txt";
	FILE *fp;
	fp = fopen(fname, "r");
	if(fp == NULL){
		printf("visitPos.txt open error\n");
		exit(1);
	}
	else{
		for(i=1;i<GENE_SIZE;i++){
			fscanf(fp, "%s %d\n", dummy, &dropPoint[i].num);
		}
	}
	for(i=1;i<GENE_SIZE;i++){
		printf("dropPoint[%d] = %d\n", i, dropPoint[i].num);
	}
	/* 遺伝子生成していくよ〜〜 */
	for(i=0;i<INDI_NUM;i++){
		for(j=1;j<GENE_SIZE;j++){
			dropPoint[j].flag = 0;
		}
		/* 最初と最後の遺伝子座には始発点と終着点を格納 */
		/* それ以外はランダムに格納 */
		for(j=1;j<GENE_SIZE;j++){
			if(j == 1 || j == GENE_SIZE-1){
				individual[i].gene[j] = dropPoint[j].num;
			}
			else{
				while(1){
					num = random()%(GENE_SIZE-3) + 2;
					if(dropPoint[num].flag == 0)
						break;
				}
				individual[i].gene[j] = dropPoint[num].num;
				dropPoint[num].flag = 1;
			}
		}
	}
	/* 待ち時間遺伝子の格納 */
	for(i=0;i<INDI_NUM;i++){
		individual[i].waitTime = random()%MAX_WAITTIME;
	}
}
#endif
		