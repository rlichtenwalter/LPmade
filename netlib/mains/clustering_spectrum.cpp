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
	cerr << "Usage: " << binary << " [options]\n";
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
	if( argc != optind ) {
		usage( argv[0] );
		return 1;
	}
	
	WeightedNetwork network = WeightedNetwork::readNetwork( cin );
	
	vector<double> sum( 10, 0.0 );
	vector<unsigned int> count( 10, 0 );
	
	for( vertex_t vertex = 0; vertex < network.vertexCount(); vertex++ ) {
		unsigned int degree = network.outDegree( vertex );
		double clusteringCoefficient = network.clusteringCoefficient( vertex );
		if( degree >= sum.size() ) {
			sum.resize( degree + 1, 0.0 );
			count.resize( degree + 1, 0 );
		}
		sum.at( degree ) += clusteringCoefficient;
		++count.at( degree );
	}
	
	for( unsigned int degree = 0; degree < sum.size(); ++degree ) {
		double degreeSum = sum.at( degree );
		unsigned int degreeCount = count.at( degree );
		if( degreeCount > 0 ) {
			double average = degreeSum / degreeCount;
			cout << degree << " " << average << "\n";
		}
	}
	
	return 0;
}
