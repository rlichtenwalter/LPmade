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
#include <cstring>
#include <cstdlib>

#include "../AdamicAdarLinkPredictor.h"
#include "../ClusteringCoefficientLinkPredictor.h"
#include "../CommonNeighborLinkPredictor.h"
#include "../DistanceLinkPredictor.h"
#include "../IDegreeLinkPredictor.h"
#include "../IPageRankLinkPredictor.h"
#include "../IVolumeLinkPredictor.h"
#include "../JaccardCoefficientLinkPredictor.h"
#include "../JDegreeLinkPredictor.h"
#include "../JPageRankLinkPredictor.h"
#include "../JVolumeLinkPredictor.h"
#include "../KatzLinkPredictor.h"
#include "../LinkPredictor.h"
#include "../MutualityLinkPredictor.h"
#include "../OneLinkPredictor.h"
#include "../PreferentialAttachmentLinkPredictor.h"
#include "../PropFlowLinkPredictor.h"
#include "../RootedPageRankLinkPredictor.h"
#include "../ShortestPathCountLinkPredictor.h"
#include "../SimRankLinkPredictor.h"
#include "../UnweightedPropFlowLinkPredictor.h"
#include "../VCP3DirectedLinkPredictor.h"
#include "../VCP3UndirectedLinkPredictor.h"
#include "../VCP4DirectedLinkPredictor.h"
#include "../VCP4UndirectedLinkPredictor.h"
#include "../WeightedRootedPageRankLinkPredictor.h"
#include "../../WeightedNetwork.h"

using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ios;
using std::ios_base;
using std::scientific;

void usage( const char* binary ) {
	cerr << "Usage: " << binary << " [options] predictor\n";
	cerr << "Options: -h             Print this usage message and exit\n";
	cerr << "         -d DIRECTION   The direction of edges to use; must be in {I,O}\n";
	cerr << "         -f NETWORK     File containing the network in which to predict\n";
	cerr << "         -n DISTANCE    Degree of neighborhood in which to predict\n";
	cerr << "         -c             Specifies that node-pairs for prediction are available at standard input\n";
}

int main( int argc, char* argv[] ) {
	char c;
	bool specifiedPredictions = false;
	bool outEdges = true;
	unsigned int degree = 2;
	ifstream file;
	while( (c = getopt( argc, argv, "+hcd:f:n:" )) >= 0 ) {
		if( c == 'd' ) {
			if( strcmp( optarg, "I" ) == 0 ) {
				outEdges = false;
			}
		} else if( c == 'f' ) {
			file.open( optarg, ios::in );
		} else if( c == 'n' ) {
			degree = atoi( optarg );
		} else if( c == 'c' ) {
			specifiedPredictions = true;
		} else if( c == 'h' ) {
			usage( argv[0] );
			exit(0);
		}
	}
	if( argc < optind + 1 ) {
		usage( argv[0] );
		return 1;
	}

	const char* predictorName = argv[optind++];
	
	WeightedNetwork network = WeightedNetwork::readNetwork( file );
	if( !outEdges ) {
		network = network.reverseEdges();
	}
	
	LinkPredictor* predictor = NULL;
	if( strcmp( predictorName, "AdamicAdar" ) == 0 ) {
		predictor = new AdamicAdarLinkPredictor( network );
	} else if( strcmp( predictorName, "CommonNeighbor" ) == 0 ) {
		predictor = new CommonNeighborLinkPredictor( network );
	} else if( strcmp( predictorName, "ClusteringCoefficient" ) == 0 ) {
		predictor = new ClusteringCoefficientLinkPredictor( network );
	} else if( strcmp( predictorName, "Distance" ) == 0 ) {
		unsigned int l = argc > optind ? atoi( argv[optind++] ) : 5;
		predictor = new DistanceLinkPredictor( network, l );
	} else if( strcmp( predictorName, "IDegree" ) == 0 ) {
		predictor = new IDegreeLinkPredictor( network );
	} else if( strcmp( predictorName, "IPageRank" ) == 0 ) {
		double d = argc > optind ? atof( argv[optind++] ) : 0.85;
		predictor = new IPageRankLinkPredictor( network, d );
	} else if( strcmp( predictorName, "IVolume" ) == 0 ) {
		predictor = new IVolumeLinkPredictor( network );
	} else if( strcmp( predictorName, "JaccardCoefficient" ) == 0 ) {
		predictor = new JaccardCoefficientLinkPredictor( network );
	} else if( strcmp( predictorName, "JDegree" ) == 0 ) {
		predictor = new JDegreeLinkPredictor( network );
	} else if( strcmp( predictorName, "JPageRank" ) == 0 ) {
		double d = argc > optind ? atof( argv[optind++] ) : 0.85;
		predictor = new JPageRankLinkPredictor( network, d );
	} else if( strcmp( predictorName, "JVolume" ) == 0 ) {
		predictor = new JVolumeLinkPredictor( network );
	} else if( strcmp( predictorName, "Katz" ) == 0 ) {
		unsigned int l = argc > optind ? atoi( argv[optind++] ) : 5;
		double beta = argc > optind ? atof( argv[optind++] ) : 0.005;
		predictor = new KatzLinkPredictor( network, l, beta );
	} else if( strcmp( predictorName, "Mutuality" ) == 0 ) {
		predictor = new MutualityLinkPredictor( network );
	} else if( strcmp( predictorName, "One" ) == 0 ) {
		predictor = new OneLinkPredictor( network );
	} else if( strcmp( predictorName, "PreferentialAttachment" ) == 0 ) {
		predictor = new PreferentialAttachmentLinkPredictor( network );
	} else if( strcmp( predictorName, "PropFlow" ) == 0 ) {
		unsigned int l = argc > optind ? atoi( argv[optind++] ) : 5;
		predictor = new PropFlowLinkPredictor( network, l );
	} else if( strcmp( predictorName, "RootedPageRank" ) == 0 ) {
		double alpha = argc > optind ? atof( argv[optind++] ) : 0.15;
		predictor = new RootedPageRankLinkPredictor( network, alpha );
	} else if( strcmp( predictorName, "ShortestPathCount" ) == 0 ) {
		unsigned int l = argc > optind ? atoi( argv[optind++] ) : 5;
		predictor = new ShortestPathCountLinkPredictor( network, l );
	} else if( strcmp( predictorName, "SimRank" ) == 0 ) {
		double C = argc > optind ? atof( argv[optind++ ] ) : 0.8;
		predictor = new SimRankLinkPredictor( network, C );
	} else if( strcmp( predictorName, "UnweightedPropFlow" ) == 0 ) {
		unsigned int l = argc > optind ? atoi( argv[optind++] ) : 5;
		predictor = new UnweightedPropFlowLinkPredictor( network, l );
	} else if( strcmp( predictorName, "VCP3Directed" ) == 0 ) {
		predictor = new VCP3DirectedLinkPredictor( network );
	} else if( strcmp( predictorName, "VCP3Undirected" ) == 0 ) {
		predictor = new VCP3UndirectedLinkPredictor( network );
	} else if( strcmp( predictorName, "VCP4Directed" ) == 0 ) {
		predictor = new VCP4DirectedLinkPredictor( network );
	} else if( strcmp( predictorName, "VCP4Undirected" ) == 0 ) {
		predictor = new VCP4UndirectedLinkPredictor( network );
	} else if( strcmp( predictorName, "WeightedRootedPageRank" ) == 0 ) {
		double alpha = argc > optind ? atof( argv[optind++] ) : 5;
		predictor = new WeightedRootedPageRankLinkPredictor( network, alpha );
	} else {
		cerr << "invalid link predictor: " << predictorName << "\n";
		return 1;
	}
	
	cout.precision( 15 );
	if( specifiedPredictions ) {
		vertex_t vertex;
		vertex_t neighbor;
		while( cin >> vertex >> neighbor ) {
			vertex_t internalVertex = network.translateExtToInt( vertex );
			vertex_t internalNeighbor = network.translateExtToInt( neighbor );
			if( internalVertex != INVALID_VERTEX && internalNeighbor != INVALID_VERTEX ) {
				cout << vertex << " " << neighbor << " " << predictor->generateScore( internalVertex, internalNeighbor );
			}
		}
	} else {
		if( !outEdges ) {
			predictor->printInNeighborScores( degree );
		} else {
			predictor->printOutNeighborScores( degree );
		}
	}
	delete predictor;
	return 0;
}
