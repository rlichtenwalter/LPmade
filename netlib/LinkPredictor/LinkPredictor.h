/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef LINKPREDICTOR_H
#define LINKPREDICTOR_H

#include <vector>
#include "../WeightedNetwork.h"

using std::ostream;
using std::vector;

class LinkPredictor {
	private:
		LinkPredictor& operator=( const LinkPredictor& );
	protected:
		const WeightedNetwork& network;
		vertex_t vertex;
		vertex_t neighbor;
		LinkPredictor();
	public:
		enum Direction { IN, OUT };
		LinkPredictor( const WeightedNetwork& );
		virtual ~LinkPredictor();
		virtual double generateScore( vertex_t, vertex_t ) = 0;
		void printOutNeighborScores( unsigned int degree );
		void printInNeighborScores( unsigned int degree );
};

#endif
