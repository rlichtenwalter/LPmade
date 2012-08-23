/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include "JDegreeLinkPredictor.h"

JDegreeLinkPredictor::JDegreeLinkPredictor( const WeightedNetwork& network ) : LinkPredictor( network ) {
}

JDegreeLinkPredictor::~JDegreeLinkPredictor() {
}

double JDegreeLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	return (double)(this->network.outDegree( neighbor ));
}
