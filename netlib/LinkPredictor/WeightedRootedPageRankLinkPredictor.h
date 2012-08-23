/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef WEIGHTEDROOTEDPAGERANKLINKPREDICTOR_H
#define WEIGHTEDROOTEDPAGERANKLINKPREDICTOR_H

#include <vector>
#include "../WeightedNetwork.h"
#include "LinkPredictor.h"

using std::vector;

class WeightedRootedPageRankLinkPredictor : public LinkPredictor {
	private:
		double alpha;
		vector<double> scores;
		WeightedRootedPageRankLinkPredictor( const WeightedNetwork& );
	protected:
	public:
		WeightedRootedPageRankLinkPredictor( const WeightedNetwork&, double );
		~WeightedRootedPageRankLinkPredictor();
		double generateScore( vertex_t, vertex_t );
};

#endif
