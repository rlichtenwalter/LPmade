/*
------------------------------------------------
Copyright (C) 2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <cmath>
#include <algorithm>
#include <iostream>
#include "WeightedTriangleLinkPredictor.h"

using std::min;
using std::cerr;

WeightedTriangleLinkPredictor::WeightedTriangleLinkPredictor( const WeightedNetwork& network, int selection = -1 ) : LinkPredictor( network ), selection( selection ) {
	if( this->selection > -1 ) {
		return;
	}
	weight_t allCounts[graphlets] = {0};
	for( vertex_t vertex = 0; vertex < network.vertexCount(); ++vertex ) {
		const neighbor_set_t& outNeighbors = network.outNeighbors( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); ++neighborIterator ) {
			const vertex_t neighbor = neighborIterator->first;
			weight_t vCounts[graphlets] = {0};
			this->edgeProfile( vertex, neighbor, vCounts );
			for( size_t i = 0; i < graphlets; ++i ) {
				allCounts[i] += vCounts[i];
			}
		}
	}
	for( size_t i = 0; i < graphlets; ++i ) {
		this->counts[i] = (weight_t)allCounts[i] / network.edgeCount();
	}
}

void WeightedTriangleLinkPredictor::computeDetailedNeighbors( const neighbor_set_t& inNeighbors, const neighbor_set_t& outNeighbors, DetailedNeighbors& result ) const {
	neighbor_set_t::const_iterator inIt = inNeighbors.begin();
	neighbor_set_t::const_iterator outIt = outNeighbors.begin();
	neighbor_set_t::const_iterator inEnd = inNeighbors.end();
	neighbor_set_t::const_iterator outEnd = outNeighbors.end();
	while( inIt != inEnd && outIt != outEnd ) {
		if( inIt->first < outIt->first ) {
			result.inOnly.push_back( *inIt );
			result.either.push_back( *inIt );
	    	++inIt;
		} else if( inIt->first > outIt->first ) {
			result.outOnly.push_back( *outIt );
			result.either.push_back( *outIt );
	    	++outIt;
		} else {
	    	result.both.push_back( *inIt );
	    	result.either.push_back( *inIt );
	    	++inIt;
	    	++outIt;
		}
	}
	while( inIt != inEnd ) {
		result.inOnly.push_back( *inIt );
		result.either.push_back( *inIt );
		++inIt;
	}
	while( outIt != outEnd ) {
		result.outOnly.push_back( *outIt );
		result.either.push_back( *outIt );
		++outIt;
	}
}

void WeightedTriangleLinkPredictor::edgeProfile( vertex_t v1, vertex_t v2, weight_t vCounts[] ) {
	const neighbor_set_t& v1InNeighbors = network.inNeighbors( v1 );
	const neighbor_set_t& v1OutNeighbors = network.outNeighbors( v1 );
	const neighbor_set_t& v2InNeighbors = network.inNeighbors( v2 );
	const neighbor_set_t& v2OutNeighbors = network.outNeighbors( v2 );
	unsigned char reciprocal = (graphlets/2) * (char)network.hasEdge( v2, v1 );
	DetailedNeighbors v1DetailedNeighbors;
	DetailedNeighbors v2DetailedNeighbors;
	this->computeDetailedNeighbors( v1InNeighbors, v1OutNeighbors, v1DetailedNeighbors );
	this->computeDetailedNeighbors( v2InNeighbors, v2OutNeighbors, v2DetailedNeighbors );
	
	weight_t w = reciprocal ? network.edgeWeight( v2, v1 ) : 1;
	vCounts[0+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.inOnly ) );
	vCounts[1+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.outOnly ) );
	vCounts[2+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.both ) );
	vCounts[3+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.inOnly ) );
	vCounts[4+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.outOnly ) );
	vCounts[5+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.both ) );
	vCounts[6+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.both, v2DetailedNeighbors.inOnly ) );
	vCounts[7+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.both, v2DetailedNeighbors.outOnly ) );
	vCounts[8+reciprocal] += min( w, this->intersectionCount( v1DetailedNeighbors.both, v2DetailedNeighbors.both ) );
	vCounts[9+reciprocal] += min( w, this->differenceCount( v2DetailedNeighbors.inOnly, v1DetailedNeighbors.either ) );
	vCounts[10+reciprocal] += min( w, this->differenceCount( v2DetailedNeighbors.outOnly, v1DetailedNeighbors.either ) );
	vCounts[11+reciprocal] += min( w, this->differenceCount( v2DetailedNeighbors.both, v1DetailedNeighbors.either ) );
	vCounts[12+reciprocal] += min( w, this->differenceCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.either ) );
	vCounts[13+reciprocal] += min( w, this->differenceCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.either ) );
	vCounts[14+reciprocal] += min( w, this->differenceCount( v1DetailedNeighbors.both, v2DetailedNeighbors.either ) );
	vCounts[15+reciprocal] += min( w, this->network.vertexCount() - this->unionCount( v1DetailedNeighbors.either, v2DetailedNeighbors.either ) - 2 );
}

weight_t WeightedTriangleLinkPredictor::intersectionCount( const vector<neighbor_t>& v1, const vector<neighbor_t>& v2 ) const {
	vector<neighbor_t>::const_iterator first1 = v1.begin();
	vector<neighbor_t>::const_iterator last1 = v1.end();
	vector<neighbor_t>::const_iterator first2 = v2.begin();
	vector<neighbor_t>::const_iterator last2 = v2.end();
	weight_t count = 0;
	while( first1 != last1 && first2 != last2 ) {
		if( first1->first < first2->first ) {
			++first1;
		} else if( first2->first < first1->first ) {
			++first2;
		} else {
			count += min( first1->second, first2->second );
			++first1;
			++first2;
		}
	}
	return count;
}

weight_t WeightedTriangleLinkPredictor::differenceCount( const vector<neighbor_t>& v1, const vector<neighbor_t>& v2 ) const {
	vector<neighbor_t>::const_iterator first1 = v1.begin();
	vector<neighbor_t>::const_iterator last1 = v1.end();
	vector<neighbor_t>::const_iterator first2 = v2.begin();
	vector<neighbor_t>::const_iterator last2 = v2.end();
	weight_t count = 0;
	while( first1 != last1 && first2 != last2 ) {
		if( first1->first < first2->first ) {
			count += first1->second;
			++first1;
		} else if( first2->first < first1->first ) {
			++first2;
		} else {
			++first1;
			++first2;
		}
	}
	return count;
}

weight_t WeightedTriangleLinkPredictor::unionCount( const vector<neighbor_t>& v1, const vector<neighbor_t>& v2 ) const {
	vector<neighbor_t>::const_iterator first1 = v1.begin();
	vector<neighbor_t>::const_iterator last1 = v1.end();
	vector<neighbor_t>::const_iterator first2 = v2.begin();
	vector<neighbor_t>::const_iterator last2 = v2.end();
	weight_t count = 0;
	while( first1 != last1 && first2 != last2 ) {
		if( first1->first < first2->first ) {
			++count;
			++first1;
		} else if( first2->first < first1->first ) {
			++count;
			++first2;
		} else {
			++first1;
			++first2;
		}
	}
	return count;
}

WeightedTriangleLinkPredictor::~WeightedTriangleLinkPredictor() {
}

double WeightedTriangleLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	weight_t vCounts[graphlets] = {0};
	this->edgeProfile( vertex, neighbor, vCounts );

	weight_t result = 0;
	if( selection > -1 ) {
		result = vCounts[selection];
	} else {
		for( size_t i = 0; i < graphlets; ++i ) {
			result += pow( this->counts[i] - (double)vCounts[i], 2 );
		}
		result = 1 / result;
	}
	return result;
}
