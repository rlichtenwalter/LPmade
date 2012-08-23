/*
------------------------------------------------
Copyright (C) 2010 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of LPmade.
LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <cmath>

using std::vector;

template <class T> class Statistics {
	private:
		static double standardDeviation( const vector<T>& x, double xmean ) {
			double sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				sum += pow( x.at(i) - xmean, 2 );
			}
			return sqrt( sum / (x.size()-1) );
		}
		static double standardDeviation( const vector<vector<T> >& x, double xmean ) {
			double sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				const vector<T>& row = x.at( i );
				for( std::size_t j = 0; j < row.size(); j++ ) {
					sum += pow( row.at( j ) - xmean, 2 );
				}
			}
			return sqrt( sum / (Statistics<T>::count( x )-1) );
		}
	protected:
	public:
		static std::size_t count( const vector<T>& x ) {
			return x.size();
		}
		static std::size_t count( const vector<vector<T> >& x ) {
			std::size_t count = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				count += x.at(i).size();
			}
			return count;
		}
		static T sum( const vector<T>& x ) {
			T sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				sum += x.at( i );
			}
			return sum;
		}
		static T sum( const vector<vector<T> >& x ) {
			T sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				sum += Statistics<T>::mean( x.at( i ) );
			}
			return sum;
		}
		static double mean( const vector<T>& x ){
			return (double)Statistics<T>::sum(x) / x.size();
		}
		static double mean( const vector<vector<T> >& x ) {
			return (double)Statistics<T>::sum(x) / Statistics<T>::count(x);
		}
		static double standardDeviation( const vector<T>& x ) {
			double xmean = Statistics<T>::mean( x );
			double sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				sum += pow( x.at(i) - xmean, 2 );
			}
			return sqrt( sum / (x.size()-1) );
		}
		static double standardDeviation( const vector<vector<T> >& x ) {
			double xmean = Statistics<T>::mean( x );
			double sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				const vector<T>& row = x.at( i );
				for( std::size_t j = 0; j < row.size(); j++ ) {
					sum += pow( row.at( j ) - xmean, 2 );
				}
			}
			return sqrt( sum / (Statistics<T>::count( x )-1) );
		}
		static double sampleCorrelationCoefficient( const vector<T>& x, const vector<T>& y ){
			if( x.size() != y.size() ) {
				throw "Statistics::r - vector sizes do not match\n";
			}
			
			double xmean = Statistics<T>::mean( x );
			double ymean = Statistics<T>::mean( y );
			double xStdDev = Statistics<T>::standardDeviation( x, xmean );
			double yStdDev = Statistics<T>::standardDeviation( y, ymean );
			double sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				sum += (x.at( i )-xmean) * (y.at(i)-ymean);
			}
			return sum / ((x.size()-1) * xStdDev * yStdDev);
		}
		static double sampleCorrelationCoefficient( const vector<vector<T> >& x, const vector<vector<T> >& y ) {
			if( Statistics<T>::count(x) != Statistics<T>::count(y) ) {
				throw "Statistics::r - matrix sizes do not match\n";
			}
			double xmean = Statistics<T>::mean( x );
			double ymean = Statistics<T>::mean( y );
			double xStdDev = Statistics<T>::standardDeviation( x, xmean );
			double yStdDev = Statistics<T>::standardDeviation( y, ymean );
			double sum = 0;
			for( std::size_t i = 0; i < x.size(); i++ ) {
				const vector<T>& xRow = x.at( i );
				const vector<T>& yRow = y.at( i );
				for( std::size_t j = 0; j < xRow.size(); j++ ) {
					sum += (xRow.at( j )-xmean) * (yRow.at( j )-ymean);
				}
			}
			return sum / ((Statistics<T>::count(x)-1) * xStdDev * yStdDev);
		}
		static void normalize( vector<T>& x ){
			T sum = Statistics<T>::sum( x );
			for( std::size_t i = 0; i < x.size(); i++ ) {
				x.at( i ) /= sum;
			}
		}
};

#endif
