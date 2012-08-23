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
#include "VCP4DirectedLinkPredictor.h"

using std::list;
using std::set;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;

const size_t VCP4DirectedLinkPredictor::map[graphlets] = {0,1,2,3,1,4,5,6,2,5,7,8,3,6,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,10,14,18,22,11,15,19,23,12,16,20,24,13,17,21,25,58,59,60,61,59,62,63,64,60,63,65,66,61,64,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,26,30,34,38,27,31,35,39,28,32,36,40,29,33,37,41,68,72,76,80,69,73,77,81,70,74,78,82,71,75,79,83,100,101,102,103,101,104,105,106,102,105,107,108,103,106,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,42,46,50,54,43,47,51,55,44,48,52,56,45,49,53,57,84,88,92,96,85,89,93,97,86,90,94,98,87,91,95,99,110,114,118,122,111,115,119,123,112,116,120,124,113,117,121,125,126,127,128,129,127,130,131,132,128,131,133,134,129,132,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,136,140,144,148,137,141,145,149,138,142,146,150,139,143,147,151,200,204,208,212,201,205,209,213,202,206,210,214,203,207,211,215,264,268,272,276,265,269,273,277,266,270,274,278,267,271,275,279,328,332,336,340,329,333,337,341,330,334,338,342,331,335,339,343,152,156,160,164,153,157,161,165,154,158,162,166,155,159,163,167,216,220,224,228,217,221,225,229,218,222,226,230,219,223,227,231,280,284,288,292,281,285,289,293,282,286,290,294,283,287,291,295,344,348,352,356,345,349,353,357,346,350,354,358,347,351,355,359,168,172,176,180,169,173,177,181,170,174,178,182,171,175,179,183,232,236,240,244,233,237,241,245,234,238,242,246,235,239,243,247,296,300,304,308,297,301,305,309,298,302,306,310,299,303,307,311,360,364,368,372,361,365,369,373,362,366,370,374,363,367,371,375,184,188,192,196,185,189,193,197,186,190,194,198,187,191,195,199,248,252,256,260,249,253,257,261,250,254,258,262,251,255,259,263,312,316,320,324,313,317,321,325,314,318,322,326,315,319,323,327,376,380,384,388,377,381,385,389,378,382,386,390,379,383,387,391,392,393,394,395,393,396,397,398,394,397,399,400,395,398,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,441,442,443,444,445,446,447,448,449,402,406,410,414,403,407,411,415,404,408,412,416,405,409,413,417,450,451,452,453,451,454,455,456,452,455,457,458,453,456,458,459,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,418,422,426,430,419,423,427,431,420,424,428,432,421,425,429,433,460,464,468,472,461,465,469,473,462,466,470,474,463,467,471,475,492,493,494,495,493,496,497,498,494,497,499,500,495,498,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,517,434,438,442,446,435,439,443,447,436,440,444,448,437,441,445,449,476,480,484,488,477,481,485,489,478,482,486,490,479,483,487,491,502,506,510,514,503,507,511,515,504,508,512,516,505,509,513,517,518,519,520,521,519,522,523,524,520,523,525,526,521,524,526,527,528,529,530,531,529,532,533,534,530,533,535,536,531,534,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,538,542,546,550,539,543,547,551,540,544,548,552,541,545,549,553,586,587,588,589,587,590,591,592,588,591,593,594,589,592,594,595,596,597,598,599,600,601,602,603,604,605,606,607,608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,624,625,626,627,554,558,562,566,555,559,563,567,556,560,564,568,557,561,565,569,596,600,604,608,597,601,605,609,598,602,606,610,599,603,607,611,628,629,630,631,629,632,633,634,630,633,635,636,631,634,636,637,638,639,640,641,642,643,644,645,646,647,648,649,650,651,652,653,570,574,578,582,571,575,579,583,572,576,580,584,573,577,581,585,612,616,620,624,613,617,621,625,614,618,622,626,615,619,623,627,638,642,646,650,639,643,647,651,640,644,648,652,641,645,649,653,654,655,656,657,655,658,659,660,656,659,661,662,657,660,662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,700,701,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752,753,754,755,756,757,758,759,760,761,762,763,764,765,766,767,768,769,770,771,772,773,774,775,776,777,778,779,780,781,782,783,784,785,786,787,788,789,790,791,792,793,794,795,796,797,798,799,800,801,802,803,804,805,806,807,808,809,810,811,812,813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,829,830,831,832,833,834,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,853,854,855,856,857,858,859,860,861,862,863,864,865,866,867,868,869,870,871,872,873,874,875,876,877,878,879,880,881,882,883,884,885,886,887,888,889,890,891,892,893,894,895,896,897,898,899,900,901,902,903,904,905,906,907,908,909,910,911,912,913,914,915,916,917,918,919,664,668,672,676,665,669,673,677,666,670,674,678,667,671,675,679,728,732,736,740,729,733,737,741,730,734,738,742,731,735,739,743,792,796,800,804,793,797,801,805,794,798,802,806,795,799,803,807,856,860,864,868,857,861,865,869,858,862,866,870,859,863,867,871,680,684,688,692,681,685,689,693,682,686,690,694,683,687,691,695,744,748,752,756,745,749,753,757,746,750,754,758,747,751,755,759,808,812,816,820,809,813,817,821,810,814,818,822,811,815,819,823,872,876,880,884,873,877,881,885,874,878,882,886,875,879,883,887,696,700,704,708,697,701,705,709,698,702,706,710,699,703,707,711,760,764,768,772,761,765,769,773,762,766,770,774,763,767,771,775,824,828,832,836,825,829,833,837,826,830,834,838,827,831,835,839,888,892,896,900,889,893,897,901,890,894,898,902,891,895,899,903,712,716,720,724,713,717,721,725,714,718,722,726,715,719,723,727,776,780,784,788,777,781,785,789,778,782,786,790,779,783,787,791,840,844,848,852,841,845,849,853,842,846,850,854,843,847,851,855,904,908,912,916,905,909,913,917,906,910,914,918,907,911,915,919,920,921,922,923,921,924,925,926,922,925,927,928,923,926,928,929,930,931,932,933,934,935,936,937,938,939,940,941,942,943,944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,975,976,977,930,934,938,942,931,935,939,943,932,936,940,944,933,937,941,945,978,979,980,981,979,982,983,984,980,983,985,986,981,984,986,987,988,989,990,991,992,993,994,995,996,997,998,999,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,946,950,954,958,947,951,955,959,948,952,956,960,949,953,957,961,988,992,996,1000,989,993,997,1001,990,994,998,1002,991,995,999,1003,1020,1021,1022,1023,1021,1024,1025,1026,1022,1025,1027,1028,1023,1026,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,962,966,970,974,963,967,971,975,964,968,972,976,965,969,973,977,1004,1008,1012,1016,1005,1009,1013,1017,1006,1010,1014,1018,1007,1011,1015,1019,1030,1034,1038,1042,1031,1035,1039,1043,1032,1036,1040,1044,1033,1037,1041,1045,1046,1047,1048,1049,1047,1050,1051,1052,1048,1051,1053,1054,1049,1052,1054,1055};

VCP4DirectedLinkPredictor::VCP4DirectedLinkPredictor( const WeightedNetwork& network ) : LinkPredictor( network ), connectedPairs( 0 ), amutualPairs( 0 ), mutualPairs( 0 ), unconnectedPairs( 0 ) {
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
				++amutualPairs;
				++outIt;
			} else if( outIt->first > inIt->first ) {
				++amutualPairs;
				++inIt;
			} else {
				++mutualPairs;
				++outIt;
				++inIt;
			}
		}
		while( outIt != outEnd ) {
			++amutualPairs;
			++connectedPairs;
			++outIt;
		}
		while( inIt != inEnd ) {
			++amutualPairs;
			++connectedPairs;
			++inIt;
		}
	}
	unsigned long potentialConnections = (unsigned long)network.vertexCount() * (unsigned long)(network.vertexCount() - 1) / 2;
	unconnectedPairs = potentialConnections - connectedPairs;
}

VCP4DirectedLinkPredictor::~VCP4DirectedLinkPredictor() {
}

double VCP4DirectedLinkPredictor::generateScore( vertex_t vertex, vertex_t neighbor ) {
	vertex_t v1 = vertex;
	vertex_t v2 = neighbor;
	unsigned long remapped[elements] = {0};
	size_t index = 1024 * (size_t)network.hasEdge( v2, v1 );
	
	unsigned long vertices = 2;
	unsigned long connections = 0;
	unsigned long amutuals = 0;
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
		size_t v1v3 = network.hasEdge( v1, v3 ) + 2 * network.hasEdge( v3, v1 );
		if( v1v3 ) {
			index2 += 1 * v1v3;
			++connections;
		} else {
			++gaps;
		}
		if( v1v3 == 1 || v1v3 == 2 ) {
			++amutuals;
		}
		size_t v2v3 = network.hasEdge( v2, v3 ) + 2 * network.hasEdge( v3, v2 );
		if( v2v3 ) {
			index2 += 16 * v2v3;
			++connections;
		} else {
			++gaps;
		}
		if( v2v3 == 1 || v2v3 == 2 ) {
			++amutuals;
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
		unsigned long v3v4Connections = 0;
		for( list<vertex_t>::const_iterator v4It = v4Vertices.begin(); v4It != v4Vertices.end(); ++v4It ) {
			vertex_t v4 = *v4It;
			size_t index3 = index2;
			size_t v1v4 = network.hasEdge( v1, v4 ) + 2 * network.hasEdge( v4, v1 );
			if( v1v4 ) {
				index3 += 4 * v1v4;
				++v3v4Connections;
			} else {
				++gaps;
			}
			if( v1v4 == 1 || v1v4 == 2 ) {
				++amutuals;
			}
			size_t v2v4 = network.hasEdge( v2, v4 ) + 2 * network.hasEdge( v4, v2 );
			if( v2v4 ) {
				index3 += 64 * v2v4;
				++v3v4Connections;
			} else {
				++gaps;
			}
			if( v2v4 == 1 || v2v4 == 2 ) {
				++amutuals;
			}
			size_t v3v4 = network.hasEdge( v3, v4 ) + 2 * network.hasEdge( v4, v3 );
			if( v3v4 ) {
				index3 += 256 * v3v4;
				++v3v4Connections;
			} else {
				++gaps;
			}
			if( v3v4 == 1 || v3v4 == 2 ) {
				++amutuals;
			}
			++remapped[ map[index3] ];
		}
		
		// for each other v3 vertex if it has a higher index
		for( list<vertex_t>::const_iterator v4It = v3Vertices.begin(); v4It != v3Vertices.end(); ++v4It ) {
			vertex_t v4 = *v4It;
			if( v4 <= v3 ) {
				continue;
			}
			size_t index3 = index2;
			size_t v1v4 = network.hasEdge( v1, v4 ) + 2 * network.hasEdge( v4, v1 );
			index3 += 4 * v1v4;
			size_t v2v4 = network.hasEdge( v2, v4 ) + 2 * network.hasEdge( v4, v2 );
			index3 += 64 * v2v4;
			size_t v3v4 = network.hasEdge( v3, v4 ) + 2 * network.hasEdge( v4, v3 );
			if( v3v4 ) {
				index3 += 256 * v3v4;
				++v3v4Connections;
			} else {
				++gaps;
			}
			if( v3v4 == 1 || v3v4 == 2 ) {
				++amutuals;
			}
			++remapped[ map[index3] ];
		}
		connections += v3v4Connections;
		
		// account for triangles and gaps
		remapped[ map[index2] ] += network.vertexCount() - 2 - (v3Vertices.size() + v4Vertices.size());
		remapped[ map[index] ] -= network.vertexCount() - 2 - (v3Vertices.size() + v4Vertices.size()); // go negative for the number of triangles (which contain v3v4 gaps)
	}
	vertices += v3Vertices.size();
		
	// account for the least connected substructures
	remapped[ map[256+index] ] = this->amutualPairs - (amutuals + index/1024);
	remapped[ map[768+index] ] = this->mutualPairs - (connections - amutuals);
	remapped[ map[index] ] += this->unconnectedPairs - (gaps + !(bool)(index/1024)) - (2 * (network.vertexCount() - vertices));

	// do the remapping and output
	cerr << network.translateIntToExt( vertex ) << " " << network.translateIntToExt( neighbor );
	for( size_t i = 0; i < elements; ++i ) {
		cerr << " " << remapped[i];
	}
	cerr << "\n";
	return -1;
}
