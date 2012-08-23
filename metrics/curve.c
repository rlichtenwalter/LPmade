#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "curve.h"
#include "point_vector.h"

const double TOL = 1e-18;
unsigned long STACK_CAPACITY = 65536;

int points_sort (const void* e1, const void* e2)
{
	// array elements are point*
	point* p1 = *((point**)e1);
	point* p2 = *((point**)e2);
	double answerx = (p1->x - p2->x);

	if (answerx > TOL)
		return 1;
	else if (answerx < -TOL)
		return -1;
	else
		return 0;

}

int right_turn(point*, point*, point*);

void hull_add_point(point_vector* vp, point* p)
{
	if (vp->size > 1)
	{
		while (!right_turn(&(vp->array[vp->size - 2]), &(vp->array[vp->size - 1]), p))
		{
			vp->size--;
			if (vp->size ==  1)
				break;
		}
	}
	point_vector_push_back(vp, p);
}

int right_turn(point* p1, point* p2, point* p3)
{
	//printf("rt: %le, %le, %le\n", p1->x, p2->x, p3->x);
	// p1, p2, p3 specified in increasing order of X.
	double a1 = p1->x - p2->x;
	double a2 = p1->y - p2->y;
	double b1 = p3->x - p2->x;
	double b2 = p3->y - p2->y;

	return (a1 * b2 > b1 * a2);
}

point_vector* read_file(double* prop_positive, long num_negatives, long num_positives)
{
	FILE* fp;
	long true_positives;
	long false_positives;
	double p1, old_p1;
	int true_class;
	point_vector* vp;
	point p;

	old_p1 = -1;
	vp = point_vector_create();
	p.x = 0;
	p.y = 0;
	point_vector_push_back(vp, &p);
	//fp = fopen(filename, "r");
	fp = stdin;
//	if (!fp) { printf("could not open %s!\n", filename); exit(0); }
	if (num_positives == -1 && num_negatives == -1)
		fscanf(fp, "%ld %ld\n", &num_negatives, &num_positives);
	//true_positives = num_positives;
	//false_positives = num_negatives;
	true_positives = 0;
	false_positives = 0;

	while (!feof(fp))
	{
		fscanf(fp, "%lf %d\n", &p1, &true_class);
		if (!feof(fp))
		{
			if (fabs(p1 - old_p1) > TOL && old_p1 > -1)
			{
				p.y = (double)true_positives / num_positives;
				p.x = (double)false_positives / num_negatives;
				//printf("ROC: (%lf, %lf)\n", p.x, p.y);
				hull_add_point(vp, &p);
			}
			if(true_class == 0)
				false_positives++;
			else
				true_positives++;
			old_p1 = p1;
		}
	}
	
	p.y = (double)true_positives / num_positives;
	p.x = (double)false_positives / num_negatives;
	hull_add_point(vp, &p);

	fclose(fp);

	*prop_positive = (double) num_positives / (num_positives + num_negatives);
	p.x = 1;
	p.y = 1;
	hull_add_point(vp, &p);

	return vp;
}

line translate_point(point* p)
{
	line ret;
	ret.b = p->x;				// false positive rate: ROC x-coordinate.
	ret.m = (1 - p->y) - p->x;	// fnr - fpr = (1 - tpr) - fpr
	return ret;
}

int intersect(line line1, line line2, point* ret)
{

	//printf("intersecting (%f, %f), (%f, %f)\n", line1->m, line1->b, line2->m, line2->b);

	if (fabs(line1.m - line2.m) < TOL) { return 0; }

	ret->x = (line2.b - line1.b) / (line1.m - line2.m);
	ret->y = line1.m * ret->x + line1.b;

	//printf ("intersection (%f, %f)\n", ret->x, ret->y);

	return -1;
}

point** compute_envelope(point_vector* roc_hull, line** lines)
{
	point** ret = (point**)malloc((roc_hull->size + 1) * sizeof(point*));
	unsigned long x;

	for (x = 0; x < roc_hull->size + 1; x++)
		ret[x] = (point*)malloc(sizeof(point));

	for (x = 0; x < roc_hull->size; x++)
		(*lines)[x] = translate_point(&roc_hull->array[x]);
	
	for (x = 0; x < roc_hull->size - 1; x++)
		intersect((*lines)[x], (*lines)[x + 1], ret[x + 1]);

	ret[0]->x = ret[1]->x;
	ret[0]->y = 0;
	ret[roc_hull->size]->x = ret[roc_hull->size - 1]->x;
	ret[roc_hull->size]->y = 0;

	return ret;
}

void points_free(point** points, int num_points)
{
	int x;

	for (x = 0; x < num_points; x++)
		free(points[x]);
	
	free(points);
}

double euclidean(point* p1, point* p2)
{
	return sqrt((p1->y - p2->y) * (p1->y - p2->y) + (p1->x - p2->x) * (p1->x - p2->x));
}


double aacc(point** points, unsigned long numpoints)
{
	double a, b, c, s;
	double envelope_area = 0;
	double box_area, min_x, max_x, min_int, max_int;
	unsigned long x;

	min_x = points[1]->x;
	max_x = points[numpoints - 2]->x;
	min_int = 0.5 * (min_x) - ((min_x - 0.5) * fabs(min_x - 0.5) / 2);
	max_int = 0.5 * (max_x) - ((max_x - 0.5) * fabs(max_x - 0.5) / 2);
	box_area = max_int - min_int;


	for (x = 1; x < numpoints - 1; x++)
	{
		a = euclidean(points[0], points[x]);
		b = euclidean(points[x], points[x + 1]);
		c = euclidean(points[x + 1], points[0]);
		s = (a + b + c) / 2;
		envelope_area += sqrt(s * (s - a) * (s - b) * (s - c));
	}

	return 4 * (box_area - envelope_area);
}

void plot_curve(line* cost_curve, int num_lines)
{
	FILE* fp;
	int x;

	fp = fopen("curves.p", "w");
	if (!fp) { fprintf(stderr, "plot file open failed!\n"); exit(0); }

	fprintf(fp, "set term postscript color\n");
	fprintf(fp, "set output 'curves.ps'\n");
	fprintf(fp, "set xrange [0:1]\n");
	fprintf(fp, "set yrange [0:0.5]\n");
	fprintf(fp, "set nokey\n");
	fprintf(fp, "plot x");

	for(x = 0; x < num_lines; x++)
		fprintf(fp, ", %lf * x + %lf", cost_curve[x].m, cost_curve[x].b);

	fprintf(fp, "\n");
	fclose(fp);
}

int main (int argc, char** argv)
{
  line* lines;
  point_vector* roc_hull;
  unsigned long num_points;
  unsigned long num_positives = 0;
  unsigned long num_negatives = 0;
  point** points;
  double prop_positive;
  double operating_range;
  unsigned long x;

  if (argc == 3)
    {
      num_negatives = atol(argv[1]);
      num_positives = atol(argv[2]);
    }

	roc_hull = read_file(&prop_positive, num_negatives, num_positives);
	lines = (line*)malloc((roc_hull->size) * sizeof(line));
	points = compute_envelope(roc_hull, &lines);
	num_points = roc_hull->size + 1;
	operating_range = points[roc_hull->size]->x - points[0]->x;


	for (x = 0; x < num_points; x++)
		fprintf(stderr, "%0.9f %0.3f\n", points[x]->x, points[x]->y);

	printf("%lE,%lE\n", aacc(points, num_points), operating_range);
	plot_curve(lines, roc_hull->size);

	return 0;
}
