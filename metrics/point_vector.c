#include <stdlib.h>
#include <stdio.h>

#include "curve.h"
#include "point_vector.h"

#define VECTOR_CAPACITY 16384

point_vector* point_vector_create()
{
	point_vector* vp = (point_vector*)malloc(sizeof(point_vector));
	vp->array = (point*)malloc(VECTOR_CAPACITY * sizeof(point));
	vp->capacity = VECTOR_CAPACITY;
	vp->size = 0;

	return vp;
}

point_vector* point_vector_create_sized(int size)
{
	point_vector* vp = (point_vector*)malloc(sizeof(point_vector));
	vp->array = (point*)malloc(size * sizeof(point));
	vp->capacity = size;
	vp->size = size;

	return vp;
}

point point_vector_pop_back(point_vector* vp)
{
	vp->size--;
	return vp->array[vp->size - 1];
}

void point_vector_push_back(point_vector* vp, point* p)
{
	if (vp->size == vp->capacity)
	{
		vp->capacity *= 2;
		vp->array = (point*)realloc(vp->array, vp->capacity * sizeof(point));
	}
	vp->array[vp->size].x = p->x;
	vp->array[vp->size].y = p->y;
	vp->size++;
}

void point_vector_free(point_vector* vp)
{
	free(vp->array);
	free(vp);
}

void point_vector_print(point_vector* vp)
{
	int x;
	double slope = 1.0 / 0.0;
	printf("[");
	for (x = 0; x < vp->size; x++)
	{
		if (x > 0)
			slope = (vp->array[x].y - vp->array[x - 1].y) / (vp->array[x].x - vp->array[x - 1].x);
		printf("(%0.7lf, %0.7lf, %0.7lf); ", vp->array[x].x, vp->array[x].y, slope);
	}
	printf("]\n");
}

point point_vector_get(point_vector* vp, long index)
{

#ifdef RANGE_CHECK
	if (index < 0 || index >= vp->size)
		fprintf(stderr, "evil index: %ld of %ld\n", index, vp->size);
#endif
		return vp->array[index];
}
