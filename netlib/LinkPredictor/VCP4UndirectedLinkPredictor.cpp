/*
------------------------------------------------
Copyright (C) 2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <iostream>
#include <set>
#include <list>
#include <utility>
#include "VCP4UndirectedLinkPredictor.h"

using std::list;
using std::set;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;

const size_t VCP4UndirectedLinkPredictor::map[graphlets] = {0,1,1,2,3,4,5,6,3,5,4,6,7,8,8,9,10,11,11,12,13,14,15,16,13,15,14,16,17,18,18,19,20,21,21,22,23,24,25,26,23,25,24,26,27,28,28,29,30,31,31,32,33,34,35,36,33,35,34,36,37,38,38,39};

VCP4UndirectedLinkPredictor::VCP4UndirectedLinkPredictor( const WeightedNetwork& network ) : LinkPredictor( network ), connectedPairs( 0 ), unconnectedPairs( 0 ) {
	// compute the total number of somehow-connected pairs in the graph
	for( vertex_t i = 0; i < network.vertexCount(); ++i ) {
		neighbor_set_t::const_iterator outIt = network.outNeighbors( i ).begin();
		neighbor_set_t::const_iterator outEnd = network.outNeighbors( i ).end();
		neighbor_set_t::const_iterator inIt = network.inNeighbors( i ).begin();
		neighbor_set_t::const_iterator inEnd = network.inNeighbors( i ).end();
		while( outIt != outEnd && outIt->first <= i ) {
			++outIt;
		}
		while( inIt != inEnd && inIt->first <= i ) {
			++inIt;
		}
		while( outIt != outEnd && inIt != inEnd ) {
			++connectedPairs;
			if( outIt->first < inIt->first ) {
				++outIt;
			} else if( outIt->first > inIt->first ) {
				++inIt;
			} else {
				++outIt;
				++inIt;
			}
		}
		while( outIt != outEnd ) {
			++connectedPairs;
			++outIt;
		}
		while( inIt != inEnd ) {
			++connectedPairs;
			++inIt;
		}
	}
	unsigned long potentialConnections = (unsigned long)network.vertexCount() * (unsigned long)(network.vertexCount() - 1) / 2;
	unconnectedPairs = potentialConnections - connectedPairs;
}

VCP4UndirectedLinkPredictor::~VCP4UndirectedLinkPredictor() {
}

double VCP4UndirectedLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	vertex_t v1 = vertex;
	vertex_t v2 = neighbor;
	unsigned long remapped[elements] = {0};
	size_t index = 32 * (size_t)network.hasEdge( v2, v1 );
	
	unsigned long vertices = 2;
	unsigned long connections = 0;
	unsigned long gaps = 0;
	
	// compose ordered list of v3 candidates
	const neighbor_set_t* v3Neighbors[4] = {&network.inNeighbors( v1 ), &network.outNeighbors( v1 ), &network.inNeighbors( v2 ), &network.outNeighbors( v2 )};
	list<vertex_t> v3Vertices;
	for( unsigned int i = 0; i < 4; ++i ) {
		list<vertex_t>::iterator v3VerticesIt = v3Vertices.begin();
		list<vertex_t>::iterator v3VerticesEnd = v3Vertices.end();
		neighbor_set_t::const_iterator neighborsIt = v3Neighbors[i]->begin();
		neighbor_set_t::const_iterator neighborsEnd = v3Neighbors[i]->end();
		while( v3VerticesIt != v3VerticesEnd && neighborsIt != neighborsEnd ) {
			if( neighborsIt->first == v1 || neighborsIt->first == v2 ) {
				++neighborsIt;
				continue;
			}
			if( *v3VerticesIt < neighborsIt->first ) {
				++v3VerticesIt;
			} else if( neighborsIt->first < *v3VerticesIt ) {
				v3Vertices.insert( v3VerticesIt, neighborsIt->first );
				++neighborsIt;
			} else {
				++v3VerticesIt;
				++neighborsIt;
			}
		}
		while( neighborsIt != neighborsEnd ) {
			if( neighborsIt->first != v1 && neighborsIt->first != v2 ) {
				v3Vertices.insert( v3VerticesIt, neighborsIt->first );
			}
			++neighborsIt;
		}
	}

	// for each v3 vertex in the above list
	for( list<vertex_t>::const_iterator v3It = v3Vertices.begin(); v3It != v3Vertices.end(); ++v3It ) {
		vertex_t v3 = *v3It;
		size_t index2 = index;
		if( network.hasEdge( v1, v3 ) || network.hasEdge( v3, v1 ) ) {
			index2 += 1;
			++connections;
		} else {
			++gaps;
		}
		if( network.hasEdge( v2, v3 ) || network.hasEdge( v3, v2 ) ) {
			index2 += 4;
			++connections;
		} else {
			++gaps;
		}

		// compose two ordered lists of v4 candidates: those from v3 and those ONLY from the the neighbors of this v3 vertex
		const neighbor_set_t* v4Neighbors[2] = {&network.inNeighbors( v3 ), &network.outNeighbors( v3 )};
		list<vertex_t> v4Vertices;
		for( unsigned int i = 0; i < 2; ++i ) {
			list<vertex_t>::iterator v4VerticesIt = v4Vertices.begin();
			list<vertex_t>::iterator v4VerticesEnd = v4Vertices.end();
			neighbor_set_t::const_iterator neighborsIt = v4Neighbors[i]->begin();
			neighbor_set_t::const_iterator neighborsEnd = v4Neighbors[i]->end();
			while( v4VerticesIt != v4VerticesEnd && neighborsIt != neighborsEnd ) {
				if( neighborsIt->first == v1 || neighborsIt->first == v2 || neighborsIt->first == v3 ) {
					++neighborsIt;
					continue;
				}
				if( *v4VerticesIt < neighborsIt->first ) {
					++v4VerticesIt;
				} else if( neighborsIt->first < *v4VerticesIt ) {
					v4Vertices.insert( v4VerticesIt, neighborsIt->first );
					++neighborsIt;
				} else {
					++v4VerticesIt;
					++neighborsIt;
				}
			}
			while( neighborsIt != neighborsEnd ) {
				if( neighborsIt->first != v1 && neighborsIt->first != v2 && neighborsIt->first != v3 ) {
					v4Vertices.insert( v4VerticesIt, neighborsIt->first );
				}
				++neighborsIt;
			}
		}
		list<vertex_t>::iterator v4VerticesIt = v4Vertices.begin();
		list<vertex_t>::iterator v4VerticesEnd = v4Vertices.end();
		list<vertex_t>::const_iterator v3VerticesIt = v3Vertices.begin();
		list<vertex_t>::const_iterator v3VerticesEnd = v3Vertices.end();
		while( v4VerticesIt != v4VerticesEnd && v3VerticesIt != v3VerticesEnd ) {
			if( *v4VerticesIt < *v3VerticesIt ) {
				++v4VerticesIt;
			} else if( *v3VerticesIt < *v4VerticesIt ) {
				++v3VerticesIt;
			} else {
				v4VerticesIt = v4Vertices.erase( v4VerticesIt );
			}
		}

		// for each v4 vertex in the above list of v4 only vertices
		vertices += v4Vertices.size();
		for( list<vertex_t>::const_iterator v4It = v4Vertices.begin(); v4It != v4Vertices.end(); ++v4It ) {
			vertex_t v4 = *v4It;
			size_t index3 = index2;
			if( network.hasEdge( v1, v4 ) || network.hasEdge( v4, v1 ) ) {
				index3 += 2;
				++connections;
			} else {
				++gaps;
			}
			if( network.hasEdge( v2, v4 ) || network.hasEdge( v4, v2 ) ) {
				index3 += 8;
				++connections;
			} else {
				++gaps;
			}
			if( network.hasEdge( v3, v4 ) || network.hasEdge( v4, v3 ) ) {
				index3 += 16;
				++connections;
			} else {
				++gaps;
			}
			++remapped[ map[index3] ];
		}
		
		// for each other v3 vertex if it has a higher index
		for( list<vertex_t>::const_iterator v4It = v3It; v4It != v3Vertices.end(); ++v4It ) {
			vertex_t v4 = *v4It;
			if( v3 < v4 ) {
				size_t index3 = index2;
				if( network.hasEdge( v1, v4 ) || network.hasEdge( v4, v1 ) ) {
					index3 += 2;
				}
				if( network.hasEdge( v2, v4 ) || network.hasEdge( v4, v2 ) ) {
					index3 += 8;
				}
				if( network.hasEdge( v3, v4 ) || network.hasEdge( v4, v3 ) ) {
					index3 += 16;
					++connections;
				} else {
					++gaps;
				}
				++remapped[ map[index3] ];
			}
		}
		
		// account for triangles and gaps
		remapped[ map[index2] ] += network.vertexCount() - 2 - (v3Vertices.size() + v4Vertices.size());
		remapped[ map[index] ] -= network.vertexCount() - 2 - (v3Vertices.size() + v4Vertices.size()); // go negative for the number of triangles (which contain v3v4 gaps)
	}
	vertices += v3Vertices.size();
	
	// account for the least connected substructures
	remapped[ map[16+index] ] = this->connectedPairs - (connections + index/32);
	remapped[ map[index] ] += this->unconnectedPairs - (gaps + !(bool)(index/32)) - (2 * (network.vertexCount() - vertices));

	// do the remapping and output
	cerr << network.translateIntToExt( vertex ) << " " << network.translateIntToExt( neighbor );
	for( size_t i = 0; i < elements; ++i ) {
		cerr << " " << remapped[i];
	}
	cerr << "\n";
	return -1;
}
