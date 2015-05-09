#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"

void fileOpenCheck(FILE **fp, char *filename, char *mode){
	if((*fp = fopen(filename, mode)) == NULL){
		printf("%s is failed open", filename);
		exit(1);
	}
	return;
}


void initBuildPos(struct build_lonlat_pos_t* build){
	build->min.lon = build->min.lat = build->max.lon = build->max.lat = EMPTY;
	return;
}

/* 引数で受け取った値の内，小さい値の方を返す */
double getMinVal(double min_val, double target_val){
	if(min_val < target_val)
		return min_val;
	else
		return target_val;
}

/* 引数で受け取った値の内，大きい値の方を返す */
double getMaxVal(double max_val, double target_val){
	if(max_val > target_val)
		return max_val;
	else
		return target_val;
}

void setBuildPos(int cnt, struct lon_lat_data_t pos, struct build_lonlat_pos_t* build){
	if(cnt == 1){
		build->min.lon = build->max.lon = pos.lon;
		build->min.lat = build->max.lat = pos.lat;
	}
	else{
		build->min.lon = getMinVal(build->min.lon, pos.lon);
		build->max.lon = getMaxVal(build->max.lon, pos.lon);
		build->min.lat = getMinVal(build->min.lat, pos.lat);
		build->max.lat = getMaxVal(build->max.lat, pos.lat);
	}
	return;
}
	

int main(int argc, char *argv[]){
	FILE *read_fp, *write_fp;
	char *read_fname, *write_fname;
	char buf[MAX_LEN];
	struct lon_lat_data_t lonlat_pos;
	//struct xy_coord_t xy_pos;
	struct build_lonlat_pos_t lonlat_build;
	struct build_xy_pos_t xy_build;
	int current_node_judge_counter, before_node_judge_counter, node_counter, build_num;
	
	current_node_judge_counter = before_node_judge_counter = node_counter = build_num = 0;
	
	if(argc != 3){
		printf("invailed command argument\n");
		printf("%d\n", argc);
		exit(1);
	}
	read_fname = argv[1];
	write_fname = argv[2];
	

	fileOpenCheck(&read_fp, read_fname, "r");
	fileOpenCheck(&write_fp, write_fname, "wt");
	
	while(fgets(buf, sizeof(buf), read_fp)){
		/* 空行を読み込んだ場合はcurrent_node_judge_counter <- 0 */
		if(*buf == '\n' || *buf == '\r'){
			current_node_judge_counter = 0;
		}
		/* 空行でない場合 */
		else{
			current_node_judge_counter++;
			if(current_node_judge_counter == 1){
				if(build_num != 0){
					xy_build.min = ConvertLonLatToXyCoord(lonlat_build.min);
					xy_build.max = ConvertLonLatToXyCoord(lonlat_build.max);
					fprintf(write_fp, "%6d %lf %lf %lf %lf\n", build_num, xy_build.min.x, xy_build.min.y, xy_build.max.x, xy_build.max.y);
					build_num++;
					initBuildPos(&lonlat_build);
				}
				else{
					build_num++;
					initBuildPos(&lonlat_build);
				}
			}
			sscanf(buf, "%lf %lf", &lonlat_pos.lon, &lonlat_pos.lat);
			setBuildPos(current_node_judge_counter, lonlat_pos, &lonlat_build);
		}
	}
	fprintf(write_fp, "%5d %lf %lf %lf %lf\n", build_num, xy_build.min.x, xy_build.min.y, xy_build.max.x, xy_build.max.y);
	
	return 0;
}