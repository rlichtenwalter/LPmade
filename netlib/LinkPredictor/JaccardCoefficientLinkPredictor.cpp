/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include "JaccardCoefficientLinkPredictor.h"

JaccardCoefficientLinkPredictor::JaccardCoefficientLinkPredictor( const WeightedNetwork& network ) : LinkPredictor(network) {
}

JaccardCoefficientLinkPredictor::~JaccardCoefficientLinkPredictor() {
}

double JaccardCoefficientLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	unsigned int intersectionSize = 0;
	unsigned int unionSize = 0;
	
	const neighbor_set_t& vertexNeighbors = this->network.outNeighbors( vertex );
	const neighbor_set_t& neighborNeighbors = this->network.outNeighbors( neighbor );
	
	neighbor_set_t::const_iterator first1 = vertexNeighbors.begin();
	neighbor_set_t::const_iterator first2 = neighborNeighbors.begin();
	neighbor_set_t::const_iterator last1 = vertexNeighbors.end();
	neighbor_set_t::const_iterator last2 = neighborNeighbors.end();
	
	// if one or both individuals have no neighbors, define the score as 0 rather than potentially 0/0 == nan
	if( first1 == last1 || first2 == last2 ) {
		return 0;
	}
	
	while (true) {
		if( first1 == last1 ) {
			unionSize += last2 - first2;
			return (double)intersectionSize / unionSize;
		}
		if( first2 == last2 ) {
			unionSize += last1 - first1;
			return (double)intersectionSize / unionSize;
		}
		
    	if( first1->first < first2->first ) {
			first1++;
		} else if( first2->first < first1->first ) {
			first2++;
		} else {
			first1++;
			first2++;
			intersectionSize++;
		}
		unionSize++;
	}
}
