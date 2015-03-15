#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define NODE_NUM 1001
#define INF 9999
#define FIELD_SIZE_X 1000
#define FIELD_SIZE_Y 1000
#define NODE_TEMPORAL 50
#define DROP_NUM 11

struct xy_coord_t{
	int x;
	int y;
};

struct node_data_t{
	int num;
	struct xy_coord_t pos;
};

struct matrix_data_t{
	double matrix[NODE_NUM][NODE_NUM];
};


struct node_data_t *getNodeData(struct node_data_t *node){
	int i;
	for(i=1;i<NODE_NUM;i++){
		node[i].num = i;
		node[i].pos.x = random()%FIELD_SIZE_X;
		node[i].pos.y = random()%FIELD_SIZE_Y;
	}
	return node;
}

double getDistance(struct xy_coord_t current_pos, struct xy_coord_t next_pos){
	return sqrt((next_pos.x - current_pos.x) * (next_pos.x - current_pos.x) + (next_pos.y - current_pos.y) * (next_pos.y - current_pos.y));
}

struct matrix_data_t getMatrixDistance(struct node_data_t *node){
	struct matrix_data_t distance;
	int i, j;
	double dist;
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<NODE_NUM;j++){
			if(i==j){
				distance.matrix[i][j] = INF;
			}
			else{
				dist = getDistance(node[i].pos, node[j].pos);
				if(dist < NODE_TEMPORAL){
					distance.matrix[i][j] = dist;
				}
				else{
					distance.matrix[i][j] = INF;
				}
			}
		}
	}
	return distance;
}


double getAngle(struct xy_coord_t current_pos, struct xy_coord_t next_pos){
	double radian = atan2(next_pos.x - current_pos.x, next_pos.y - current_pos.y);
	return (radian * 180.0 / M_PI);
}

struct matrix_data_t getMatrixAngle(struct node_data_t *node, struct matrix_data_t distance){
	struct matrix_data_t angle;
	int i, j;
	for(i=1;i<NODE_NUM;i++){
		for(j=1;j<NODE_NUM;j++){
			if(distance.matrix[i][j] == INF){
				angle.matrix[i][j] = INF;
			}
			else{
				angle.matrix[i][j] = getAngle(node[i].pos, node[j].pos);
			}
		}
	}
	return angle;
}

void printNode(struct node_data_t *node){
	char *fname = "node.csv";
	FILE *fp;
	int i;
	
	fp = fopen(fname, "w");
	if(fp == NULL){
		printf("%s open error\n", fname);
		exit(1);
	}
	else{
		fprintf(fp, "node_num, x, y\n");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%d,%d,%d\n", node[i].num, node[i].pos.x, node[i].pos.y);
		}
	}
	fclose(fp);
}

void printDistanceMatrix(struct matrix_data_t distance, struct node_data_t *node){
	char *fname = "matrixDistance.csv";
	FILE *fp;
	int i, j;
	fp = fopen(fname, "w");
	if(fp == NULL){
		printf("%s open error\n", fname);
		exit(1);
	}
	else{
		fprintf(fp, "  ,");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%d,", node[i].num);
		}
		fprintf(fp, "\n");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp,"%d,", node[i].num);
			for(j=1;j<NODE_NUM;j++){
				fprintf(fp, "%6.1f,", distance.matrix[i][j]);
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);
}

void printAngleMatrix(struct matrix_data_t angle, struct node_data_t *node){
	char *fname = "matrixAngle.csv";
	FILE *fp;
	int i, j;
	fp = fopen(fname, "w");
	if(fp == NULL){
		printf("%s open error\n", fname);
		exit(1);
	}
	else{
		fprintf(fp, "  ,");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp, "%d,", node[i].num);
		}
		fprintf(fp, "\n");
		for(i=1;i<NODE_NUM;i++){
			fprintf(fp,"%d,", node[i].num);
			for(j=1;j<NODE_NUM;j++){
				fprintf(fp, "%6.1f,", angle.matrix[i][j]);
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);
}

void printByougaMapInformation(struct matrix_data_t distance, struct node_data_t *node){
	char *fname1 = "byougaMap\\map.txt";
	char *fname2 = "byougaMap\\edge.txt";
	FILE *fp1, *fp2;
	int i, j;
	int edge_cnt = 1;
	
	fp1 = fopen(fname1, "wt");
	if(fp1 == NULL){
		printf("%s open error\n", fname1);
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
			if(distance.matrix[i][j] != INF)
				edge_cnt++;
		}
	}
	
	fp2 = fopen(fname2, "wt");
	if(fp2 == NULL){
		printf("%s is open error\n", fname2);
		exit(1);
	}
	else{
		fprintf(fp2, "edge %d\n", edge_cnt);
		for(i=1;i<NODE_NUM;i++){
			for(j=1;j<NODE_NUM;j++){
				if(distance.matrix[i][j] != INF){
					fprintf(fp2, "%3d, %3d, %3d, %3d\n", node[i].pos.x, node[i].pos.y, node[j].pos.x, node[j].pos.y);
				}
			}
		}
	}
	fclose(fp2);
}
	
void printVisitPos(struct node_data_t *node){
	char *fname = "visitPos10.csv";
	FILE *fp;
	int i, num;
	int flag[NODE_NUM];
	int drop_node[DROP_NUM];
	for(i=1;i<NODE_NUM;i++)
		flag[i] = 0;
	
	fp = fopen(fname, "w");
	if(fp == NULL){
		printf("%s open error\n", fname);
		exit(1);
	}
	else{
		for(i=1;i<NODE_NUM;i++){
			if(node[i].pos.x < 10){
				drop_node[1] = node[i].num;
				flag[i] = 1;
			}
			else if(node[i].pos.x > FIELD_SIZE_X-10){
				drop_node[DROP_NUM-1] = node[i].num;
				flag[i] = 1;
			}
		}
		for(i=2;i<DROP_NUM-1;i++){
			while(1){
				num = random()%(NODE_NUM-1)+1;
				if(flag[num] != 1)
					break;
			}
			drop_node[i] = node[num].num;
			flag[num] = 1;
		}
		for(i=1;i<DROP_NUM;i++){
			fprintf(fp, "drop%d, %d\n", i, drop_node[i]);
		}
	}
	fclose(fp);
}
			

int main(void){
	printf("c\n");
	struct node_data_t *node = (struct node_data_t *)malloc(sizeof(struct node_data_t) * NODE_NUM);
	struct matrix_data_t distance;
	struct matrix_data_t angle;
	printf("b\n");
	node = getNodeData(node);
	distance = getMatrixDistance(node);
	angle = getMatrixAngle(node, distance);
	printf("a\n");
	printNode(node);
	printDistanceMatrix(distance, node);
	printAngleMatrix(angle, node);
	printByougaMapInformation(distance, node);
	printVisitPos(node);
	return 0;
}