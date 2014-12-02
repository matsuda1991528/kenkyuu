#ifndef GetData_C
#define GetData_C

#include<stdio.h>
#include<stdlib.h>
#include"extern.h"

void getMapData(){
	int i, j;
	int integerDummy;
	char characterDummy[255];
	int cnt = 0;
	
	char *fname1 = "map3/node.txt";
	char *fname2 = "map3/matrixDistance.txt";
	char *fname3 = "map3/matrixAngle.txt";
	FILE *fp1, *fp2, *fp3;
	
	fp1 = fopen(fname1, "r");
	if(fp1 == NULL){
		printf("file1 open error\n");
		exit(1);
	}
	else{
		fscanf(fp1, "%s", characterDummy);
		fscanf(fp1, "%d", &integerDummy);
		fscanf(fp1, "%s %s %s", characterDummy, characterDummy, characterDummy);
		for(i=1;i<NODE_NUM;i++){
			fscanf(fp1,"%d",&node[i].num);
			fscanf(fp1,"%d",&node[i].pos.x);
			fscanf(fp1,"%d",&node[i].pos.y);
		}
	}
	fclose(fp1);
//	for(i=1;i<nodeNum;i++)
//		printf("%d,%d\n",cord[i].x, cord[i].y);
	
	fp2 = fopen(fname2, "r");
	if(fp2 == NULL)
		printf("file2 open error\n");
	else{
		for(i=1;i<NODE_NUM;i++)
			fscanf(fp2,"%d",&integerDummy);
		for(i=1;i<NODE_NUM;i++){
			fscanf(fp2,"%d",&integerDummy);
			for(j=1;j<NODE_NUM;j++)
				fscanf(fp2,"%lf",&matrix[i][j].distance);
		}
	}
	fclose(fp2);
	
	fp3 = fopen(fname3, "r");
	if(fp3 == NULL)
		printf("file3 open error\n");
	else{
		for(i=1;i<NODE_NUM;i++)
			fscanf(fp3, "%d", &integerDummy);
		for(i=1;i<NODE_NUM;i++){
			fscanf(fp3, "%d", &integerDummy);
			for(j=1;j<NODE_NUM;j++)
				fscanf(fp3, "%lf", &matrix[i][j].angle);
		}
	}
	fclose(fp3);

	/* Š—vŽžŠÔs—ñ‚ÌŽæ“¾ */
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<NODE_NUM;j++){
			if(matrix[i][j].distance != INF){
				matrix[i][j].time = matrix[i][j].distance / MOVE_SPEED * 60;
			}
			else{
				matrix[i][j].time = INF;
			}
		}
	}
}
#endif