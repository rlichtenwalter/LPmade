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

void ef( unsigned long bins, unsigned long total_instance_count ) {
	double double_threshold = (double) total_instance_count / bins;
	unsigned long threshold = (unsigned long)double_threshold;
	if( double_threshold > threshold ) {
		threshold++;
	}
	
	unsigned long length = 16384;
	double* values = (double*)malloc( length*sizeof(double) );
	unsigned long* running_instance_counts = (unsigned long*)malloc( length*sizeof(unsigned long) );

	int bin_found = 0;
	long max_index = -1;
	unsigned long current_instance = 0;
	unsigned long median_instance = 0;
	unsigned long positives = 0;
	unsigned long negatives = 0;
	double prob_val;
	double last_prob_val = 0.0/0.0;
	unsigned long class_val;
	int result = 0;
	while( (result = fscanf( stdin, "%lf %lu\n", &prob_val, &class_val )) != EOF ) {
		if( prob_val < last_prob_val || isnan(last_prob_val) ) {
			if( bin_found == 1 ) {
				double median = 0;
				unsigned long i;
				for( i = 0; i <= max_index && median == 0; i++ ) {
					if( running_instance_counts[i] >= median_instance ) {
						median = values[i];
					}
				}
				fprintf( stdout, "%lg %lg\n", median, (double)positives / (positives+negatives) );
				bin_found = 0;
				max_index = -1;
				current_instance = 0;
				median_instance = 0;
				positives = 0;
				negatives = 0;
			}
			if( max_index >= length ) {
				length *= 2;
				values = (double*)realloc( values, length*sizeof(double) );
				running_instance_counts = (unsigned long*)realloc( running_instance_counts, length*sizeof(unsigned long) );
			}
			max_index++;
			values[max_index] = prob_val;
			if( max_index > 0 ) {
				running_instance_counts[max_index] = running_instance_counts[max_index-1];
			} else {
				running_instance_counts[max_index] = 0;
			}
		}
//		fprintf( stderr, "last_prob_val: %lf - max_index: %ld\n", last_prob_val, max_index );
		
		if( class_val == 0 ) {
			negatives++;
		} else {
			positives++;
		}
		current_instance++;
		median_instance = current_instance / 2;
		running_instance_counts[max_index]++;
		
//		fprintf( stderr, "Threshold: %lu - Current Instance: %lu - Max Index: %ld\n", threshold, current_instance, max_index );
		if( current_instance >= threshold ) {
			bin_found = 1;
		}
		
		last_prob_val = prob_val;
	}
//	fprintf( stderr, "median_instance: %lu\n", median_instance );
	double median = 0;
	unsigned long i;
	for( i = 0; i <= max_index && median == 0; i++ ) {
//		fprintf( stderr, "\trunning_instance_counts[i]: %lu\n", running_instance_counts[i] );
		if( running_instance_counts[i] >= median_instance ) {
			median = values[i];
		}
	}
	fprintf( stdout, "%lg %lg\n", median, (double)positives / (positives+negatives) );
		
	free( values );
	free( running_instance_counts );
	return;
}

void usage( const char* binary ) {
	fprintf( stderr, "Usage: %s [options] bins instances\n", binary );
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
	unsigned long bins = atoi( argv[optind++] );
	unsigned long total_instance_count = atoi( argv[optind++] );
	ef( bins, total_instance_count );
	return 0;
}
