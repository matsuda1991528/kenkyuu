#include<stdio.h>
#include<stdlib.h>
#include"header.h"

/* struct build_pos_t型分のメモリを確保し，struct build_pos_t*　へキャストする */
static struct build_pos_t *buildMalloc(void){
	return (struct build_pos_t *)malloc(sizeof(struct build_pos_t));
}

/* 
建物の最小・最大xy座標データを読み込み，それを基に
各建物に番号，中心位置座標を割り当てる
 */
static struct build_pos_t *loadBuildPosData(void){
	FILE *fp;
	char *fname = "..\\build_pos.txt";
	struct build_pos_t *head, *p, *old;
	struct xy_coord_t min_pos, max_pos;
	head = buildMalloc();
	
	fileOpenCheck(&fp, fname, "r");
	old = head;
	while(p = buildMalloc(), fscanf(fp, "%d", &p->num) != EOF){
		fscanf(fp, "%lf %lf %lf %lf", &min_pos.x, &min_pos.y, &max_pos.x, &max_pos.y);
		//中心位置の計算
		p->pos.x = (min_pos.x + max_pos.x) / 2;
		p->pos.y = (min_pos.y + max_pos.y) / 2;
		//printf("%6d %f %f\n", p->num, p->pos.x, p->pos.y);
		
		old->next = p;
		old = p;
	}
	fclose(fp);
	old->next = NULL;
	
	return head;
}

/* 
建物数の取得 
build_size は建物数+１の値が入る（for等を考慮して）
*/
static int countBuildSize(struct build_pos_t *head){
	struct build_pos_t *p;
	int build_size = 0;
	
	p = head;
	while(p != NULL){
		p = p->next;
		build_size++;
	}
	return build_size;
}
	
	


void getBuildPos(struct build_set_t* build_set){
	//struct build_pos_t *head;
	//int build_size;
	
	build_set->head = loadBuildPosData();
	build_set->build_size = countBuildSize(build_set->head);
	//printf("%d\n", build_size);
	
	return;
}
	 