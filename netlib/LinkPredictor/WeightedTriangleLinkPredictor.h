/*
------------------------------------------------
Copyright (C) 2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef WEIGHTEDTRIANGLELINKPREDICTOR_H
#define WEIGHTEDTRIANGLELINKPREDICTOR_H

#include "../WeightedNetwork.h"
#include "LinkPredictor.h"

struct DetailedNeighbors;

class WeightedTriangleLinkPredictor : public LinkPredictor {
	private:
		const static unsigned long graphlets = 32;
		int selection;
		weight_t counts[graphlets];
		struct DetailedNeighbors {
			vector<neighbor_t> inOnly;
			vector<neighbor_t> outOnly;
			vector<neighbor_t> both;
			vector<neighbor_t> either;
		};
		void computeDetailedNeighbors( const neighbor_set_t&, const neighbor_set_t&, DetailedNeighbors& ) const;
		weight_t intersectionCount( const vector<neighbor_t>&, const vector<neighbor_t>& ) const;
		weight_t differenceCount( const vector<neighbor_t>&, const vector<neighbor_t>& ) const;
		weight_t unionCount( const vector<neighbor_t>&, const vector<neighbor_t>& ) const;
		void edgeProfile( vertex_t, vertex_t, weight_t[] );
	protected:
	public:
		WeightedTriangleLinkPredictor( const WeightedNetwork&, int );
		~WeightedTriangleLinkPredictor();
		double generateScore( vertex_t, vertex_t );
};

#endif
