/*
------------------------------------------------
Copyright (C) 2010,2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <iostream>
#include <unistd.h>

#include "../WeightedNetwork.h"

using std::cin;
using std::cout;
using std::cerr;
using std::ios_base;
using std::scientific;

void usage( const char* binary ) {
	cerr << "Usage: " << binary << " [options] C\n";
	cerr << "Options: -h             Print this usage message and exit\n";
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
	double C = atof( argv[optind++] );
	
	WeightedNetwork network = WeightedNetwork::readNetwork( cin );
	vector<vector<double> > simRanks = network.simRank( C );
	for( vector<vector<double> >::const_iterator dim1 = simRanks.begin(); dim1 != simRanks.end(); dim1++ ) {
		for( vector<double>::const_iterator dim2 = dim1->begin(); dim2 != dim1->end(); dim2++ ) {
			if( *dim2 != int(*dim2) ) {
				cout << scientific << *dim2 << "\n";
			} else {
				cout.unsetf( ios_base::floatfield );
				cout << *dim2 << "\n";
			}
		}
	}
	
	return 0;
}
