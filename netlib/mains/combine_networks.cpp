/*
------------------------------------------------
Copyright (C) 2010,2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <iostream>
#include <fstream>
#include <unistd.h>

#include "../WeightedNetwork.h"

using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;

void usage( const char* binary ) {
	cerr << "Usage: " << binary << " [options] <list of network files>\n";
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
	if( argc < optind + 1 ) {
		usage( argv[0] );
		return 1;
	}
	
	const char* filename = argv[optind++];
	ifstream file;
	file.open( filename );
	WeightedNetwork network = WeightedNetwork::readNetwork( file );
	file.close();
	for( int index = 2; index < argc; index++ ) {
		filename = argv[index];
		file.open( filename );
		WeightedNetwork toAdd = WeightedNetwork::readNetwork( file );
		network = WeightedNetwork::combine( network, toAdd );
		file.close();
	}
	network.print( cout );
	
	return 0;
}
