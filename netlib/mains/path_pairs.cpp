/*
------------------------------------------------
Copyright (C) 2010,2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include "../WeightedNetwork.h"

using std::cin;
using std::cout;
using std::cerr;

void usage( const char* binary ) {
	cerr << "Usage: " << binary << " [options] <path_sequence (I|O)+ (e.g. IIO)>\n";
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
	char* spec = argv[optind];
	unsigned int currentDistance = 0;
	unsigned int distance = strlen( spec );
	vector<bool> direction( distance );
	while( currentDistance < distance ) {
		if( tolower( spec[currentDistance] ) == 'o'  ) {
			direction[currentDistance++] = true;
		} else if( tolower( spec[currentDistance] ) == 'i' ) {
			direction[currentDistance++] = false;
		} else {
			usage( argv[0] );
			return 1;
		}
	}
	
	WeightedNetwork network = WeightedNetwork::readNetwork( cin );
	
	for( vertex_t v1 = 0; v1 < network.vertexCount(); ++v1 ) {
		vector<bool> found = vector<bool>( network.vertexCount() );
		vector<vertex_t> search;
		
		found.at( v1 ) = true;
		search.push_back( v1 );
		for( currentDistance = 0; currentDistance < distance; ++currentDistance ) {
			vector<vertex_t> newSearch;
			for( vector<vertex_t>::const_iterator vertexIterator = search.begin(); vertexIterator != search.end(); ++vertexIterator ) {
				const vertex_t searchVertex = *vertexIterator;
				const neighbor_set_t& neighbors = direction[currentDistance] ? network.outNeighbors( searchVertex ) : network.inNeighbors( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
					const vertex_t neighbor = neighborIterator->first;
					if( !found.at( neighbor ) ) {
						found.at( neighbor ) = true;
						if( currentDistance == distance - 1 ) {
							cout << network.translateIntToExt( v1 ) << " " << network.translateIntToExt( neighbor ) << "\n";
						} else {
							newSearch.push_back( neighbor );
						}
					}
				}
			}
			search.swap( newSearch );
		}
	}
	
	return 0;
}
