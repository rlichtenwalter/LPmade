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
	double sum = 0.0;
	unsigned long count = 0;
	int result = 0;
	while( (result = fscanf( stdin, "%lg %u\n", &prob_val, &class_val )) != EOF ) {
		double delta = class_val - prob_val;
		double se = delta * delta;
		sum += se;
		count++;
	}
	return sum / count;
}

void usage( const char* binary ) {
	fprintf( stderr, "Usage: %s [options]\n", binary );
	fprintf( stderr, "Options: -h             Print this usage message and exit\n" );
}

int main() {
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
	double mse = auc();
	fprintf( stdout, "%.9lf\n", mse );
	return 0;
}

