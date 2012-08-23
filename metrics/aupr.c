/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

double auc(double step_size, unsigned int p_count) {
	double prob_val;
	unsigned int class_val;
	double area = 0.0;
	unsigned long last_tp_count = 0;
	unsigned long last_fp_count = 0;
	unsigned long tp_count = 0;
	unsigned long fp_count = 0;
	double last_prob_val = 1.0;
	int result = 0;
	while( (result = fscanf( stdin, "%lg %u\n", &prob_val, &class_val )) != EOF ) {
		if( class_val != 1 ) {
			fp_count++;
		} else {
			tp_count++;
		}
		if( prob_val < last_prob_val ) {
			unsigned long interp_count = (unsigned long)(((tp_count - last_tp_count) / step_size) );
			if( interp_count > 0 ) {
//				fprintf( stderr, "prob_val: %.3lf - tp_count: %lu - fp_count: %lu - interp_count: %lu\n", prob_val, tp_count, fp_count, interp_count );
				double tp_delta = (tp_count - last_tp_count) / (double)interp_count;
				double fp_delta = (fp_count - last_fp_count) / (double)interp_count;
	
				double lower_tp = last_tp_count;
				double lower_fp = last_fp_count;
				if( lower_tp == 0 ) {
					lower_tp = 0.0000000000000001;
				}
				while( lower_tp < tp_count ) {
					double upper_tp = lower_tp + tp_delta;
					double upper_fp = lower_fp + fp_delta;
					
					double lower_recall = lower_tp / p_count;
					double lower_precision = lower_tp / (lower_tp+lower_fp);
//					fprintf( stderr, "prob_val: %.3lf - class_val: %u - tp: %.3lf - fp: %.3lf - r: %.3lf - p: %.3lf\n", prob_val, class_val, lower_tp, lower_fp, lower_recall, lower_precision );
					double upper_recall = upper_tp / p_count;
					double upper_precision = upper_tp / (upper_tp+upper_fp);
					double rectangle_area = (upper_recall-lower_recall)*lower_precision;
					double triangle_area = 0.5*(upper_recall-lower_recall)*(upper_precision-lower_precision);
//					fprintf( stderr, "Rectangle Area: %.3lf - Triangle Area: %.3lf\n", rectangle_area, triangle_area );
					area += rectangle_area + triangle_area;
					
					lower_tp = upper_tp;
					lower_fp = upper_fp;
				}
			}
			last_tp_count = tp_count;
			last_fp_count = fp_count;
		}
		last_prob_val = prob_val;
	}
//	fprintf( stdout, "False Positives: %lu - True Positives: %lu\n", fp_count, tp_count );
	unsigned long interp_count = (unsigned long)(((tp_count - last_tp_count) / step_size) );
	if( interp_count > 0 ) {
//		fprintf( stderr, "prob_val: %.3lf - tp_count: %lu - fp_count: %lu - interp_count: %lu\n", prob_val, tp_count, fp_count, interp_count );
		double tp_delta = (tp_count - last_tp_count) / (double)interp_count;
		double fp_delta = (fp_count - last_fp_count) / (double)interp_count;
	
		double lower_tp = last_tp_count;
		double lower_fp = last_fp_count;
		if( lower_tp == 0 ) {
			lower_tp = 0.0000000000000001;
		}
		while( lower_tp < tp_count ) {
			double upper_tp = lower_tp + tp_delta;
			double upper_fp = lower_fp + fp_delta;
					
			double lower_recall = lower_tp / p_count;
			double lower_precision = lower_tp / (lower_tp+lower_fp);
//			fprintf( stderr, "prob_val: %.3lf - class_val: %u - tp: %.3lf - fp: %.3lf - r: %.3lf - p: %.3lf\n", prob_val, class_val, lower_tp, lower_fp, lower_recall, lower_precision );
			double upper_recall = upper_tp / p_count;
			double upper_precision = upper_tp / (upper_tp+upper_fp);
			double rectangle_area = (upper_recall-lower_recall)*lower_precision;
			double triangle_area = 0.5*(upper_recall-lower_recall)*(upper_precision-lower_precision);
//			fprintf( stderr, "Rectangle Area: %.3lf - Triangle Area: %.3lf\n", rectangle_area, triangle_area );
			area += rectangle_area + triangle_area;
					
			lower_tp = upper_tp;
			lower_fp = upper_fp;
		}
	}
	
//	fprintf( stdout, "FP: %lu - TP: %lu - lFP: %lu - lTP: %lu - cip: %lf - cin: %lf - cum_neg: %lf\n", fp_count, tp_count, last_fp_count, last_tp_count, cip, cin, cum_neg );

	return area;
}

void usage( const char* binary ) {
	fprintf( stderr, "Usage: %s [options] precision num_positive\n", binary );
	fprintf( stderr, "Options: -h             Print this usage message and exit\n" );
}

int main( int argc, char* argv[] ) {
	char c;
	while( (c = getopt( argc, argv, "+h" )) >= 0 ) {
		if( c == 'h' ) {
			usage( argv[0] );
			return 0;
		}
	}
	if( argc != optind + 2 ) {
		usage( argv[0] );
		return 1;
	}
	double step_size = atof( argv[optind++] );
	unsigned long p_count = atoi( argv[optind++] );
	double area = auc( step_size, p_count );
	fprintf( stdout, "%.9lf\n", area );
	return 0;
}

