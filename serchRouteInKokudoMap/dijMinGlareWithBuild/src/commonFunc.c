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