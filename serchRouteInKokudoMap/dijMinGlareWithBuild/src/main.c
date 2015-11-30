#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"header.h"

#define MAX_LEN 256

void readArgm(int argc, char **argv, struct include_command_data_t *input){

	while(--argc > 0 && **(++argv) == '-'){
		argc--;
		if(strcmp(*argv, "-start") == 0){
			input->start_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-goal") == 0){
			input->goal_vertex = atoi(*++argv);
		}
		else if(strcmp(*argv, "-dptr_h") == 0){
			input->dptr.hour = atoi(*++argv);
		}
		else if(strcmp(*argv, "-dptr_m") == 0){
			input->dptr.min = atoi(*++argv);
		}
		else if(strcmp(*argv, "-kph") == 0){
			input->vel_kph = atoi(*++argv);
		}
		else if(strcmp(*argv, "-gx") == 0){
			g_size.x = atof(*++argv);
		}
		else if(strcmp(*argv, "-gy") == 0){
			g_size.y = atof(*++argv);
		}
		else if(strcmp(*argv, "-lam") == 0){
			lambda = atof(*++argv);
		}
		else if(strcmp(*argv, "-mode")==0){
			input->route_mode = atoi(*++argv);
		}else{
			printf("invailed command\n");
			exit(1);
		}
	}
	return;
}

int main(int argc, char **argv){
	struct include_command_data_t input;
	initIncludeCommand(&input);
	readArgm(argc, argv, &input);
	
	test_build_count = 0;
	test_grid_count = 0;
	test_edge_count = 0;
	test_overlap_build_counter = 0;
	
	/* 実行時コマンドを変数に格納 */
	checkIncludeCommand(input);
	struct vertex_t *vertex;  //頂点データ
	int node_size; //頂点データサイズ
	struct build_set_t build_set; //建物データ
	struct grid_size_t grid_size; //グリッドの東西方向と南北方向におけるセル数
	struct build_grid_t **build_grid;
	int loop_state = FALSE;
	build_grid = NULL;
	clock_t start, end;
	start = end = 0;
	
	//start = clock();
	/* ノードサイズの取得 */
	node_size = loadNodeSize();
	checkStartAndGoalNum(input.start_vertex, input.goal_vertex, node_size);
	
	/* 隣接リストの記憶領域の取得 */	
	vertex = (struct vertex_t *)malloc(sizeof(struct vertex_t) * node_size);
	if(vertex == NULL){
		printf("cannnot malloc node list memory\n");
		exit(1);
	}

	/* リストの初期化 */
	initVertex(vertex, node_size);
	//頂点データの取得
	loadNodeData(vertex, node_size);
	//建物データの取得
	getBuildPos(&build_set);
	//建物の最大幅の取得
	build_width = getMaxBuildWidth(build_set.head);		
	
/* 不快度最小経路を探索する場合 */
	if(input.route_mode == 0){
		/* エリートデータの初期化 */
		initElite(node_size);
		//建物データグリッドマップの生成
		build_grid = createGridMap(build_set.head, build_grid, &grid_size);
		start = clock();
		shotestPathTravTime = serchRouteDistDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.vel_kph);

		while(loop_state == FALSE){
			loop_state = serchRouteGridDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_grid, grid_size);
		}
		end = clock();
	}
	
/* 最短経路を探索する場合 */
	else if(input.route_mode == 1){
		//start = clock();
		build_grid = createGridMap(build_set.head, build_grid, &grid_size);
		serchRouteGridDistDijkstra(vertex, node_size, input.start_vertex, input.goal_vertex, input.dptr, input.vel_kph, build_grid, grid_size);
	}			
		//end = clock();
	else{
		printf("invailed route mode  -mode -> 0 minimize uncom route  -mode -> 1 minimize dist route\n");
	}
	
	free(vertex);

	//printf("%d, %d, %d, %d, %f, %d\n", (int)g_size.x, (int)g_size.y, test_grid_count, test_build_count, (double)(end - start) / 1000.0f, test_overlap_build_counter);
/* 計算時間の算出 */	
/*
	if(g_size.y == 1000)
		printf("%lf\n", (double)(end - start) / 1000);
	else
		printf("%lf, ", (double)(end - start) / 1000);
*/
		
	//if(g_size.y == 2000)
	//	printf("%d\n", test_build_count);
	//else
	//	printf("%d,", test_build_count);
		
	//if(g_size.y == 2000)
	//	printf("%d\n", test_grid_count);
	//else
	//	printf("%d,", test_grid_count);
	
	//printf("%d\n", test_edge_count);
	return 0;
}