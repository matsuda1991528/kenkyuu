#include<stdio.h>
#include"extern.h"
#include"ga.h"


int main(void){
	int cnt;
	eliteIndividual.uncomValue = INF;
	
	getMapData();
	//printf("aftergetMap\n");
	getGene();
	//printf("afterGetGene\n");
	//printGene();
	setUncomEstimateData();
	//printf("aftersetUncomEstimate\n");
	valuation();
	//printf("afterGetValuation\n");
	//printGene();
	eliteSave();
	//printf("after eliteSave\n");
	//printGene();
	for(cnt=0;cnt<GENERATION;cnt++){
		selection();
		//printf("afterselection\n");
		//printGene();
		insertElite();
		//printf("beforecrossover\n");
		//printGene();
		crossover();
		//printf("aftercrossover\n");
		//printGene();
		//printf("aftercrossover\n");
		//printGene();
		mutation();
		//specificWesteringSunMutation();
		//printf("aftermutation\n");
		waitTimeMutation();
		//printf("afterwaitTimeMutation\n");
		valuation();
		//printf("aftervaluation\n");
		//printGene();
		eliteSave();
	}
	printElite();
	
	return 0;
}

	