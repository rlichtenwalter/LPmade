/*
------------------------------------------------
Copyright (C) 2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <cmath>
#include <iostream>
#include "VCP3UndirectedLinkPredictor.h"

using std::cout;
using std::cerr;

VCP3UndirectedLinkPredictor::VCP3UndirectedLinkPredictor( const WeightedNetwork& network ) : LinkPredictor( network ) {
}

VCP3UndirectedLinkPredictor::DetailedNeighbors VCP3UndirectedLinkPredictor::computeDetailedNeighbors( const neighbor_set_t& inNeighbors, const neighbor_set_t& outNeighbors, vertex_t avoidance ) const {
	neighbor_set_t::const_iterator inIt = inNeighbors.begin();
	neighbor_set_t::const_iterator outIt = outNeighbors.begin();
	neighbor_set_t::const_iterator inEnd = inNeighbors.end();
	neighbor_set_t::const_iterator outEnd = outNeighbors.end();
	DetailedNeighbors result;
	while( inIt != inEnd && outIt != outEnd ) {
		if( inIt->first == avoidance ) {
			++inIt;
			continue;
		}
		if( outIt->first == avoidance ) {
			++outIt;
			continue;
		}
		if( inIt->first < outIt->first ) {
			result.either.push_back( inIt->first );
	    	++inIt;
		} else if( inIt->first > outIt->first ) {
			result.either.push_back( outIt->first );
	    	++outIt;
		} else {
	    	result.either.push_back( inIt->first );
	    	++inIt;
	    	++outIt;
		}
	}
	while( inIt != inEnd ) {
		if( inIt->first == avoidance ) {
			++inIt;
			continue;
		}
		result.either.push_back( inIt->first );
		++inIt;
	}
	while( outIt != outEnd ) {
		if( outIt->first == avoidance ) {
			++outIt;
			continue;
		}
		result.either.push_back( outIt->first );
		++outIt;
	}
	return result;
}

void VCP3UndirectedLinkPredictor::edgeProfile( vertex_t v1, vertex_t v2, unsigned long vCounts[] ) const {
	unsigned char reciprocal = (graphlets/2) * (char)network.hasEdge( v2, v1 );
	DetailedNeighbors v1DetailedNeighbors = this->computeDetailedNeighbors( network.inNeighbors( v1 ), network.outNeighbors( v1 ), v2 );
	DetailedNeighbors v2DetailedNeighbors = this->computeDetailedNeighbors( network.inNeighbors( v2 ), network.outNeighbors( v2 ), v1 );
	
	vCounts[reciprocal] += this->network.vertexCount() - this->unionCount( v1DetailedNeighbors.either, v2DetailedNeighbors.either ) - 2;
	vCounts[1+reciprocal] += this->differenceCount( v1DetailedNeighbors.either, v2DetailedNeighbors.either );
	vCounts[2+reciprocal] += this->differenceCount( v2DetailedNeighbors.either, v1DetailedNeighbors.either );
	vCounts[3+reciprocal] += this->intersectionCount( v1DetailedNeighbors.either, v2DetailedNeighbors.either );
}

unsigned long VCP3UndirectedLinkPredictor::intersectionCount( const vector<vertex_t>& v1, const vector<vertex_t>& v2 ) const {
	vector<vertex_t>::const_iterator first1 = v1.begin();
	vector<vertex_t>::const_iterator last1 = v1.end();
	vector<vertex_t>::const_iterator first2 = v2.begin();
	vector<vertex_t>::const_iterator last2 = v2.end();
	unsigned long count = 0;
	while( first1 != last1 && first2 != last2 ) {
		if( *first1 < *first2 ) {
			++first1;
		} else if( *first2 < *first1 ) {
			++first2;
		} else {
			++count;
			++first1;
			++first2;
		}
	}
	return count;
}

unsigned long VCP3UndirectedLinkPredictor::differenceCount( const vector<vertex_t>& v1, const vector<vertex_t>& v2 ) const {
	vector<vertex_t>::const_iterator first1 = v1.begin();
	vector<vertex_t>::const_iterator last1 = v1.end();
	vector<vertex_t>::const_iterator first2 = v2.begin();
	vector<vertex_t>::const_iterator last2 = v2.end();
	unsigned long count = 0;
	while( first1 != last1 && first2 != last2 ) {
		if( *first1 < *first2 ) {
			++count;
			++first1;
		} else if( *first2 < *first1 ) {
			++first2;
		} else {
			++first1;
			++first2;
		}
	}
	while( first1 != last1 ) {
		++count;
		++first1;
	}
	return count;
}

unsigned long VCP3UndirectedLinkPredictor::unionCount( const vector<vertex_t>& v1, const vector<vertex_t>& v2 ) const {
	vector<vertex_t>::const_iterator first1 = v1.begin();
	vector<vertex_t>::const_iterator last1 = v1.end();
	vector<vertex_t>::const_iterator first2 = v2.begin();
	vector<vertex_t>::const_iterator last2 = v2.end();
	unsigned long count = 0;
	while( first1 != last1 && first2 != last2 ) {
		if( *first1 < *first2 ) {
			++count;
			++first1;
		} else if( *first2 < *first1 ) {
			++count;
			++first2;
		} else {
			++count;
			++first1;
			++first2;
		}
	}
	while( first1 != last1 ) {
		++count;
		++first1;
	}
	while( first2 != last2 ) {
		++count;
		++first2;
	}
	return count;
}

VCP3UndirectedLinkPredictor::~VCP3UndirectedLinkPredictor() {
}

double VCP3UndirectedLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	unsigned long vCounts[graphlets] = {0};
	this->edgeProfile( vertex, neighbor, vCounts );

	cerr << network.translateIntToExt( vertex ) << " " << network.translateIntToExt( neighbor );
	for( size_t i = 0; i < graphlets; ++i ) {
		cerr << " " << vCounts[i];
	}
	cerr << "\n";

	return -1;
}
