#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define NODE_NUM 42
#define MAX_EDGE 5
#define INF 999
#define NODE_TEMPORAL 10

struct xyCoord{
	int x;  //x座標
	int y;  //y座標
};
struct nodeData{
	int num;   //ノード番号
	struct xyCoord pos; //各ノードの位置情報
};	
struct mapData{
	double distance;
	double angle;
};

struct nodeData node[NODE_NUM];
struct mapData mapMatrix[NODE_NUM][NODE_NUM];

/* プロトタイプ宣言 */
double getDistance(struct xyCoord, struct xyCoord);
double getAngle(struct xyCoord, struct xyCoord);
void getNodeNum();
void getNodePos_x();
void getNodePos_y();
void getDistanceMatrix();
void getAngleMatrix();
void printNode();
void printMatrixDistance();
void printMatrixAngle();
void printByougaMapInformation();


int main(void){

/* ノード番号の取得 */
	getNodeNum();
	getNodePos_x();
	getNodePos_y();
	getDistanceMatrix();
	getAngleMatrix();
	printNode();
	printMatrixDistance();
	printMatrixAngle();
	printByougaMapInformation();
	
	return 0;
}

void getNodeNum(){
	int i;
	/* ノード番号の割り当て */
	for(i=1;i<NODE_NUM;i++){
		node[i].num = i;
	}
}

void getNodePos_x(){
	int i;	
	/* 各ノードへx座標の割り当て */
	for(i=1;i<NODE_NUM;i++){
		if(i==1){
			node[i].pos.x = 0;
		}
		else if(i>1 && i<5){
			node[i].pos.x = 10;
		}
		else if(i>4 && i<10){
			node[i].pos.x = 20;
		}
		else if(i>9 && i<17){
			node[i].pos.x = 30;
		}
		else if(i>16 && i<26){
			node[i].pos.x = 40;
		}
		else if(i>25 && i<33){
			node[i].pos.x = 50;
		}
		else if(i>32 && i<38){
			node[i].pos.x = 60;
		}
		else if(i>37 && i<41){
			node[i].pos.x = 70;
		}
		else{
			node[i].pos.x = 80;
		}
	}
	for(i=1;i<NODE_NUM;i++){
		node[i].pos.x += 20;
	}
}

void getNodePos_y(){
	int i;
	for(i=1;i<NODE_NUM;i++){
		if(i==17){
			node[i].pos.y = 40;
		}
		else if(i==10 || i==18 || i==26){
			node[i].pos.y = 30;
		}
		else if(i==5 || i==11 || i==19 || i==27 || i==33){
			node[i].pos.y = 20;
		}
		else if(i==2 || i==6 || i==12 || i==20 || i==28 || i==34 || i==38){
			node[i].pos.y = 10;
		}
		else if(i==4 || i==8 || i==14 || i==22 || i==30 || i==36 || i==40){
			node[i].pos.y = -10;
		}
		else if(i==9 || i==15 || i==23 || i==31 || i==37){
			node[i].pos.y = -20;
		}
		else if(i==16 || i==24 || i==32){
			node[i].pos.y = -30;
		}
		else if(i==25){
			node[i].pos.y = -40;
		}
		else{
			node[i].pos.y = 0;
		}
	}
	
	for(i=1;i<NODE_NUM;i++)
		node[i].pos.y += 60;
}

/* ノードと2*NODE_TEMPORAL内の距離にあるノード間に辺を生成 */
/* 距離行列の生成 */
void getDistanceMatrix(){
	int i, j;
	double nodeDistance;
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<NODE_NUM;j++){
			if(i == j){
				mapMatrix[i][j].distance = INF;
			}
			else{
				nodeDistance = getDistance(node[i].pos, node[j].pos);
				if(nodeDistance < 2*NODE_TEMPORAL){
					mapMatrix[i][j].distance = nodeDistance;
				}else{
					mapMatrix[i][j].distance = INF;
				}
			}
		}
	}				
}

double getDistance(struct xyCoord currentPos, struct xyCoord nextPos){
	double distance;
	distance = sqrt((nextPos.x - currentPos.x) * (nextPos.x - currentPos.x) + (nextPos.y - currentPos.y) * (nextPos.y - currentPos.y));
	return distance;
}

/* 辺の角度行列の作成 */
void getAngleMatrix(){
	int i, j;
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<NODE_NUM;j++){
			if(mapMatrix[i][j].distance == INF){
				mapMatrix[i][j].angle = INF;
			}
			else{
				mapMatrix[i][j].angle = getAngle(node[i].pos, node[j].pos);
			}
		}
	}
}				

double getAngle(struct xyCoord currentPos, struct xyCoord nextPos){
	double diff_x, diff_y;
	double radian, angle;
	
	diff_x = nextPos.x - currentPos.x;
	diff_y = nextPos.y - currentPos.y;
	radian = atan2(diff_y, diff_x);
	angle = (radian * 180.0 / M_PI);
	return angle;
}

void printNode(){
	char *fname = "node.txt";
	FILE *fp;
	int i;
	
	fp = fopen(fname,"wt");
	if(fp == NULL){
		printf("file open error\n");
		exit(1);
	}
	else{
		fprintf(fp, "node_num %d\n", NODE_NUM);
		fprintf(fp, "node_num x_coord y_coord\n");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%2d %3d %3d\n", i, node[i].pos.x, node[i].pos.y);
		}
	}
	fclose(fp);
}

void printMatrixDistance(){
	char *fname = "matrixDistance.txt";
	FILE *fp;
	int i, j;
	fp = fopen(fname,"wt");
	if(fp == NULL){
		printf("file open error\n");
		exit(1);
	}
	else{
		fprintf(fp, "   ");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%6d ", i);
		}
		fprintf(fp, "\n");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%2d ", i);
			for(j=1;j<NODE_NUM;j++){
				fprintf(fp, "%6.1f ", mapMatrix[i][j].distance);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

void printMatrixAngle(){
	char *fname = "matrixAngle.txt";
	FILE *fp;
	int i, j;
	fp = fopen(fname, "wt");
	if(fp == NULL){
		printf("file open error\n");
		exit(1);
	}
	else{
		fprintf(fp, "   ");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%6d ", i);
		}
		fprintf(fp, "\n");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%2d ", i);
			for(j=1;j<NODE_NUM;j++){
				fprintf(fp, "%6.1f ", mapMatrix[i][j].angle);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

void printByougaMapInformation(){
	char *fname1 = "byougaMap\\map.txt";
	char *fname2 = "byougaMap\\edge.txt";
	FILE *fp1, *fp2;
	int i, j;
	int edge_cnt = 1;
	fp1 = fopen(fname1, "wt");
	if(fp1 == NULL){
		printf("file open error\n");
		exit(1);
	}
	else{
		fprintf(fp1, "nodeNum %d\n", NODE_NUM);
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp1, "%d, %d\n", node[i].pos.x, node[i].pos.y);
		}
	}
	fclose(fp1);
	
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<NODE_NUM;j++){
			if(mapMatrix[i][j].distance != INF){
				edge_cnt++;
			}
		}
	}
	
	fp2 = fopen(fname2, "wt");
	if(fp2 == NULL){
		printf("file open error\n");
		exit(1);
	}
	else{
		fprintf(fp2, "edge %d\n", edge_cnt);
		for(i=1;i<NODE_NUM;i++){
			for(j=1;j<NODE_NUM;j++){
				if(mapMatrix[i][j].distance != INF){
					fprintf(fp2, "%3d, %3d, %3d, %3d\n", node[i].pos.x, node[i].pos.y, node[j].pos.x, node[j].pos.y);
				}
			}
		}
	}
	fclose(fp2);
}