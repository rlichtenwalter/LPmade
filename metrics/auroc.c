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

double auc() {
	double prob_val;
	unsigned int class_val;
	double area = 0.0;
	double cum_neg = 0.0;
	unsigned long last_tp_count = 0;
	unsigned long last_fp_count = 0;
	unsigned long tp_count = 0;
	unsigned long fp_count = 0;
	double last_prob_val = 1.1;
	double cip = 0.0;
	double cin = 0.0;
	int result = 0;
	while( (result = fscanf( stdin, "%lg %u\n", &prob_val, &class_val )) != EOF ) {
		if( prob_val < last_prob_val ) {
			cip = tp_count - last_tp_count;
			cin = fp_count - last_fp_count;
//			fprintf( stdout, "FP: %lu - TP: %lu - lFP: %lu - lTP: %lu - cip: %lf - cin: %lf - cum_neg: %lf\n", fp_count, tp_count, last_fp_count, last_tp_count, cip, cin, cum_neg );
			area += cip * (cum_neg + (0.5*cin));
			cum_neg += cin;
			last_tp_count = tp_count;
			last_fp_count = fp_count;
//			fprintf( stdout, "Area: %lf\n", area );
		}
		if( class_val != 1 ) {
			fp_count++;
		} else {
			tp_count++;
		}
		last_prob_val = prob_val;
	}
//	fprintf( stdout, "False Positives: %lu - True Positives: %lu\n", fp_count, tp_count );
	cip = tp_count - last_tp_count;
	cin = fp_count - last_fp_count;
//	fprintf( stdout, "FP: %lu - TP: %lu - lFP: %lu - lTP: %lu - cip: %lf - cin: %lf - cum_neg: %lf\n", fp_count, tp_count, last_fp_count, last_tp_count, cip, cin, cum_neg );
	area += cip * (cum_neg + (0.5*cin));
	cum_neg += cin;
	last_tp_count = tp_count;
	last_fp_count = fp_count;
	
	area /= ((unsigned long long)fp_count*tp_count);
	return 1 - area;
}

void usage( const char* binary ) {
	fprintf( stderr, "Usage: %s [options]\n", binary );
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
	if( argc != optind ) {
		usage( argv[0] );
		return 1;
	}
	double area = auc();
	fprintf( stdout, "%.9lf\n", area );
	return 0;
}

