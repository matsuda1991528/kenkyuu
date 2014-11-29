#include<stdio.h>
#include"extern.h"
#include"ga.h"


int main(void){
	int cnt;
	eliteIndividual.uncomValue = INF;
	
	getMapData();
	printf("aftergetMap\n");
	getGene();
	printf("afterGetGene\n");
	printGene();
	valuation();
	printf("afterGetValuation\n");
	printGene();
	eliteSave();
	for(cnt=0;cnt<GENERATION;cnt++){
		selection();
		printf("afterselection\n");
		insertElite();
		crossover();
		printf("aftercrossover\n");
		mutation();
		printf("aftermutation\n");
		waitTimeMutation();
		printf("afterwaitTimeMutation\n");
		valuation();
		printf("aftervaluation\n");
		eliteSave();
	}
	printElite();
	
	return 0;
}

	