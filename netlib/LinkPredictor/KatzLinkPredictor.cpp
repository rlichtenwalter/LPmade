/*
------------------------------------------------
Copyright (C) 2010,2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <stack>
#include <utility>
#include <cmath>
#include "KatzLinkPredictor.h"

using std::stack;
using std::make_pair;
using std::cout;
using std::cerr;

KatzLinkPredictor::KatzLinkPredictor( const WeightedNetwork& network, unsigned int l, double beta ) : LinkPredictor(network), l(l), beta(beta), lookup(vector<double>(l+1)) {
	for( unsigned int degree = 0; degree <= l; ++degree ) {
		this->lookup.at( degree ) = pow( beta, degree );
	}
}

KatzLinkPredictor::~KatzLinkPredictor() {
}

double KatzLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	if( this->vertex != vertex ) {
		this->vertex = vertex;
		this->scores = vector<double>( this->network.vertexCount() );
		
		vector<vertex_t> oldSearch;
		vector<unsigned long> oldHits( this->network.vertexCount() );
		
		oldSearch.push_back( vertex );
		oldHits.at( vertex ) = 1;
		for( unsigned int degree = 1; degree <= this->l; ++degree ) {
			vector<unsigned long> newHits( this->network.vertexCount() );
			vector<bool> found( this->network.vertexCount() );
			vector<vertex_t> newSearch;
			for( vector<vertex_t>::const_iterator vertexIterator = oldSearch.begin(); vertexIterator != oldSearch.end(); ++vertexIterator ) { 
				const vertex_t searchVertex = *vertexIterator;
				const neighbor_set_t& neighbors = this->network.outNeighbors( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
					const vertex_t neighborVertex = neighborIterator->first;
					newHits.at( neighborVertex ) += oldHits.at( searchVertex );
					if( !found.at( neighborVertex ) ) {
						found.at( neighborVertex ) = true;
						newSearch.push_back( neighborVertex );
					}
				}
			}
			double factor = this->lookup.at( degree );
			for( size_t i = 0; i < this->scores.size(); ++i ) {
				this->scores.at( i ) += factor * newHits.at( i );
			}
			oldSearch.swap( newSearch );
			oldHits.swap( newHits );

		}
	}
	return this->scores.at( neighbor );
}
