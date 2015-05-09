#ifndef HEADER_H
#define HEADER_H

#define MAX_LEN 256
#define EMPTY -1

/* xy座標を表現する構造体 */
struct xy_coord_t{
	double x;
	double y;
};

/* 経度緯度を表現する構造体 */
struct lon_lat_data_t{
	double lon;
	double lat;
};

struct build_xy_pos_t{
	struct xy_coord_t min;
	struct xy_coord_t max;
};

struct build_lonlat_pos_t{
	struct lon_lat_data_t min;
	struct lon_lat_data_t max;
};

struct xy_coord_t ConvertLonLatToXyCoord(struct lon_lat_data_t before_convert_pos);

#endif