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
#include "MRLPLinkPredictor.h"

using std::cerr;

MRLPLinkPredictor::MRLPLinkPredictor( const WeightedNetwork& network ) : LinkPredictor( network ) {
	unsigned long allCounts[graphlets] = {0};
	for( vertex_t vertex = 0; vertex < network.vertexCount(); ++vertex ) {
		const neighbor_set_t& outNeighbors = network.outNeighbors( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); ++neighborIterator ) {
			const vertex_t neighbor = neighborIterator->first;
			unsigned long vCounts[graphlets] = {0};
			this->edgeProfile( vertex, neighbor, vCounts );
			for( size_t i = 0; i < graphlets; ++i ) {
				allCounts[i] += vCounts[i];
			}
		}
	}
	this->bidirectional = network.countMutuals();
	this->unidirectional = network.edgeCount() - network.countMutuals() * 2;
	for( size_t i = 0; i < graphlets; ++i ) {
		this->counts[i] = (double)allCounts[i] / network.edgeCount();
	}
}

void MRLPLinkPredictor::computeDetailedNeighbors( const neighbor_set_t& inNeighbors, const neighbor_set_t& outNeighbors, DetailedNeighbors& result ) const {
	neighbor_set_t::const_iterator inIt = inNeighbors.begin();
	neighbor_set_t::const_iterator outIt = outNeighbors.begin();
	neighbor_set_t::const_iterator inEnd = inNeighbors.end();
	neighbor_set_t::const_iterator outEnd = outNeighbors.end();
	while( inIt != inEnd && outIt != outEnd ) {
		if( inIt->first < outIt->first ) {
			result.inOnly.push_back( inIt->first );
			result.either.push_back( inIt->first );
	    	++inIt;
		} else if( inIt->first > outIt->first ) {
			result.outOnly.push_back( outIt->first );
			result.either.push_back( outIt->first );
	    	++outIt;
		} else {
	    	result.both.push_back( inIt->first );
	    	result.either.push_back( inIt->first );
	    	++inIt;
	    	++outIt;
		}
	}
	while( inIt != inEnd ) {
		result.inOnly.push_back( inIt->first );
		result.either.push_back( inIt->first );
		++inIt;
	}
	while( outIt != outEnd ) {
		result.outOnly.push_back( outIt->first );
		result.either.push_back( outIt->first );
		++outIt;
	}
}

void MRLPLinkPredictor::edgeProfile( vertex_t v1, vertex_t v2, unsigned long vCounts[] ) {
	const neighbor_set_t& v1InNeighbors = network.inNeighbors( v1 );
	const neighbor_set_t& v1OutNeighbors = network.outNeighbors( v1 );
	const neighbor_set_t& v2InNeighbors = network.inNeighbors( v2 );
	const neighbor_set_t& v2OutNeighbors = network.outNeighbors( v2 );
	unsigned char reciprocal = (graphlets/2) * (char)network.hasEdge( v2, v1 );
	DetailedNeighbors v1DetailedNeighbors;
	DetailedNeighbors v2DetailedNeighbors;
	this->computeDetailedNeighbors( v1InNeighbors, v1OutNeighbors, v1DetailedNeighbors );
	this->computeDetailedNeighbors( v2InNeighbors, v2OutNeighbors, v2DetailedNeighbors );
	
	vCounts[0+reciprocal] += this->intersectionCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.inOnly );
	vCounts[1+reciprocal] += this->intersectionCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.outOnly );
	vCounts[2+reciprocal] += this->intersectionCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.both );
	vCounts[3+reciprocal] += this->intersectionCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.inOnly );
	vCounts[4+reciprocal] += this->intersectionCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.outOnly );
	vCounts[5+reciprocal] += this->intersectionCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.both );
	vCounts[6+reciprocal] += this->intersectionCount( v1DetailedNeighbors.both, v2DetailedNeighbors.inOnly );
	vCounts[7+reciprocal] += this->intersectionCount( v1DetailedNeighbors.both, v2DetailedNeighbors.outOnly );
	vCounts[8+reciprocal] += this->intersectionCount( v1DetailedNeighbors.both, v2DetailedNeighbors.both );
	vCounts[9+reciprocal] += this->differenceCount( v2DetailedNeighbors.inOnly, v1DetailedNeighbors.either );
	vCounts[10+reciprocal] += this->differenceCount( v2DetailedNeighbors.outOnly, v1DetailedNeighbors.either );
	vCounts[11+reciprocal] += this->differenceCount( v2DetailedNeighbors.both, v1DetailedNeighbors.either );
	vCounts[12+reciprocal] += this->differenceCount( v1DetailedNeighbors.inOnly, v2DetailedNeighbors.either );
	vCounts[13+reciprocal] += this->differenceCount( v1DetailedNeighbors.outOnly, v2DetailedNeighbors.either );
	vCounts[14+reciprocal] += this->differenceCount( v1DetailedNeighbors.both, v2DetailedNeighbors.either );
	vCounts[15+reciprocal] += this->network.vertexCount() - this->unionCount( v1DetailedNeighbors.either, v2DetailedNeighbors.either ) - 2;
}

unsigned long MRLPLinkPredictor::intersectionCount( const vector<vertex_t>& v1, const vector<vertex_t>& v2 ) const {
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

unsigned long MRLPLinkPredictor::differenceCount( const vector<vertex_t>& v1, const vector<vertex_t>& v2 ) const {
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
	return count;
}

unsigned long MRLPLinkPredictor::unionCount( const vector<vertex_t>& v1, const vector<vertex_t>& v2 ) const {
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
			++first1;
			++first2;
		}
	}
	return count;
}

MRLPLinkPredictor::~MRLPLinkPredictor() {
}

double MRLPLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	unsigned long vCounts[graphlets] = {0};
	this->edgeProfile( vertex, neighbor, vCounts );

	double result = 0;
	for( size_t i = 0; i < graphlets; ++i ) {
		result += pow( this->counts[i] - (double)vCounts[i], 2 );
	}
	result = 1 / result;

	return result;
}
