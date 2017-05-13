
#ifndef _Series_h_
#define _Series_h_

#include "Season.h"

class Series {
public:
	string name;
	unsigned level;
	vector< Season > seasons;

	Series( string n, unsigned l, unsigned from, unsigned to ) : name( n ), level( l ) {
		for ( unsigned i = from; i <= to; ++i ) {
			seasons.push_back( Season( n, i ) );
		}
	}

	void tableprint( ostream & stream ) const {
		for ( unsigned i = 0; i < seasons.size(); ++i ) {
			seasons[i].tableprint( stream );
			stream << "\n";
		}
	}

	void tabprint( ostream & stream ) const {
		for ( unsigned i = 0; i < seasons.size(); ++i )
			stream << seasons[i] << "\n";
	}
};

ostream & operator<<( ostream & stream, const Series & s ) {
	s.tabprint( stream );
	return stream;
}

#endif
