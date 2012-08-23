/*
------------------------------------------------
Copyright (C) 2010,2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef WEIGHTEDNETWORK_H
#define WEIGHTEDNETWORK_H

#include <vector>
#include <iostream>
#include <climits>
#include <limits>
#include <cstdlib>

using std::vector;
using std::pair;
using std::istream;
using std::ostream;
using std::numeric_limits;

typedef unsigned int vertex_t;
typedef double weight_t;
typedef pair<vertex_t,vertex_t> edge_t;
typedef pair<vertex_t,weight_t> neighbor_t;
typedef vector<neighbor_t> neighbor_set_t;
typedef vector<neighbor_set_t> vertex_set_t;

const vertex_t INVALID_VERTEX = numeric_limits<vertex_t>::max();

class WeightedNetwork {
	private:
	protected:
		vector<vertex_t> dictionary;
		vertex_set_t outList;
		vertex_set_t inList;
		unsigned int numEdges;
		void compact();
		void addEdge( vertex_t, vertex_t, weight_t );
		static neighbor_set_t intersect( const neighbor_set_t&, const neighbor_set_t& );
		WeightedNetwork();

	public:
		WeightedNetwork( const WeightedNetwork& );
		WeightedNetwork& operator=( const WeightedNetwork& );
		~WeightedNetwork();
		static WeightedNetwork readStream( istream&, const char, unsigned int, unsigned int, bool );
		static WeightedNetwork readWeightedStream( istream&, const char, unsigned int, unsigned int, unsigned int, bool );
		static WeightedNetwork readNetwork( istream& );
		static WeightedNetwork combine( const WeightedNetwork& network1, const WeightedNetwork& network2 );
		neighbor_set_t commonOutNeighbors( vertex_t, vertex_t ) const;
		neighbor_set_t commonInNeighbors( vertex_t, vertex_t ) const;
		void print( ostream& ) const;
		void printEdgeList( ostream& ) const;
		void printAdjacencyList( ostream& ) const;
		unsigned int vertexCount() const;
		unsigned int edgeCount() const;
		const neighbor_set_t& outNeighbors( vertex_t ) const;
		const neighbor_set_t& inNeighbors( vertex_t ) const;
		const vector<vertex_t> findOutNeighbors( vertex_t, unsigned int ) const;
		const vector<vertex_t> findInNeighbors( vertex_t, unsigned int ) const;
		WeightedNetwork snowballSample( vertex_t, unsigned int ) const;
		WeightedNetwork snowballSampleIn( vertex_t, unsigned int ) const;
		WeightedNetwork randomEdgeSample( double, unsigned int ) const;
		WeightedNetwork undirected() const;
		WeightedNetwork reverseEdges() const;
		WeightedNetwork removeEdges( const WeightedNetwork & ) const;
		WeightedNetwork removeEdges( const vector<edge_t>& ) const;
		WeightedNetwork thresholdEdges( weight_t ) const;
		WeightedNetwork removeAmutuals() const;
		WeightedNetwork removeIsolates() const;
		bool hasEdge( vertex_t, vertex_t ) const;
		weight_t edgeWeight( vertex_t, vertex_t ) const;
		vertex_t maxOutDegreeVertex() const;
		vector<WeightedNetwork> wccs() const;
		vector<WeightedNetwork> sccs() const;
		unsigned int pseudoDiameter() const;
		unsigned int countMutuals() const;
		double freeChoiceMutualityIndex() const;
		unsigned int outDegree( vertex_t ) const;
		unsigned int inDegree( vertex_t ) const;
		weight_t outVolume( vertex_t ) const;
		weight_t inVolume( vertex_t ) const;
		vertex_t translateExtToInt( vertex_t ) const;
		vertex_t translateIntToExt( vertex_t ) const;
		bool hasEdgeExt( vertex_t, vertex_t ) const;
		weight_t edgeWeightExt( vertex_t, vertex_t ) const;
		unsigned int eccentricity( vertex_t vertex ) const;
		double clusteringCoefficient( vertex_t vertex ) const;
		double clusteringCoefficient() const;
		vector<double> pageRank( double ) const;
		vector<vector<double> > simRank( double ) const;
		double assortativityCoefficient() const;
		vector<double> betweenness() const;
		double closeness( vertex_t ) const;
		vector<double> closeness() const;
		vector<unsigned int> shortestPathLengths() const;
		vector<unsigned int> shortestPathLengths( vertex_t vertex ) const;
};

#endif
