#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"header.h"

/* struct build_pos_t�^���̃��������m�ۂ��Cstruct build_pos_t*�@�փL���X�g���� */
static struct build_pos_t *buildMalloc(void){
	return (struct build_pos_t *)malloc(sizeof(struct build_pos_t));
}

/* 2�_�Ԃ̍��W���v�Z���� */
static double getDist(struct xy_coord_t p1, struct xy_coord_t p2){
	return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

/* 
�����̍ŏ��E�ő�xy���W�f�[�^��ǂݍ��݁C��������
�e�����ɔԍ��C���S�ʒu���W�����蓖�Ă�
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
		//���S�ʒu�̌v�Z
		p->pos.x = (min_pos.x + max_pos.x) / 2;
		p->pos.y = (min_pos.y + max_pos.y) / 2;
		p->width = getDist(min_pos, max_pos) / 2;		
		
		old->next = p;
		old = p;
	}
	fclose(fp);
	old->next = NULL;
	
	return head;
}

/* 
�������̎擾 
build_size �͌�����+�P�̒l������ifor�����l�����āj
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

/* �n�}���̌�������ő啝���Z�o */
double getMaxBuildWidth(struct build_pos_t *head){
	struct build_pos_t *p;
	double width_max = EMPTY;
	p = head;
	p = p->next;
	while(p != NULL){
		if(p->width > width_max){
			width_max = p->width;
		}
		p = p->next;
	}
	return width_max;
}

void getBuildPos(struct build_set_t* build_set){
	
	build_set->head = loadBuildPosData();
	build_set->build_size = countBuildSize(build_set->head);

	return;
}
