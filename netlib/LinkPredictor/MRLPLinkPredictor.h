/*
------------------------------------------------
Copyright (C) 2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef MRLPLINKPREDICTOR_H
#define MRLPLINKPREDICTOR_H

#include "../WeightedNetwork.h"
#include "LinkPredictor.h"

struct DetailedNeighbors;

class MRLPLinkPredictor : public LinkPredictor {
	private:
		const static unsigned long graphlets = 32;
		unsigned long unidirectional;
		unsigned long bidirectional;
		double counts[graphlets];
		struct DetailedNeighbors {
			vector<vertex_t> inOnly;
			vector<vertex_t> outOnly;
			vector<vertex_t> both;
			vector<vertex_t> either;
		};
		void computeDetailedNeighbors( const neighbor_set_t&, const neighbor_set_t&, DetailedNeighbors& ) const;
		unsigned long intersectionCount( const vector<vertex_t>&, const vector<vertex_t>& ) const;
		unsigned long differenceCount( const vector<vertex_t>&, const vector<vertex_t>& ) const;
		unsigned long unionCount( const vector<vertex_t>&, const vector<vertex_t>& ) const;
		void edgeProfile( vertex_t, vertex_t, unsigned long[] );
	protected:
	public:
		MRLPLinkPredictor( const WeightedNetwork& );
		~MRLPLinkPredictor();
		double generateScore( vertex_t, vertex_t );
};

#endif
