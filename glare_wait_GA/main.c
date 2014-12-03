#include<stdio.h>
#include"extern.h"
#include"ga.h"

int main(void){
	int cnt;
	eliteIndividual.uncomValue = INF;
	
	getMapData();
	getGene();
//	printGene();
	valuation();
	eliteSave();
	for(cnt=0;cnt<GENERATION;cnt++){
		selection();
		insertElite();
		crossover();
		mutation();
		waitTimeMutation();
		valuation();
		eliteSave();
	}
	printElite();
	return 0;
}