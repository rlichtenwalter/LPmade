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
#include <math.h>
#include <unistd.h>

void topnprec( unsigned long total_instance_count ) {
	unsigned long instances = 0;
	unsigned long positives = 0;

	double prob_val;
	double last_prob_val = 0.0/0.0;
	unsigned long class_val;
	int result = 0;
	while( (result = fscanf( stdin, "%lf %lu\n", &prob_val, &class_val )) != EOF ) {
		if( prob_val < last_prob_val || isnan(last_prob_val) ) {
			if( instances > 0 ) {
				fprintf( stdout, "%lg %lg\n", (double)instances/total_instance_count, (double)positives/instances );
			}
			instances++;
			if( class_val == 1 ) {
				positives++;
			}
		}
	}
	fprintf( stdout, "%lg %lg\n", (double)instances/total_instance_count, (double)positives/instances );
}

void usage( const char* binary ) {
	fprintf( stderr, "Usage: %s [options] num_instances\n", binary );
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
	if( argc != optind + 1 ) {
		usage( argv[0] );
		return 1;
	}
	unsigned long total_instance_count = atoi( argv[optind++] );
	topnprec( total_instance_count );
	return 0;
}
