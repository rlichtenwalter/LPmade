/*
------------------------------------------------
Copyright (C) 2011 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef VCP4DIRECTEDLINKPREDICTOR_H
#define VCP4DIRECTEDLINKPREDICTOR_H


#include "../WeightedNetwork.h"
#include "LinkPredictor.h"

class VCP4DirectedLinkPredictor : public LinkPredictor {
	private:
		const static size_t graphlets = 2048;
		const static size_t map[graphlets];
		const static size_t elements = 1056;
		unsigned long connectedPairs;
		unsigned long amutualPairs;
		unsigned long mutualPairs;
		unsigned long unconnectedPairs;
	protected:
	public:
		VCP4DirectedLinkPredictor( const WeightedNetwork& );
		~VCP4DirectedLinkPredictor();
		double generateScore( vertex_t, vertex_t );
};

#endif
