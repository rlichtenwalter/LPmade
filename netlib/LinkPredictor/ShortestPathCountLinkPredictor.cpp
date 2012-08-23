/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include "ShortestPathCountLinkPredictor.h"

ShortestPathCountLinkPredictor::ShortestPathCountLinkPredictor( const WeightedNetwork& network, unsigned int l ) : LinkPredictor(network), l(l) {
}

ShortestPathCountLinkPredictor::~ShortestPathCountLinkPredictor() {
}

double ShortestPathCountLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	if( this->vertex != vertex ) {
		this->vertex = vertex;
		this->scores = vector<double>( this->network.vertexCount() );
		
		vector<bool> found = vector<bool>( this->network.vertexCount() );
		
		vector<vertex_t>* oldSearch = new vector<vertex_t>();
		vector<vertex_t>* newSearch = new vector<vertex_t>();
	
		found.at( vertex ) = true;
		newSearch->push_back( vertex );
		for( unsigned int degree = 0; degree < this->l; degree++ ) {
			delete oldSearch;
			oldSearch = newSearch;
			newSearch = new vector<vertex_t>();
			for( vector<vertex_t>::const_iterator vertexIterator = oldSearch->begin(); vertexIterator != oldSearch->end(); vertexIterator++ ) {
				const vertex_t searchVertex = *vertexIterator;
				const neighbor_set_t& neighbors = this->network.outNeighbors( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
					const vertex_t neighbor = neighborIterator->first;
					this->scores.at( neighbor )++;
					if( !found.at( neighbor ) ) {
						found.at( neighbor ) = true;
						newSearch->push_back( neighbor );
					}
				}
			}
		}
	
		delete newSearch;
		delete oldSearch;
	}
	return this->scores.at( neighbor );
}
