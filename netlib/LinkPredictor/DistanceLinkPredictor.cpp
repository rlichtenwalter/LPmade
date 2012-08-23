/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include "DistanceLinkPredictor.h"

#include <iostream>
using std::cout;
using std::cerr;

DistanceLinkPredictor::DistanceLinkPredictor( const WeightedNetwork& network, unsigned int l ) : LinkPredictor(network), l(l) {
}

DistanceLinkPredictor::~DistanceLinkPredictor() {
}

double DistanceLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	if( this->vertex != vertex ) {
		this->vertex = vertex;
		this->scores = vector<double>( this->network.vertexCount() );
		vector<vertex_t> toSearch;

		this->scores.at( vertex ) = 0;
		toSearch.push_back( vertex );
		for( unsigned int distance = 1; distance <= this->l; distance++ ) {
			vector<vertex_t> found;
			for( vector<vertex_t>::const_iterator vertexIterator = toSearch.begin(); vertexIterator != toSearch.end(); ++vertexIterator ) { 
				const vertex_t searchVertex = *vertexIterator;
				const neighbor_set_t& neighbors = this->network.outNeighbors( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
					const vertex_t currentNeighbor = neighborIterator->first;
					if( vertex != currentNeighbor && this->scores.at( currentNeighbor ) == 0 ) {
						this->scores.at( currentNeighbor ) = 1.0 / distance;
						found.push_back( currentNeighbor );
					}
				}
			}
			toSearch.swap( found );
		}
	}
	return this->scores.at( neighbor );
}
