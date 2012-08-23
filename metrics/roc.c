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

void auc( unsigned long negatives, unsigned long positives ) {
	double prob_val;
	unsigned int class_val;
	unsigned long tp_count = 0;
	unsigned long fp_count = 0;
	double last_prob_val = 1.1;
	int result = 0;
	while( (result = fscanf( stdin, "%lf %u\n", &prob_val, &class_val )) != EOF ) {
		if( prob_val < last_prob_val ) {
			fprintf( stdout, "%.6lf %.6lf\n", (double)fp_count/(double)negatives, (double)tp_count/(double)positives );
		}
		if( class_val != 1 ) {
			fp_count++;
		} else {
			tp_count++;
		}
		last_prob_val = prob_val;
	}
	fprintf( stdout, "%.6lf %.6lf\n", (double)fp_count/(double)negatives, (double)tp_count/(double)positives );
	return;
}

void usage( const char* binary ) {
	fprintf( stderr, "Usage: %s [options] num_negative num_positive\n", binary );
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
	unsigned long negative = atol( argv[optind++] );
	unsigned long positive = atol( argv[optind++] );
	auc( negative, positive );
	return 0;
}

