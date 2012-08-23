/*
------------------------------------------------
Copyright (C) 2010,2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <cassert>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stack>
#include <map>
#include <stdexcept>
#include <limits.h>
#include <limits>
#include <cstdlib>
#include <stddef.h>

#include "WeightedNetwork.h"
#include "Statistics.h"

using std::map;
using std::make_pair;
using std::runtime_error;
using std::cout;
using std::queue;
using std::stack;
using std::cerr;
using std::streamsize;
using std::numeric_limits;

ptrdiff_t randomNumberGenerator (ptrdiff_t i) { return rand()%i; }
ptrdiff_t (*randomNumberGeneratorPtr)(ptrdiff_t) = randomNumberGenerator;

bool neighborCompare( const neighbor_t& neighbor1, const neighbor_t& neighbor2 ) {
	return neighbor1.first < neighbor2.first;
}

void WeightedNetwork::compact() {
/*
	unsigned int capacity = 0;
	unsigned int size = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		capacity += sizeof(neighbor_t) * ( this->outList.at( vertex ).capacity() + this->inList.at( vertex ).capacity() );
		size += sizeof(neighbor_t) * ( this->outList.at( vertex ).size() + this->inList.at( vertex ).size() );
	}
	cerr << "Old capacity: " << capacity << " - Old size: " << size << "\n";
*/
	
	vector<vertex_t> tempDictionary;
	tempDictionary.reserve( this->vertexCount() );
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		tempDictionary.push_back( this->dictionary.at( vertex ) );
		
		neighbor_set_t& outNeighbors = this->outList.at( vertex );
		neighbor_set_t tempOutNeighbors = neighbor_set_t();
		tempOutNeighbors.reserve( outNeighbors.size() );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
			tempOutNeighbors.push_back( *neighborIterator );
		}
		outNeighbors.swap( tempOutNeighbors );
		
		neighbor_set_t& inNeighbors = this->inList.at( vertex );
		neighbor_set_t tempInNeighbors = neighbor_set_t();
		tempInNeighbors.reserve( inNeighbors.size() );
		for( neighbor_set_t::const_iterator neighborIterator = inNeighbors.begin(); neighborIterator != inNeighbors.end(); neighborIterator++ ) {
			tempInNeighbors.push_back( *neighborIterator );
		}
		inNeighbors.swap( tempInNeighbors );
	}
	this->dictionary.swap( tempDictionary );
/*
	capacity = 0;
	size = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		capacity += sizeof(neighbor_t) * ( this->outList.at( vertex ).capacity() + this->inList.at( vertex ).capacity() );
		size += sizeof(neighbor_t) * ( this->outList.at( vertex ).size() + this->inList.at( vertex ).size() );
	}
	cerr << "New capacity: " << capacity << " - New size: " << size << "\n";
*/
}

inline void WeightedNetwork::addEdge( vertex_t vertex, vertex_t neighbor, weight_t weight ) {
	assert( this->outList.at( vertex ).size() == 0 || this->outList.at( vertex ).back().first < neighbor );
	assert( this->inList.at( neighbor ).size() == 0 || this->inList.at( neighbor ).back().first < vertex );
	this->outList.at( vertex ).push_back( make_pair( neighbor, weight ) );
	this->inList.at( neighbor ).push_back( make_pair( vertex, weight ) );
	this->numEdges++;
}	

neighbor_set_t WeightedNetwork::intersect( const neighbor_set_t& set1, const neighbor_set_t& set2 ) {
	neighbor_set_t::const_iterator it1 = set1.begin();
	neighbor_set_t::const_iterator it2 = set2.begin();
	
	neighbor_set_t result = neighbor_set_t();
	while( it1 != set1.end() && it2 != set2.end() ) {
		vertex_t v1 = it1->first;
		vertex_t v2 = it2->first;
		if( v1 < v2 ) {
	    	++it1;
		} else if( v1 > v2 ) {
	    	++it2;
		} else {
	    	result.push_back( *it1 );
	    	++it1;
	    	++it2;
		}
	}
	return result;
}

WeightedNetwork::WeightedNetwork() : dictionary(), outList(), inList(), numEdges(0) {
}

WeightedNetwork::WeightedNetwork( const WeightedNetwork& network ) : dictionary(network.dictionary), outList(network.outList), inList(network.inList), numEdges(network.numEdges) {
}

WeightedNetwork::~WeightedNetwork() {
}

WeightedNetwork& WeightedNetwork::operator=( const WeightedNetwork& network ) {
	this->dictionary = network.dictionary;
	this->outList = network.outList;
	this->inList = network.inList;
	this->numEdges = network.numEdges;
	
	return *this;
}

WeightedNetwork WeightedNetwork::readStream( istream& is, const char separator, unsigned int iIndex, unsigned int jIndex, bool ignoreSelfEdges ) {
	iIndex--;
	jIndex--;
	unsigned int maxIndex = jIndex;
	if( iIndex > jIndex ) {
		maxIndex = iIndex;
	}
	
	map<vertex_t,map<vertex_t,weight_t> > tempNetwork = map<vertex_t,map<vertex_t,weight_t> >();
	
	while( !is.eof() ) {
		vertex_t vertex = 0;
		vertex_t neighbor = 0;
		for( unsigned int index = 0; index <= maxIndex; index++ ) {
			if( index == iIndex ) {
				is >> vertex;
			} else if( index == jIndex ) {
				is >> neighbor;
			}
			if( index < maxIndex ) {
				is.ignore( numeric_limits<streamsize>::max(), separator );
			}
		}
		is.ignore( numeric_limits<streamsize>::max(), '\n' );
		is.peek();
		
		pair<map<vertex_t,map<vertex_t,weight_t> >::iterator,bool> infoPair = tempNetwork.insert( make_pair( vertex, map<vertex_t,weight_t>() ) );
		map<vertex_t,map<vertex_t,weight_t> >::iterator vertexLocation = infoPair.first;
		
		infoPair = tempNetwork.insert( make_pair( neighbor, map<vertex_t,weight_t>() ) );
//		map<vertex_t,map<vertex_t,weight_t> >::iterator neighborLocation = infoPair.first;
		
		if( vertex != neighbor || !ignoreSelfEdges ) {
			pair<map<vertex_t,weight_t>::iterator,bool> neighborInfoPair = vertexLocation->second.insert( make_pair( neighbor, 0 ) );
			neighborInfoPair.first->second++;
		}
	}
	
	WeightedNetwork network;
	network.dictionary.reserve( tempNetwork.size() );
	network.outList = vertex_set_t( tempNetwork.size() );
	network.inList = vertex_set_t( tempNetwork.size() );
	for( map<vertex_t,map<vertex_t,weight_t> >::const_iterator tempIt = tempNetwork.begin(); tempIt != tempNetwork.end(); tempIt++ ) {
		vertex_t name = tempIt->first;
		network.dictionary.push_back( name );
	}
	vertex_t vertex = 0;
	for( map<vertex_t,map<vertex_t,weight_t> >::const_iterator tempIt = tempNetwork.begin(); tempIt != tempNetwork.end(); tempIt++, vertex++ ) {
		const map<vertex_t,weight_t>& neighborMap = tempIt->second;
		for( map<vertex_t,weight_t>::const_iterator neighborIterator = neighborMap.begin(); neighborIterator != neighborMap.end(); neighborIterator++ ) {
			vertex_t neighbor = neighborIterator->first;
			weight_t weight = neighborIterator->second;
			network.addEdge( vertex, network.translateExtToInt( neighbor ), weight );
		}
	}

	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::readWeightedStream( istream& is, const char separator, unsigned int iIndex, unsigned int jIndex, unsigned int wIndex, bool ignoreSelfEdges ) {
	iIndex--;
	jIndex--;
	wIndex--;
	unsigned int maxIndex = iIndex;
	maxIndex = std::max( maxIndex, jIndex );
	maxIndex = std::max( maxIndex, wIndex );
	
	map<vertex_t,map<vertex_t,weight_t> > tempNetwork = map<vertex_t,map<vertex_t,weight_t> >();
	
	while( !is.eof() ) {
		vertex_t vertex = 0;
		vertex_t neighbor = 0;
		weight_t weight = 0;
		for( unsigned int index = 0; index <= maxIndex; index++ ) {
			if( index == iIndex ) {
				is >> vertex;
			} else if( index == jIndex ) {
				is >> neighbor;
			} else if( index == wIndex ) {
				is >> weight;
			}
			if( index < maxIndex ) {
				is.ignore( numeric_limits<streamsize>::max(), separator );
			}
		}
		is.ignore( numeric_limits<streamsize>::max(), '\n' );
		is.peek();
		
		pair<map<vertex_t,map<vertex_t,weight_t> >::iterator,bool> infoPair = tempNetwork.insert( make_pair( vertex, map<vertex_t,weight_t>() ) );
		map<vertex_t,map<vertex_t,weight_t> >::iterator vertexLocation = infoPair.first;
		
		infoPair = tempNetwork.insert( make_pair( neighbor, map<vertex_t,weight_t>() ) );
//		map<vertex_t,map<vertex_t,weight_t> >::iterator neighborLocation = infoPair.first;
		
		if( vertex != neighbor || !ignoreSelfEdges ) {
			pair<map<vertex_t,weight_t>::iterator,bool> neighborInfoPair = vertexLocation->second.insert( make_pair( neighbor, 0 ) );
			neighborInfoPair.first->second += weight;
		}
	}
	
	WeightedNetwork network;
	network.dictionary.reserve( tempNetwork.size() );
	network.outList = vertex_set_t( tempNetwork.size() );
	network.inList = vertex_set_t( tempNetwork.size() );
	for( map<vertex_t,map<vertex_t,weight_t> >::const_iterator tempIt = tempNetwork.begin(); tempIt != tempNetwork.end(); tempIt++ ) {
		vertex_t name = tempIt->first;
		network.dictionary.push_back( name );
	}
	vertex_t vertex = 0;
	for( map<vertex_t,map<vertex_t,weight_t> >::const_iterator tempIt = tempNetwork.begin(); tempIt != tempNetwork.end(); tempIt++, vertex++ ) {
		const map<vertex_t,weight_t>& neighborMap = tempIt->second;
		for( map<vertex_t,weight_t>::const_iterator neighborIterator = neighborMap.begin(); neighborIterator != neighborMap.end(); neighborIterator++ ) {
			vertex_t neighbor = neighborIterator->first;
			weight_t weight = neighborIterator->second;
			network.addEdge( vertex, network.translateExtToInt( neighbor ), weight );
		}
	}

	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::readNetwork( istream& is ) {
	WeightedNetwork network = WeightedNetwork();
	is.ignore( numeric_limits<streamsize>::max(), ' ' );
	vertex_t vertexCount = 0;
	is >> vertexCount;
	network.dictionary.reserve( vertexCount );
	network.outList = vertex_set_t( vertexCount );
	network.inList = vertex_set_t( vertexCount );
	for( vertex_t vertex = 0; vertex < vertexCount; vertex++ ) {
		vertex_t index;
		vertex_t name;
		is >> index >> name;
		network.dictionary.push_back( name );
	}
	is.ignore( numeric_limits<streamsize>::max(), ' ' );
	unsigned int edgeCount = 0;
	is >> edgeCount;
	for( unsigned int edge = 0; edge < edgeCount; edge++ ) {
		vertex_t index1;
		vertex_t index2;
		weight_t weight;
		is >> index1 >> index2 >> weight;
		network.addEdge( index1, index2, weight );
	}
	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::combine( const WeightedNetwork& network1, const WeightedNetwork& network2 ) {
	WeightedNetwork network = WeightedNetwork();
	
	// handle joining dictionaries
	vector<vertex_t>::const_iterator it1 = network1.dictionary.begin();
	vector<vertex_t>::const_iterator it2 = network2.dictionary.begin();
	
	while( it1 != network1.dictionary.end() || it2 != network2.dictionary.end() ) {
		if( it1 == network1.dictionary.end() ) {
			network.dictionary.push_back( *it2 );
			it2++;
		} else if( it2 == network2.dictionary.end() ) {
			network.dictionary.push_back( *it1 );
			it1++;
		} else if( *it1 > *it2 ) {
			network.dictionary.push_back( *it2 );
			it2++;
		} else if( *it1 < *it2 ) {
			network.dictionary.push_back( *it1 );
			it1++;
		} else {
			network.dictionary.push_back( *it1 );
			it1++;
			it2++;
		}
	}
		
	// handle joining lists
	network.outList = vertex_set_t( network.dictionary.size() );
	network.inList = vertex_set_t( network.dictionary.size() );
	for( vertex_t vertex = 0; vertex < network.dictionary.size(); vertex++ ) {
		vertex_t name = network.translateIntToExt( vertex );
		vertex_t index1 = network1.translateExtToInt( name );
		vertex_t index2 = network2.translateExtToInt( name );
		
		if( index1 == INVALID_VERTEX ) {
			const neighbor_set_t& outNeighbors2 = network2.outList.at( index2 );
			for( neighbor_set_t::const_iterator neighborIterator = outNeighbors2.begin(); neighborIterator != outNeighbors2.end(); neighborIterator++ ) {
				network.addEdge( vertex, network.translateExtToInt( network2.translateIntToExt( neighborIterator->first ) ), neighborIterator->second );
			}
		} else if( index2 == INVALID_VERTEX ) {
			const neighbor_set_t& outNeighbors1 = network1.outList.at( index1 );
			for( neighbor_set_t::const_iterator neighborIterator = outNeighbors1.begin(); neighborIterator != outNeighbors1.end(); neighborIterator++ ) {
				network.addEdge( vertex, network.translateExtToInt( network1.translateIntToExt( neighborIterator->first ) ), neighborIterator->second );
			}
		} else {
			const neighbor_set_t& outNeighbors1 = network1.outList.at( index1 );
			const neighbor_set_t& outNeighbors2 = network2.outList.at( index2 );
			neighbor_set_t::const_iterator it1 = outNeighbors1.begin();
			neighbor_set_t::const_iterator it2 = outNeighbors2.begin();
			while( it1 != outNeighbors1.end() || it2 != outNeighbors2.end() ) {
				vertex_t neighborName1 = 0;
				vertex_t neighborName2 = 0;
				if( it1 != outNeighbors1.end() ) {
					neighborName1 = network1.translateIntToExt( it1->first );
				}
				if( it2 != outNeighbors2.end() ) {
					neighborName2 = network2.translateIntToExt( it2->first );
				}
				if( it1 == outNeighbors1.end() ) {
					network.addEdge( vertex, network.translateExtToInt( neighborName2 ), it2->second );
					it2++;
				} else if( it2 == outNeighbors2.end() ) {
					network.addEdge( vertex, network.translateExtToInt( neighborName1 ), it1->second );
					it1++;
				} else if( neighborName1 > neighborName2 ) {
					network.addEdge( vertex, network.translateExtToInt( neighborName2 ), it2->second );
					it2++;
				} else if( neighborName1 < neighborName2 ) {
					network.addEdge( vertex, network.translateExtToInt( neighborName1 ), it1->second );
					it1++;
				} else {
					network.addEdge( vertex, network.translateExtToInt( neighborName1 ), it1->second + it2->second );
					it1++;
					it2++;
				}
			}
		}
	}
	network.compact();
	return network;
}

neighbor_set_t WeightedNetwork::commonOutNeighbors( vertex_t vertex, vertex_t neighbor ) const {
	const neighbor_set_t& vertexNeighbors = this->outNeighbors( vertex );
	const neighbor_set_t& neighborNeighbors = this->outNeighbors( neighbor );
	return WeightedNetwork::intersect( vertexNeighbors, neighborNeighbors );
}

neighbor_set_t WeightedNetwork::commonInNeighbors( vertex_t vertex, vertex_t neighbor ) const {
	const neighbor_set_t& vertexNeighbors = this->inNeighbors( vertex );
	const neighbor_set_t& neighborNeighbors = this->inNeighbors( neighbor );
	return WeightedNetwork::intersect( vertexNeighbors, neighborNeighbors );
}

void WeightedNetwork::print( ostream& os ) const {
	os << "*Vertices " << this->vertexCount() << "\n";
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		os << vertex << " " << this->dictionary.at( vertex ) << "\n";
	}
	os << "*Edges " << this->edgeCount() << "\n";
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		const neighbor_set_t& outNeighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
			vertex_t neighbor = neighborIterator->first;
			weight_t weight = neighborIterator->second;
			os << vertex << " " << neighbor << " " << weight << "\n";
		}
	}
}

void WeightedNetwork::printEdgeList( ostream& os ) const {
	for( vertex_t vertex = 0; vertex < this->vertexCount(); ++vertex ) {
		const neighbor_set_t& outNeighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); ++neighborIterator ) {
			os << this->translateIntToExt( vertex ) << " " << this->translateIntToExt( neighborIterator->first ) << "\n";

		}
	}
}

void WeightedNetwork::printAdjacencyList( ostream& os ) const {
	for( vertex_t vertex = 0; vertex < this->vertexCount(); ++vertex ) {
		os << this->translateIntToExt( vertex );
		const neighbor_set_t& outNeighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); ++neighborIterator ) {
			os << " " << this->translateIntToExt( neighborIterator->first ) << "," << neighborIterator->second;
		}
		os << "\n";
	}
}

unsigned int WeightedNetwork::vertexCount() const {
	return this->dictionary.size();
}

unsigned int WeightedNetwork::edgeCount() const {
	return this->numEdges;
}

const neighbor_set_t& WeightedNetwork::outNeighbors( vertex_t vertex ) const {
	return this->outList.at( vertex );
}

const neighbor_set_t& WeightedNetwork::inNeighbors( vertex_t vertex ) const {
	return this->inList.at( vertex );
}

const vector<vertex_t> WeightedNetwork::findOutNeighbors( vertex_t vertex, unsigned int degree ) const {
	vector<bool> found = vector<bool>( this->vertexCount() );

	vector<vertex_t>* oldSearch = new vector<vertex_t>();
	vector<vertex_t>* newSearch = new vector<vertex_t>();
	
	found.at( vertex ) = true;
	newSearch->push_back( vertex );
	for( unsigned int currentDegree = 0; currentDegree < degree; currentDegree++ ) {
		delete oldSearch;
		oldSearch = newSearch;
		newSearch = new vector<vertex_t>();
		for( vector<vertex_t>::const_iterator searchIterator = oldSearch->begin(); searchIterator != oldSearch->end(); searchIterator++ ) {
			const vertex_t searchVertex = *searchIterator;
			const neighbor_set_t& neighbors = this->outList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
				const vertex_t neighborVertex = neighborIterator->first;
				if( !found.at( neighborVertex ) ) {
					found.at( neighborVertex ) = true;
					newSearch->push_back( neighborVertex );
				}
			}
		}
	}
	
	vector<vertex_t> results = vector<vertex_t>( newSearch->size() );
	for( unsigned int searchVertex = 0; searchVertex < newSearch->size(); searchVertex++ ) {
		results.at( searchVertex ) = newSearch->at( searchVertex );
	}
	
	delete newSearch;
	delete oldSearch;

	return results;
}

const vector<vertex_t> WeightedNetwork::findInNeighbors( vertex_t vertex, unsigned int degree ) const {
	vector<bool> found = vector<bool>( this->vertexCount() );

	vector<vertex_t>* oldSearch = new vector<vertex_t>();
	vector<vertex_t>* newSearch = new vector<vertex_t>();
	
	found.at( vertex ) = true;
	newSearch->push_back( vertex );
	for( unsigned int currentDegree = 0; currentDegree < degree; currentDegree++ ) {
		delete oldSearch;
		oldSearch = newSearch;
		newSearch = new vector<vertex_t>();
		for( vector<vertex_t>::const_iterator searchIterator = oldSearch->begin(); searchIterator != oldSearch->end(); searchIterator++ ) {
			const vertex_t searchVertex = *searchIterator;
			const neighbor_set_t& neighbors = this->inList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
				const vertex_t neighborVertex = neighborIterator->first;
				if( !found.at( neighborVertex ) ) {
					found.at( neighborVertex ) = true;
					newSearch->push_back( neighborVertex );
				}
			}
		}
	}
	
	vector<vertex_t> results = vector<vertex_t>( newSearch->size() );
	for( unsigned int searchVertex = 0; searchVertex < newSearch->size(); searchVertex++ ) {
		results.at( searchVertex ) = newSearch->at( searchVertex );
	}
	
	delete newSearch;
	delete oldSearch;

	return results;
}

WeightedNetwork WeightedNetwork::thresholdEdges( weight_t threshold ) const {
	WeightedNetwork network = WeightedNetwork();
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		const neighbor_set_t& outNeighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
			const vertex_t outNeighbor = neighborIterator->first;
			const weight_t weight = neighborIterator->second;
			if( weight >= threshold ) {
				network.addEdge( vertex, outNeighbor, weight );
			}
		}
	}
	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::snowballSample( vertex_t vertex, unsigned int degree ) const {
	WeightedNetwork network;
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );

	vector<bool> found( this->vertexCount() );
	
	vector<vertex_t> search;
	
	found.at( vertex ) = true;
	search.push_back( vertex );
	for( unsigned int currentDegree = 0; currentDegree < degree; currentDegree++ ) {
		vector<vertex_t> newSearch;
		for( vector<vertex_t>::const_iterator vertexIterator = search.begin(); vertexIterator != search.end(); vertexIterator++ ) {
			const vertex_t searchVertex = *vertexIterator;
			const neighbor_set_t& neighbors = this->outList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
				const vertex_t neighbor = neighborIterator->first;
				const weight_t weight = neighborIterator->second;
				network.addEdge( searchVertex, neighbor, weight );
				if( !found.at( neighbor ) ) {
					found.at( neighbor ) = true;
					newSearch.push_back( neighbor );
				}
			}
		}
		search.swap( newSearch );
	}
	
	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::snowballSampleIn( vertex_t vertex, unsigned int degree ) const {
	WeightedNetwork network;
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );

	vector<bool> found( this->vertexCount() );
	
	vector<vertex_t> search;
	
	found.at( vertex ) = true;
	search.push_back( vertex );
	for( unsigned int currentDegree = 0; currentDegree < degree; currentDegree++ ) {
		vector<vertex_t> newSearch;
		for( vector<vertex_t>::const_iterator vertexIterator = search.begin(); vertexIterator != search.end(); vertexIterator++ ) {
			const vertex_t searchVertex = *vertexIterator;
			const neighbor_set_t& neighbors = this->inList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
				const vertex_t neighbor = neighborIterator->first;
				const weight_t weight = neighborIterator->second;
				network.addEdge( neighbor, searchVertex, weight );
				if( !found.at( neighbor ) ) {
					found.at( neighbor ) = true;
					newSearch.push_back( neighbor );
				}
			}
		}
		search.swap( newSearch );
	}
	
	network.compact();
	return network;
}


WeightedNetwork WeightedNetwork::randomEdgeSample( double percentEdges, unsigned int seed ) const {	
	WeightedNetwork network = WeightedNetwork();
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );
	network.numEdges = 0;
	
	vector<unsigned int> edgeIndices = vector<unsigned int>();
	edgeIndices.reserve( this->numEdges );
	for( unsigned int index = 0; index < this->edgeCount(); index++ ) {
		edgeIndices.push_back( index );
	}
	srand( seed );
	random_shuffle( edgeIndices.begin(), edgeIndices.end(), randomNumberGeneratorPtr );
	percentEdges = (100 - percentEdges) / 100.0;
	unsigned int deletionCount = (unsigned int)(percentEdges*this->numEdges);
	edgeIndices.resize( deletionCount );
	sort( edgeIndices.begin(), edgeIndices.end() );

	unsigned int edgeIndex = 0;
	unsigned int deletionIndex = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		const neighbor_set_t& neighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
			if( deletionIndex < edgeIndices.size() && edgeIndex == edgeIndices.at(deletionIndex) ) {
					deletionIndex++;
			} else {
				const vertex_t neighbor = neighborIterator->first;
				const weight_t weight = neighborIterator->second;
				network.addEdge( vertex, neighbor, weight );
			}
			edgeIndex++;
		}
	}

	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::removeIsolates() const {
	WeightedNetwork network = WeightedNetwork();
	
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		if( this->outDegree( vertex ) > 0 || this->inDegree( vertex ) > 0 ) {
			network.dictionary.push_back( this->translateIntToExt( vertex ) );
		}
	}
	// network.vertexCount() is now safely up to date since the dictionary has been populated
	network.outList = vertex_set_t( network.vertexCount() );
	network.inList = vertex_set_t( network.vertexCount() );
	for( vertex_t vertex = 0; vertex < network.vertexCount(); vertex++ ) {
		const vertex_t name = this->translateIntToExt( vertex );
		const vertex_t oldIndex = this->translateExtToInt( name );

		const neighbor_set_t& outNeighbors = this->outList.at( oldIndex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
			const vertex_t outNeighbor = neighborIterator->first;
			const weight_t weight = neighborIterator->second;
			network.addEdge( vertex, network.translateExtToInt( this->translateIntToExt( outNeighbor ) ), weight );
		}
	}
	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::undirected() const {
	WeightedNetwork network = WeightedNetwork();
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );
	network.numEdges = 0;
	
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		const neighbor_set_t& neighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
			const vertex_t neighbor = neighborIterator->first;
			const weight_t weight = neighborIterator->second;
			network.addEdge( vertex, neighbor, weight );
			if( !this->hasEdge( neighbor, vertex ) ) {
				network.addEdge( neighbor, vertex, weight );
			}
		}
	}
	network.compact();
	return network;

}

WeightedNetwork WeightedNetwork::reverseEdges() const {
	WeightedNetwork network = WeightedNetwork();
	network.dictionary = this->dictionary;
	network.outList = this->inList;
	network.inList = this->outList;
	network.numEdges = this->numEdges;
	return network;
}

WeightedNetwork WeightedNetwork::removeEdges( const WeightedNetwork& network ) const {
	WeightedNetwork newNetwork;
	newNetwork.dictionary = this->dictionary;
	newNetwork.outList = vertex_set_t( this->vertexCount() );
	newNetwork.inList = vertex_set_t( this->vertexCount() );
	newNetwork.numEdges = 0;
	
	for( vertex_t vertex = 0; vertex < this->vertexCount(); ++vertex ) {
		const neighbor_set_t& neighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
			const vertex_t neighbor = neighborIterator->first;
			const weight_t weight = neighborIterator->second;
			if( !network.hasEdgeExt( this->translateIntToExt( vertex ), this->translateIntToExt( neighbor ) ) ) {
				newNetwork.addEdge( vertex, neighbor, weight );
			}
		}
	}
	newNetwork.compact();
	return newNetwork;
}

WeightedNetwork WeightedNetwork::removeEdges( const vector<edge_t>& edgesToRemove ) const {
	vector<edge_t> edgeList = edgesToRemove;
	sort( edgeList.begin(), edgeList.end() );
	
	WeightedNetwork network;
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );
	network.numEdges = 0;
	
	for( vertex_t vertex = 0; vertex < this->vertexCount(); ++vertex ) {
		const neighbor_set_t& neighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
			const vertex_t neighbor = neighborIterator->first;
			const weight_t weight = neighborIterator->second;
			if( !binary_search( edgeList.begin(), edgeList.end(), make_pair( vertex, neighbor ) ) ) {
				network.addEdge( vertex, neighbor, weight );
			}
		}
	}
	network.compact();
	return network;
}

WeightedNetwork WeightedNetwork::removeAmutuals() const {
	WeightedNetwork network = WeightedNetwork();
	network.dictionary = this->dictionary;
	network.outList = vertex_set_t( this->vertexCount() );
	network.inList = vertex_set_t( this->vertexCount() );
	network.numEdges = 0;
	
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		const neighbor_set_t& neighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
			const vertex_t neighbor = neighborIterator->first;
			const weight_t weight = neighborIterator->second;
			if( this->hasEdge( neighbor, vertex ) ) {
				network.addEdge( vertex, neighbor, weight );
			}
		}
	}
	network.compact();
	return network;
}

bool WeightedNetwork::hasEdge( vertex_t vertex1, vertex_t vertex2 ) const {
	neighbor_t neighbor = make_pair( vertex2, 0 );
	const neighbor_set_t& neighbors = this->outList.at( vertex1 );
	return binary_search( neighbors.begin(), neighbors.end(), neighbor, neighborCompare );
}

weight_t WeightedNetwork::edgeWeight( vertex_t vertex1, vertex_t vertex2 ) const {
	neighbor_t neighbor = make_pair( vertex2, 0 );
	const neighbor_set_t& neighbors = this->outList.at( vertex1 );
	neighbor_set_t::const_iterator it = lower_bound( neighbors.begin(), neighbors.end(), neighbor );
	return it == neighbors.end() ? 0 : it->second;
}

vertex_t WeightedNetwork::maxOutDegreeVertex() const {
	vertex_t highestVertex = 0;
	unsigned int highestDegree = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		unsigned int vertexDegree = this->outDegree( vertex );
		if( vertexDegree > highestDegree ) {
			highestDegree = vertexDegree;
			highestVertex = vertex;
		}
	}
	return highestVertex;
}

vertex_t WeightedNetwork::translateExtToInt( vertex_t vertex ) const {
	vector<vertex_t>::const_iterator it = lower_bound( this->dictionary.begin(), this->dictionary.end(), vertex );
	if( it == this->dictionary.end() || *it != vertex ) {
		return INVALID_VERTEX;
	} else {
		return it - this->dictionary.begin();
	}
}

vertex_t WeightedNetwork::translateIntToExt( vertex_t vertex ) const {
	return this->dictionary.at( vertex );
}

bool WeightedNetwork::hasEdgeExt( vertex_t vertex1, vertex_t vertex2 ) const {
	vertex_t vertex = 0;
	vertex_t neighbor = 0;
	vector<vertex_t>::const_iterator it = lower_bound( this->dictionary.begin(), this->dictionary.end(), vertex1 );
	if( it == this->dictionary.end() || *it != vertex1 ) {
		return false;
	} else {
		vertex = it - this->dictionary.begin();
	}
	
	it = lower_bound( this->dictionary.begin(), this->dictionary.end(), vertex2 );
	if( it == this->dictionary.end() || *it != vertex2 ) {
		return false;
	} else {
		neighbor = it - this->dictionary.begin();
	}
	
	return this->hasEdge( vertex, neighbor );
}

weight_t WeightedNetwork::edgeWeightExt( vertex_t vertex1, vertex_t vertex2 ) const {
	vertex_t vertex = 0;
	vertex_t neighbor = 0;
	vector<vertex_t>::const_iterator it = lower_bound( this->dictionary.begin(), this->dictionary.end(), vertex1 );
	if( it == this->dictionary.end() || *it != vertex1 ) {
		return false;
	} else {
		vertex = it - this->dictionary.begin();
	}
	
	it = lower_bound( this->dictionary.begin(), this->dictionary.end(), vertex2 );
	if( it == this->dictionary.end() || *it != vertex2 ) {
		return false;
	} else {
		neighbor = it - this->dictionary.begin();
	}
	
	return this->edgeWeight( vertex, neighbor );
}

vector<WeightedNetwork> WeightedNetwork::wccs() const {
	vector<WeightedNetwork> wccs = vector<WeightedNetwork>();
	vector<bool> found = vector<bool>( this->vertexCount() );
	
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		if( !found.at( vertex ) ) {
			vector<vertex_t> vertexList;

			found.at( vertex ) = true;
			queue<vertex_t> search;
			search.push( vertex );
			while( !search.empty() ) {
				const vertex_t searchVertex = search.front();
				search.pop();
				vertexList.push_back( searchVertex );
				const neighbor_set_t& outNeighbors = this->outList.at( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
					const vertex_t neighborVertex = neighborIterator->first;
					if( !found.at( neighborVertex ) ) {
						found.at( neighborVertex ) = true;
						search.push( neighborVertex );
					}
				}
				const neighbor_set_t& inNeighbors = this->inList.at( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = inNeighbors.begin(); neighborIterator != inNeighbors.end(); neighborIterator++ ) {
					const vertex_t neighborVertex = neighborIterator->first;
					if( !found.at( neighborVertex ) ) {
						found.at( neighborVertex ) = true;
						search.push( neighborVertex );
					}
				}
			}
			
			WeightedNetwork wcc = WeightedNetwork();
			sort( vertexList.begin(), vertexList.end() );
			wcc.dictionary.reserve( vertexList.size() );
			for( vertex_t vListIndex = 0; vListIndex < vertexList.size(); vListIndex++ ) {
				wcc.dictionary.push_back( this->translateIntToExt( vertexList.at( vListIndex ) ) );
			}
			wcc.outList = vertex_set_t( vertexList.size() );
			wcc.inList = vertex_set_t( vertexList.size() );
			for( vertex_t vListIndex = 0; vListIndex < vertexList.size(); vListIndex++ ) {
				const neighbor_set_t& outNeighbors = this->outList.at( vertexList.at( vListIndex ) );
				for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
					const vertex_t neighbor = neighborIterator->first;
					const weight_t weight = neighborIterator->second;
					wcc.addEdge( vListIndex, wcc.translateExtToInt( this->translateIntToExt( neighbor ) ), weight );
				}
			}
			wcc.compact();
			wccs.push_back( wcc );
		}
	}

	return wccs;
}

vector<WeightedNetwork> WeightedNetwork::sccs() const {
	vector<WeightedNetwork> sccs = vector<WeightedNetwork>();

	unsigned int index = 1;
	stack<vertex_t> searchStack;
	vector<bool> onStack = vector<bool>( this->vertexCount() );
	vector<unsigned int> neighborIndices = vector<unsigned int>( this->vertexCount() );
	vector<unsigned int> indices = vector<unsigned int>( this->vertexCount() );
	vector<unsigned int> lowlinks = vector<unsigned int>( this->vertexCount() );
	vector<vertex_t> callers = vector<unsigned int>( this->vertexCount() );
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		if( indices.at( vertex ) == 0 ) { // not found
			indices.at( vertex ) = index;
			lowlinks.at( vertex ) = index;
			index++;
			callers.at( vertex ) = INVALID_VERTEX;
			searchStack.push( vertex );
			onStack.at( vertex ) = true;
			vertex_t searchVertex = vertex;
			while( true ) {
				unsigned int& neighborIndex = neighborIndices.at( searchVertex );
				if( neighborIndex < this->outDegree( searchVertex ) ) {
					const vertex_t neighbor = this->outList.at( searchVertex ).at( neighborIndex ).first;
					neighborIndex++;
					if( indices.at( neighbor ) == 0 ) { // not found
						indices.at( neighbor ) = index;
						lowlinks.at( neighbor ) = index;
						index++;
						callers.at( neighbor ) = searchVertex;
						searchStack.push( neighbor );
						onStack.at( neighbor ) = true;
						searchVertex = neighbor;
					} else if( onStack.at( neighbor ) == true ) {
						lowlinks.at( searchVertex ) = std::min( lowlinks.at( searchVertex ), indices.at( neighbor ) );
					}
				} else {
					if( lowlinks.at( searchVertex ) == indices.at( searchVertex ) ) {
						vector<vertex_t> vertexList;

						vertex_t sccMember = searchStack.top();
						vertexList.push_back( sccMember );
						onStack.at( sccMember ) = false;
						searchStack.pop();
						
						while( sccMember != searchVertex ) {
							sccMember = searchStack.top();
							vertexList.push_back( sccMember );
							onStack.at( sccMember ) = false;
							searchStack.pop();
						}			

						WeightedNetwork scc = WeightedNetwork();
						sort( vertexList.begin(), vertexList.end() );
						scc.dictionary.reserve( vertexList.size() );
						for( vertex_t vListIndex = 0; vListIndex < vertexList.size(); vListIndex++ ) {
							scc.dictionary.push_back( this->translateIntToExt( vertexList.at( vListIndex ) ) );
						}
						scc.outList = vertex_set_t( vertexList.size() );
						scc.inList = vertex_set_t( vertexList.size() );
						for( vertex_t vListIndex = 0; vListIndex < vertexList.size(); vListIndex++ ) {
							const neighbor_set_t& outNeighbors = this->outList.at( vertexList.at( vListIndex ) );
							for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
								const vertex_t neighbor = neighborIterator->first;
								const weight_t weight = neighborIterator->second;
								if( binary_search( vertexList.begin(), vertexList.end(), neighbor ) ) {
									scc.addEdge( vListIndex, scc.translateExtToInt( this->translateIntToExt( neighbor ) ), weight );
								}
							}
						}
						scc.compact();
						sccs.push_back( scc );
					}
					vertex_t caller = callers.at( searchVertex );
					if( caller != INVALID_VERTEX ) {
						lowlinks.at( caller ) = std::min( lowlinks.at( caller ), lowlinks.at( searchVertex ) );
						searchVertex = caller;
					} else {
						break;
					}
				}
			}
		}
	}
	return sccs;
}

unsigned int WeightedNetwork::pseudoDiameter() const {
	vertex_t root = this->maxOutDegreeVertex();
	vertex_t farthest = root;
	int distance = -1;
	int lastDistance = -2;
	while( distance > lastDistance ) {
		lastDistance = distance;
		distance = -1;
		root = farthest;
		vector<bool> found = vector<bool>( this->vertexCount() );
		found.at( root ) = true;
		
		vector<vertex_t> oldSearch;
		oldSearch.push_back( root );
		while( !oldSearch.empty() ) {
			++distance; // distance should start at 0; imagine if the wcc has only one node
			vector<vertex_t> newSearch;
			for( vector<vertex_t>::const_iterator vertexIterator = oldSearch.begin(); vertexIterator != oldSearch.end(); ++vertexIterator ) {
				const vertex_t searchVertex = *vertexIterator;
				const neighbor_set_t& neighbors = this->outList.at( searchVertex );
				for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
					const vertex_t neighborVertex = neighborIterator->first;
					if( !found.at( neighborVertex ) ) {
						found.at( neighborVertex ) = true;
						newSearch.push_back( neighborVertex );
						farthest = neighborVertex;
					}
				}
			}
			oldSearch.swap( newSearch );
		}
	}
	return lastDistance;
}

unsigned int WeightedNetwork::countMutuals() const {
	unsigned int count = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); ++vertex ) {
		const neighbor_set_t& neighbors = this->outList.at( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
			const vertex_t neighborVertex = neighborIterator->first;
			if( this->hasEdge( neighborVertex, vertex ) ) {
				++count;
			}
		}
	}
	return count / 2;
}

double WeightedNetwork::freeChoiceMutualityIndex() const {
	double g = (double)this->vertexCount();
	double M = (double)this->countMutuals();
	double L = (double)this->edgeCount();
	double L2 = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		L2 += pow( this->outDegree( vertex ), 2 );
	}
//	cerr << "g: " << g << " - M: " << M << " - L: " << L << " - L2: " << L2 << "\n";
	double numerator = 2*pow(g - 1, 2)*M - pow(L, 2) + L2;
	double denominator = L*pow(g - 1, 2) - pow(L, 2) + L2;
	return numerator / denominator;
}

unsigned int WeightedNetwork::outDegree( vertex_t vertex ) const {
	return this->outList.at( vertex ).size();
}

unsigned int WeightedNetwork::inDegree( vertex_t vertex ) const {
	return this->inList.at( vertex ).size();
}

weight_t WeightedNetwork::outVolume( vertex_t vertex ) const {
	weight_t volume = 0.0;
	const neighbor_set_t& neighbors = this->outList.at( vertex );
	for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
		volume += neighborIterator->second;
	}
	return volume;
}

weight_t WeightedNetwork::inVolume( vertex_t vertex ) const {
	weight_t volume = 0.0;
	const neighbor_set_t& neighbors = this->inList.at( vertex );
	for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
		volume += neighborIterator->second;
	}
	return volume;
}

unsigned int WeightedNetwork::eccentricity( vertex_t vertex ) const {
	vector<bool> found = vector<bool>( this->vertexCount() );
	
	vector<vertex_t>* oldSearch = new vector<vertex_t>();
	vector<vertex_t>* newSearch = new vector<vertex_t>();
	
	found.at( vertex ) = true;
	newSearch->push_back( vertex );
	unsigned int currentDegree = -1;
	while( !newSearch->empty() ) {
		delete oldSearch;
		oldSearch = newSearch;
		newSearch = new vector<vertex_t>();
		for( vector<vertex_t>::const_iterator searchIterator = oldSearch->begin(); searchIterator != oldSearch->end(); searchIterator++ ) {
			const vertex_t searchVertex = *searchIterator;
			const neighbor_set_t& neighbors = this->outList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
				const vertex_t neighborVertex = neighborIterator->first;
				if( !found.at( neighborVertex ) ) {
					found.at( neighborVertex ) = true;
					newSearch->push_back( neighborIterator->first );
				}
			}
		}
		currentDegree++;
	}
	
	delete newSearch;
	delete oldSearch;

	return currentDegree;
}

double WeightedNetwork::clusteringCoefficient( vertex_t vertex ) const {
	unsigned int edgeCount = 0;
	const neighbor_set_t& neighbors = this->outList.at( vertex );
	if( neighbors.size() < 2 ) {
		return 0.0;
	}
	for( unsigned int index1 = 0; index1 < neighbors.size(); index1++ ) {
		const vertex_t neighbor1 = neighbors.at( index1 ).first;
		for( unsigned int index2 = index1 + 1; index2 < neighbors.size(); index2++ ) {
			const vertex_t neighbor2 = neighbors.at( index2 ).first;
			bool edge1 = this->hasEdge( neighbor1, neighbor2 );
			bool edge2 = this->hasEdge( neighbor2, neighbor1 );
			edgeCount += (unsigned int)edge1 + (unsigned int)edge2;
		}
	}
	
	return (double)edgeCount / (neighbors.size()*(neighbors.size()-1));
}

double WeightedNetwork::clusteringCoefficient() const {
	double sum = 0;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		sum += this->clusteringCoefficient( vertex );
	}
	return sum / this->vertexCount();
}

vector<double> WeightedNetwork::pageRank( double d ) const {
	vector<double> oldPageRanks = vector<double>( this->vertexCount() );
	vector<double>* pageRanks = new vector<double>( this->vertexCount() );
	vector<double> scaledPageRanks = vector<double>( this->vertexCount() );
	vector<double>* newPageRanks = new vector<double>( this->vertexCount() );
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		pageRanks->at( vertex ) = (double)1 / d;
	}
	
	for( unsigned int k = 0; true; k++ ) {
		for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
			// this prevents repetition of costly floating-point divisions
			scaledPageRanks.at( vertex ) = pageRanks->at( vertex ) / this->outList.at( vertex ).size();
		}
		
		double sum = 0.0;
		for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
			double& newPageRank = newPageRanks->at( vertex );
			newPageRank = 0;
			const neighbor_set_t& inNeighbors = this->inList.at( vertex );
			for( neighbor_set_t::const_iterator neighborIterator = inNeighbors.begin(); neighborIterator != inNeighbors.end(); neighborIterator++ ) {
				const vertex_t inNeighbor = neighborIterator->first;
				newPageRank += scaledPageRanks.at( inNeighbor );
			}
			newPageRank *= d;
			newPageRank += (1-d)/this->vertexCount();
			sum += newPageRank;
		}
		
		for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
			newPageRanks->at( vertex ) /= sum;
		}
		vector<double>* temp = pageRanks;
		pageRanks = newPageRanks;
		newPageRanks = temp;

//		sum = 0;
//		for( vector<double>::const_iterator it = pageRanks->begin(); it != pageRanks->end(); it++ ) {
//			sum += *it;
//		}
		// convergence test (r at the moment)
		if( k > 0 ) {
			double r = Statistics<double>::sampleCorrelationCoefficient( oldPageRanks, *pageRanks );
//			cerr << "k=" << k << " - r=" << r << " - sum=" << sum << "\n";
			if( r > 0.999 ) {
				oldPageRanks = *pageRanks;
				delete pageRanks;
				delete newPageRanks;
				return oldPageRanks;
			}
		}
		oldPageRanks = *pageRanks;
	}
}

vector<vector<double> > WeightedNetwork::simRank( double C ) const {
	vertex_t n = this->vertexCount();
	vector<vector<double> >* oldSimRanks = new vector<vector<double> >( n, vector<double>(n) );
	vector<vector<double> >* simRanks = new vector<vector<double> >( n, vector<double>(n) );
	
	// set initial values; note that vector initialization above gives us initial 0s
	for( vertex_t vertex = 0; vertex < n; vertex++ ) {
		oldSimRanks->at( vertex ).at( vertex ) = 1;
		simRanks->at( vertex ).at( vertex ) = 1;
	}
	
	// perform iterations until convergence
	for( unsigned int k = 0; true; k++ ) {
//		cerr << "Iteration " << k << "...\n";
		for( vertex_t vertex1 = 0; vertex1 < n; vertex1++ ) {
			const neighbor_set_t& inNeighbors1 = this->inList.at( vertex1 );
			for( vertex_t vertex2 = vertex1+1; vertex2 < n; vertex2++ ) {
				const neighbor_set_t& inNeighbors2 = this->inList.at( vertex2 );
				double& cell = simRanks->at( vertex1 ).at( vertex2 );
				cell = 0;
				if( inNeighbors1.size() > 0 && inNeighbors2.size() > 0 ) {
					for( neighbor_set_t::const_iterator it1 = inNeighbors1.begin(); it1 != inNeighbors1.end(); it1++ ) {
						vertex_t neighbor1 = it1->first;
						const vector<double>& oneDim = oldSimRanks->at( neighbor1 );
						for( neighbor_set_t::const_iterator it2 = inNeighbors2.begin(); it2 != inNeighbors2.end(); it2++ ) {
							vertex_t neighbor2 = it2->first;
							cell += oneDim.at( neighbor2 );
						}
					}
					cell = C*cell / (inNeighbors1.size()*inNeighbors2.size());
				}
				simRanks->at( vertex2 ).at( vertex1 ) = cell;
			}
		}
		
		// convergence test (r at the moment)
		if( k > 1 ) {
			double r = Statistics<double>::sampleCorrelationCoefficient( *oldSimRanks, *simRanks );
//			cerr << "r=" << r << "\n";
			if( r > 0.999 ) {
				delete oldSimRanks;
				vector<vector<double> > result = *simRanks;
				delete simRanks;
				return result;
			}
		}
		vector<vector<double> >* tempSimRanks = oldSimRanks;
		oldSimRanks = simRanks;
		simRanks = tempSimRanks;
	}
}

double WeightedNetwork::assortativityCoefficient() const {
	vector<unsigned int> x = vector<unsigned int>();
	vector<unsigned int> y = vector<unsigned int>();
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		const neighbor_set_t& outNeighbors = this->outNeighbors( vertex );
		for( neighbor_set_t::const_iterator neighborIterator = outNeighbors.begin(); neighborIterator != outNeighbors.end(); neighborIterator++ ) {
			x.push_back( outNeighbors.size() );
			y.push_back( this->outDegree( neighborIterator->first ) );
		}
	}
	return Statistics<unsigned int>::sampleCorrelationCoefficient( x, y );
}

vector<double> WeightedNetwork::betweenness() const {
	vector<double> C = vector<double>( this->vertexCount(), 0.0 );
	for( vertex_t s = 0; s < this->vertexCount(); s++ ) {
		stack<vertex_t> S;
		vector<vector<vertex_t> > P = vector<vector<vertex_t> >( this->vertexCount() );
		vector<unsigned long> sigma = vector<unsigned long>( this->vertexCount(), 0 );
		vector<long> d = vector<long>( this->vertexCount(), -1 );
		queue<vertex_t> Q;
		
		sigma.at( s ) = 1;
		d.at( s ) = 0;
		Q.push( s );
		
		while( !Q.empty() ) {
			vertex_t v = Q.front();
			Q.pop();
			S.push( v );
			const neighbor_set_t& outNeighbors = this->outNeighbors( v );
			for( neighbor_set_t::const_iterator neighborIt = outNeighbors.begin(); neighborIt != outNeighbors.end(); neighborIt++ ) {
				vertex_t w = neighborIt->first;
				if( d.at( w ) < 0 ) {
					Q.push( w );
					d.at( w ) = d.at( v ) + 1;
				}
				if( d.at( w ) == d.at( v ) + 1 ) {
					sigma.at( w ) = sigma.at( w ) + sigma.at( v );
					P.at( w ).push_back( v );
				}
			}
		}
		vector<double> delta = vector<double>( this->vertexCount(), 0 );
		while( !S.empty() ) {
			vertex_t w = S.top();
			S.pop();
			for( vector<vertex_t>::const_iterator it = P.at( w ).begin(); it != P.at( w ).end(); it++ ) {
				vertex_t v = *it;
				delta.at( v ) = delta.at( v ) + ((double)sigma.at(v)/(double)sigma.at(w)) * (1 + delta.at(w));
			}
			if( w != s ) {
				C.at( w ) = C.at( w ) + delta.at( w );
			}
		}
	}
	return C;
}

double WeightedNetwork::closeness( vertex_t vertex ) const {
	vector<unsigned int> distances = vector<unsigned int>( this->vertexCount() );
	unsigned int found = 0;

	vector<vertex_t> oldSearch;

	distances.at( vertex ) = 0;
	oldSearch.push_back( vertex );
	for( unsigned int distance = 1; !oldSearch.empty(); distance++ ) {
		vector<vertex_t> newSearch;
		for( vector<vertex_t>::const_iterator vertexIterator = oldSearch.begin(); vertexIterator != oldSearch.end(); vertexIterator++ ) {
			const vertex_t searchVertex = *vertexIterator;
			const neighbor_set_t& neighbors = this->outList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); neighborIterator++ ) {
				const vertex_t neighbor = neighborIterator->first;
				if( neighbor != vertex && distances.at( neighbor ) == 0 ) {
					distances.at( neighbor ) = distance;
					newSearch.push_back( neighbor );
					found++;
				}
			}
		}
		oldSearch.swap( newSearch );
	}
	
	double total = 0.0;
	for( vector<unsigned int>::const_iterator distancesIt = distances.begin(); distancesIt != distances.end(); distancesIt++ ) {
		total += *distancesIt;
	}
	return total / found;
}

vector<double> WeightedNetwork::closeness() const {
	vector<double> closenesses = vector<double>( this->vertexCount() );
	for( vertex_t vertex = 0; vertex < this->vertexCount(); vertex++ ) {
		closenesses.at( vertex ) = this->closeness( vertex );
	}
	return closenesses;
}

vector<unsigned int> WeightedNetwork::shortestPathLengths( vertex_t vertex ) const {
	vector<unsigned int> found = vector<unsigned int>( this->vertexCount() );
	
	vector<vertex_t> oldSearch;
	
	found.at( vertex ) = 0;
	oldSearch.push_back( vertex );
	unsigned int currentDegree = 1;
	while( !oldSearch.empty() ) {
		vector<vertex_t> newSearch;
		for( vector<vertex_t>::const_iterator searchIterator = oldSearch.begin(); searchIterator != oldSearch.end(); ++searchIterator ) {
			const vertex_t searchVertex = *searchIterator;
			const neighbor_set_t& neighbors = this->outList.at( searchVertex );
			for( neighbor_set_t::const_iterator neighborIterator = neighbors.begin(); neighborIterator != neighbors.end(); ++neighborIterator ) {
				const vertex_t neighborVertex = neighborIterator->first;
				if( neighborVertex != vertex && found.at( neighborVertex ) == 0 ) {
					found.at( neighborVertex ) = currentDegree;
					newSearch.push_back( neighborIterator->first );
				}
			}
		}
		++currentDegree;
		oldSearch = newSearch;
	}

	return found;
}

vector<unsigned int> WeightedNetwork::shortestPathLengths() const {
	vector<unsigned int> lengths;
	for( vertex_t vertex = 0; vertex < this->vertexCount(); ++vertex ) {
		vector<unsigned int> vertexLengths = this->shortestPathLengths( vertex );
		lengths.insert( lengths.end(), vertexLengths.begin(), vertexLengths.end() );
	}
	return lengths;
}
