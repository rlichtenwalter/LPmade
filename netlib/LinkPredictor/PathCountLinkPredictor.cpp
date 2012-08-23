/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <stack>
#include <utility>
#include "PathCountLinkPredictor.h"

using std::stack;
using std::make_pair;

PathCountLinkPredictor::PathCountLinkPredictor( const WeightedNetwork& network, unsigned int l ) : LinkPredictor(network), l(l) {
}

PathCountLinkPredictor::~PathCountLinkPredictor() {
}

double PathCountLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	if( this->vertex != vertex ) {
		this->vertex = vertex;
		this->scores = vector<double>( this->network.vertexCount() );

		stack<pair<vertex_t,unsigned int> > search = stack<pair<vertex_t,unsigned int> >();
		search.push( make_pair( vertex, 0 ) );		
		while( !search.empty() ) {
			const pair<vertex_t,unsigned int>& searchItem = search.top();
			vertex_t searchVertex = searchItem.first;
			unsigned int distance = searchItem.second;
			search.pop();

			this->scores.at( searchVertex )++;
			if( distance < l ) {
				const neighbor_set_t& neighbors = this->network.outNeighbors( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
					vertex_t neighbor = neighborIterator->first;
					search.push( make_pair( neighbor, distance+1 ) );
				}
			}
		}
	}
	return this->scores.at( neighbor );
}
