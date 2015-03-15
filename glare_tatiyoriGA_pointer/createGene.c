#ifndef Create_Gene_C
#define Create_Gene_C
#include<stdio.h>
#include<stdlib.h>
#include"header.h"


/* ��`�q�̗l�q��\������֐� */
void print_gene(struct ga_population_t ga_population){
	int i, j;
	printf("*****print population*****\n");
	for(i=0;i<IND_NUM;i++){
		for(j=1;j<ga_population.gene_length;j++){
			printf("|%3d", ga_population.individual[i].gene[j]);
		}
		printf("|\n");
		printf("fitness = %f\n\n", ga_population.individual[i].fitness);
	}
	printf("*****print population*****\n");
}

/* ��`�q���𒲂ׂ� */
int setGeneLength(struct drop_node_t *drop_node){
	int gene_size = 1;
	while(drop_node != NULL){
		drop_node = drop_node->next;
		gene_size++;
	}
	return gene_size;
}

/* ��`�q�����̃��������m�� */
struct ga_population_t gene_malloc(struct ga_population_t ga_population){
	int i;
	for(i=0;i<IND_NUM;i++){
		ga_population.individual[i].gene = (int *)malloc(sizeof(int) * ga_population.gene_length);
	}
	ga_population.elite_individual.gene = (int *)malloc(sizeof(int) * ga_population.gene_length);
	return ga_population;
}

void copyCheckRepeatDropNode(struct drop_node_t *drop_node, struct check_repeat_drop_t *check_repeat_drop){
	int cnt = 1;
	while(drop_node != NULL){
		check_repeat_drop[cnt].num = drop_node->num;
		drop_node = drop_node->next;
		cnt++;
	}
	return;
}

/* 
�d��������`�q���܂ނ��̔���
if �d�����Ă��遨�d���n�_�Əd�������������̂̈�`�q��\�����C�����I��
otherwise ���������Ȃ�
*/
void checkRepeatGene(struct ga_population_t ga_population, struct check_repeat_drop_t *check_repeat_drop){
	int i, j, k, l;
	
	for(i=0;i<IND_NUM;i++){
		for(j=1;j<ga_population.gene_length;j++){
			check_repeat_drop[j].flag = 0;
		}
		for(j=1;j<ga_population.gene_length;j++){
			for(k=1;k<ga_population.gene_length;k++){
				if(ga_population.individual[i].gene[j] == check_repeat_drop[k].num){
					check_repeat_drop[k].flag++;
					if(check_repeat_drop[k].flag > 1){
						printf("ga_population.individual[%d].gene[%d] has repeat gene!!! repeat point is %d\n", i, j, check_repeat_drop[k].num);
						printf("individual[%d]>>\n", i);
						for(l=1;l<ga_population.gene_length;l++){
							printf("|%3d", ga_population.individual[i].gene[l]);
						}
						printf("|\n");
						exit(1);
					}
				}
			}
		}
	}
	return;
}

/* �o�H��������`�q�̐��� */
void getGene(struct ga_population_t ga_population, struct check_repeat_drop_t *check_repeat_drop){
	int i, j;
	int check_repeat_drop_cp[ga_population.gene_length], rand_num;
	
	for(i=0;i<IND_NUM;i++){
		for(j=1;j<ga_population.gene_length;j++){
			check_repeat_drop_cp[j] = check_repeat_drop[j].num;
		}
		for(j=2;j<ga_population.gene_length-1;j++){
			rand_num = random()%(ga_population.gene_length-1-j)+2;
			ga_population.individual[i].gene[j] = check_repeat_drop_cp[rand_num];
			if(ga_population.gene_length - j >= 0){
				check_repeat_drop_cp[rand_num] = check_repeat_drop_cp[ga_population.gene_length-j];
			}
		}
		ga_population.individual[i].gene[1] = START;
		ga_population.individual[i].gene[ga_population.gene_length-1] = GOAL;
	}
	checkRepeatGene(ga_population, check_repeat_drop);
	return;
}

struct ga_population_t getPopulation(struct ga_population_t ga_population, struct drop_node_t *drop_node_head){
	int i;
	/* �������̈�̊m�� */
	struct check_repeat_drop_t *check_repeat_drop = (struct check_repeat_drop_t *)malloc(sizeof(struct check_repeat_drop_t) * ga_population.gene_length);
	copyCheckRepeatDropNode(drop_node_head, check_repeat_drop);
	getGene(ga_population, check_repeat_drop);
	/* �֐������Ă��Ȃ��̂́C�R���p�C���Ɍx������邽�� */
	for(i=0;i<IND_NUM;i++){
		ga_population.individual[i].wait_time = random()%MAX_WAIT_TIME;
	}
	return ga_population;
}
	
#endif