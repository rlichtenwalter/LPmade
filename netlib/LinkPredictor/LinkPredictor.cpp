/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <iostream>
#include <limits>
#include "LinkPredictor.h"

using std::numeric_limits;
using std::cout;
using std::cerr;
using std::scientific;
using std::ios_base;

LinkPredictor::LinkPredictor( const WeightedNetwork& network ) : network(network), vertex(INVALID_VERTEX), neighbor(INVALID_VERTEX) {
}

LinkPredictor::~LinkPredictor() {
}

void LinkPredictor::printOutNeighborScores( unsigned int degree ) {
	// if the degree is 0, do all-pairs predictions
	if( degree == 0 ) {
		for( vertex_t vertex = 0; vertex < network.vertexCount(); ++vertex ) {
			for( vertex_t neighbor = 0; neighbor < network.vertexCount(); ++neighbor ) {
				if( vertex != neighbor && !network.hasEdge( vertex, neighbor ) ) {
					// DO WE PREDICT LINKS THAT ALREADY EXIST? - // DO PROVIDED NETWORKS INCLUDE LINKS IN BOTH DIRECTIONS
					double predictedValue = this->generateScore( vertex, neighbor );
					unsigned int vertexName = this->network.translateIntToExt( vertex );
					unsigned int neighborName = this->network.translateIntToExt( neighbor );
					cout << vertexName << " " << neighborName << " " << predictedValue << "\n";;
				}
			}
		}
		return;
	}
	for( vertex_t vertex = 0; vertex < network.vertexCount(); vertex++ ) {
		vector<vertex_t> verticesToPredict;
		verticesToPredict = this->network.findOutNeighbors( vertex, degree );
		for( vector<unsigned int>::const_iterator neighborIterator = verticesToPredict.begin(); neighborIterator != verticesToPredict.end(); neighborIterator++ ) {
			vertex_t neighbor = *neighborIterator;
			double predictedValue = this->generateScore( vertex, neighbor );
			unsigned int vertexName = this->network.translateIntToExt( vertex );
			unsigned int neighborName = this->network.translateIntToExt( neighbor );
			cout << vertexName << " " << neighborName << " " << predictedValue << "\n";;
		}
	}
}

void LinkPredictor::printInNeighborScores( unsigned int degree ) {
	// if the degree is 0, do all-pairs predictions
	if( degree == 0 ) {
		for( vertex_t vertex = 0; vertex < network.vertexCount(); ++vertex ) {
			for( vertex_t neighbor = 0; neighbor < network.vertexCount(); ++neighbor ) {
				if( vertex != neighbor && !network.hasEdge( neighbor, vertex ) ) {
					// DO WE PREDICT LINKS THAT ALREADY EXIST? - // DO PROVIDED NETWORKS INCLUDE LINKS IN BOTH DIRECTIONS
					double predictedValue = this->generateScore( vertex, neighbor );
					unsigned int vertexName = this->network.translateIntToExt( vertex );
					unsigned int neighborName = this->network.translateIntToExt( neighbor );
					cout << vertexName << " " << neighborName << " " << predictedValue << "\n";;
				}
			}
		}
		return;
	}
	for( vertex_t vertex = 0; vertex < network.vertexCount(); vertex++ ) {
		vector<vertex_t> verticesToPredict;
		verticesToPredict = this->network.findInNeighbors( vertex, degree );
		for( vector<unsigned int>::const_iterator neighborIterator = verticesToPredict.begin(); neighborIterator != verticesToPredict.end(); neighborIterator++ ) {
			vertex_t neighbor = *neighborIterator;
			double predictedValue = this->generateScore( vertex, neighbor );
			unsigned int vertexName = this->network.translateIntToExt( vertex );
			unsigned int neighborName = this->network.translateIntToExt( neighbor );
			cout << vertexName << " " << neighborName << " " << predictedValue << "\n";;
		}
	}
}
