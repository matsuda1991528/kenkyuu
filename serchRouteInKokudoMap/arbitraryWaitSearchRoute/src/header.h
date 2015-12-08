#ifndef HEADER_H
#define HEADER_H

/* xy���W��\������\���� */
typedef struct xy_coord_t{
	double x;  //x���W
	double y;  //y���W
}xy_coord_t;

/* ������\���\���� */
typedef struct tim_t{
	int hour;
	int min;
	int sec;
}tim_t;

/* ���z�p�x��\���\���� */
typedef struct sun_angle_t{
	double elev;   //���x
	double azim;  //����
}sun_angle_t;

/**
**********************
*   �����_�֌W�̃f�[�^     *
**********************
**/
/* ���ԋ�Ԃ�\���\���� */
typedef struct time_space_t{
	tim_t bgn_tim; //���ԋ��S�֑J�ڏo����ł���������
	tim_t end_tim; //���ԋ��S�֑J�ڏo����ł��x������
	double cost;
}time_space_t;


/* �אڃ��X�g��\���\���� */
typedef struct adj_list_t{
	int num;
	time_space_t *tim_spc; //���Ԉˑ������O���A���
	struct adj_list_t *next;
}adj_list_t;


/* ���_���X�g��\������\���� */
typedef struct vertex_t{
	int num; //�C���f�b�N�X�ԍ�
	xy_coord_t pos; //2�������[�N���b�h��Ԃɂ�����ʒu���W
	adj_list_t *ptr;
	adj_list_t *head;
	adj_list_t *old;
}vertex_t;

typedef struct vertex_set_t{
	int sz; //���_�̑���
	vertex_t *indx; //�n�}��̌����_�̑����i�z��̐錾�Œ�`���ׂ��T�C�Y�j
}vertex_set_t;

/**
**********************
*     �����֌W�̃f�[�^      *
**********************
**/

/* ������\���\���� */
typedef struct build_pos_t{
	int num;       //�����ԍ�
	double wdth; //������
	xy_coord_t pos; //�����̒��S�ʒu
	struct build_pos_t *next;
}build_pos_t;

typedef struct build_set_t{
	build_pos_t *head; //�擪���X�g�̓_�~�[
	build_pos_t *ptr;
	build_pos_t *old;
	int sz; //�n�}��̌����̑����i�z��̐錾�Œ�`���ׂ��T�C�Y�j
}build_set_t;

typedef struct grid_size_t{
	int sn; //��k�����ɂ�����O���b�h�T�C�Y
	int ew; //���������ɂ�����O���b�h�T�C�Y
}grid_size_t;

/* �����f�[�^�O���b�h�}�b�v��\���\���� */
typedef struct build_grid_t{
	xy_coord_t bgn;  //�O���b�h�n�}�̎n�[(�ŏ�)xy���W
	xy_coord_t end; //�O���b�h�n�}�̏I�[(�ő�)xy���W
	build_pos_t *head;  //�������X�g  //�擪���X�g�̓_�~�[
	build_pos_t *ptr;
	build_pos_t *old;
}build_grid_t;

void setInptFilesData(vertex_set_t *vrtx_st, build_set_t* bld_st);
//void cretGrdMap(build_grid_t **bld_grd, build_set_t bld_st, xy_coord_t grd_sz);
build_grid_t **cretGrdMap(build_set_t bld_st, xy_coord_t grd_len);

#endif
