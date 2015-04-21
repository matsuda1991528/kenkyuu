/*
 * header.h
 *
 *  Created on: 2015/04/07
 *      Author: sosmem
 */

#ifndef HEADER_H_
#define HEADER_H_

/* xy座標を表現する構造体 */
struct xy_coord_data_t{
	double x;   //x座標
	double y;   //y座標
};

struct node_t{
	struct xy_coord_data_t pos;
	int num;
	struct node_t *pointer;
};
/* プロトタイプ宣言 */
//struct xy_coord_data_t ConvertLonLatToXyCoord(struct lon_lat_data_t before_convert_pos);

#endif /* HEADER_H_ */
