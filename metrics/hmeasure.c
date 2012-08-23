#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "curve.h"
#include "point_vector.h"

const int INTEGRAL_STEPS = 100000;
const double TOL = 1e-18;

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

void point_swap(point* p1, point* p2)
{
	point temp = *p1;
	*p1 = *p2;
	*p2 = temp;
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

double betapdf(double x, double a, double b)
{
	return powl(x, (a - 1)) * powl(1 - x, b - 1);
}
 
double ibeta(double c, double a, double b)
{
	double ret = 0;
	int x;
	
	for (x = 1; x <= INTEGRAL_STEPS; x++)
	{
		double xval = c / INTEGRAL_STEPS * (x - 0.5);
		ret += betapdf(xval, a, b)  * (c / INTEGRAL_STEPS);
	}
  
  return ret;
}
    
    
double betafunc(double c, double a, double b, double beta_constant)
{
  return ibeta(c, a, b) / beta_constant;
}

// returns ROC covex hull
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

double h_measure(point_vector* convex_hull, double prop_positive, double alpha, double beta)
{
	long size = convex_hull->size;
	double prop_negative = 1 - prop_positive;
	double beta_constant = ibeta(1.0, alpha, beta);
	double beta0 = betafunc(0, alpha + 1, beta, beta_constant); // reversed (see bottom)
	double beta1 = betafunc(0, alpha, beta + 1, beta_constant); // reversed (see bottom)
	double hull_dx = convex_hull->array[1].x - convex_hull->array[0].x; // reversed
	double hull_dy = convex_hull->array[1].y - convex_hull->array[0].y; // reversed
	double cost = prop_positive * hull_dy / (prop_positive * hull_dy + prop_negative * hull_dx);
	double beta0_1 = betafunc(cost, alpha + 1, beta, beta_constant);
	double beta1_1 = betafunc(cost, alpha, beta + 1, beta_constant);
	double lhat = 0.0;
	long x;

	lhat += prop_negative * (1 - convex_hull->array[0].x) * (beta0_1 - beta0); //reversed
	lhat += prop_positive * convex_hull->array[0].y       * (beta1_1 - beta1); //reversed
	/*printf("LHat: %0.20lf\n", lhat);
	printf("lhs:  %0.20lf\n", (beta0_1 - beta0));
	printf("rhs:  %0.20lf\n", (beta1_1 - beta1));*/
	
	beta0 = beta0_1;
	beta1 = beta1_1;

	for (x = 1; x < size - 1; x++)
	{
		hull_dx = convex_hull->array[x + 1].x - convex_hull->array[x].x;
		hull_dy = convex_hull->array[x + 1].y - convex_hull->array[x].y; // reversed
		cost = prop_positive * hull_dy / (prop_positive * hull_dy + prop_negative * hull_dx);
		beta0_1 = betafunc(cost, alpha + 1, beta, beta_constant);
		beta1_1 = betafunc(cost, alpha, beta + 1, beta_constant);
		lhat += prop_negative * (1 - convex_hull->array[x].x) * (beta0_1 - beta0); //reversed
		lhat += prop_positive * convex_hull->array[x].y       * (beta1_1 - beta1); //reversed
		beta0 = beta0_1;
		beta1 = beta1_1;
	}

	beta0_1 = betafunc(1, alpha + 1, beta, beta_constant); // reversed (see top)
	beta1_1 = betafunc(1, alpha, beta + 1, beta_constant); // reversed (see top)
	lhat += prop_negative * (1 - convex_hull->array[x].x) * (beta0_1 - beta0); // reversed
	lhat += prop_positive * convex_hull->array[x].y       * (beta1_1 - beta1); // reversed
	

	beta0 = betafunc(prop_positive, 1 + alpha, beta, beta_constant);
	beta1 = betafunc(1, alpha, 1 + beta, beta_constant);
	beta1 -= betafunc(prop_positive, alpha, 1 + beta, beta_constant);

	return 1 - lhat / (prop_negative * beta0 + prop_positive * beta1);
}

int main (int argc, char** argv)
{
	double prop_positive;
	long num_negatives = -1;
	long num_positives = -1;
	point_vector* convex_hull;
	double hmeasure;

	if (argc == 3)
	{
		num_negatives = atol(argv[1]);
		num_positives = atol(argv[2]);
	} else {
		fprintf( stderr, "required arguments: neg_count pos_count\n" );
		return 1;
	}

	convex_hull = read_file(&prop_positive, num_negatives, num_positives);
	hmeasure = h_measure(convex_hull, prop_positive, 2.0, 2.0);

	printf("%lE\n", hmeasure);
	return 0;
}
