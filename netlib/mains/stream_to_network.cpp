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

void usage( const char* binary ) {
	cerr << "Usage: " << binary << " [options] seperator iindex jindex\n";
	cerr << "Options: -h             Print this usage message and exit\n";
	cerr << "         -i             Ignore self-edges\n";
}

int main( int argc, char* argv[] ) {
	bool ignoreSelfEdges = false;
	char c;
	while( (c = getopt( argc, argv, "+i" )) >= 0 ) {
		if( c == 'h' ) {
			usage( argv[0] );
			return 0;
		} else if( c == 'i' ) {
			ignoreSelfEdges = true;
		}
	}
	
	if( argc != optind + 3 ) {
		usage( argv[0] );
		return 1;
	}
	const char separator = argv[optind++][0];
	const unsigned int iIndex = atoi( argv[optind++] );
	const unsigned int jIndex = atoi( argv[optind++] );


	WeightedNetwork network = WeightedNetwork::readStream( cin, separator, iIndex, jIndex, ignoreSelfEdges );
	network.print( cout );
		
	return 0;
}
