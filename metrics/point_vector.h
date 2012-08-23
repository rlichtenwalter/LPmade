#ifndef _POINT_VECTOR_H
#define _POINT_VECTOR_H

#include "curve.h"

typedef struct
{
	long size;
	long capacity;
	point* array;
} point_vector;

point_vector* point_vector_create();
point_vector* point_vector_create_sized(int size);
point point_vector_pop_back(point_vector* vp);
void point_vector_push_back(point_vector* vp, point* p);
void point_vector_free(point_vector* vp);
void point_vector_print(point_vector* vp);
point point_vector_get(point_vector* vp, long index);

#endif

