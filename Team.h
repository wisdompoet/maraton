
#ifndef _Team_h_
#define _Team_h_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <string>
#include <vector>

using namespace std;

class Team {
public:
	set< string > names;
	int m, w, t, l, f, a, p, d;
	map< unsigned, unsigned > series;
	int first, second, third;

	Team( string n ) : m( 0 ), w( 0 ), t( 0 ), l( 0 ), f( 0 ), a( 0 ), p( 0 ), d( 0 ), first( 0 ), second( 0 ), third( 0 ) {
		names.insert( n );
	}

	Team( const set< string > & n ) : names( n ), m( 0 ), w( 0 ), t( 0 ), l( 0 ), f( 0 ), a( 0 ), p( 0 ), d( 0 ), first( 0 ), second( 0 ), third( 0 ) {}

	bool operator<( const Team &t ) const {
		return t.p < p || ( t.p == p && ( t.d < d || ( t.d == d && t.f < f ) ) );
	}

	void update( Team & team ) {
		m += team.m;
		w += team.w;
		t += team.t;
		l += team.l;
		f += team.f;
		a += team.a;
		p += team.p;
		d += team.d;
	}

	unsigned appearances( unsigned ser ) {
		unsigned app = 0, old = 0;
		for ( map< unsigned, unsigned >::iterator it = series.begin(); it != series.end(); ++it )
			if ( it->second == ser ) {
				if ( old + 1 < it->first )
					++app;
				old = it->first;
			}
		return app;
	}

	void update( int x, int y ) {
		m++;
		if ( y < x ) w++;
		else if ( x < y ) l++;
		else t++;
		f += x;
		a += y;
		p += y < x ? 3 : x < y ? 0 : 1;
		d += x - y;
	}

	void tableprint( ostream & stream ) const {
		stream << "[td]" << *names.begin() << "[/td][td]" << m << "[/td][td]" << w;
		stream << "[/td][td]" << t << "[/td][td]" << l << "[/td][td]" << f << "-" << a;
		stream << "[/td][td]" << p << "[/td][td]" << (d < 0 ? "" : "+") << d << "[/td]";
	}

	void medalprint( ostream & stream ) const {
		stream << "[td]" << *names.begin() << "[/td][td]" << first << "[/td][td]" << second << "[/td][td]" << third << "[/td]";
	}

	void tabprint( ostream & stream ) const {
		stream << *names.begin() << "\t" << m << "\t" << w << "\t" << t << "\t" << l << "\t";
		stream << f << "\t" << a << "\t" << p << "\t" << d;
	}
};

ostream & operator<<( ostream & stream, const Team & t ) {
	t.tabprint( stream );
	return stream;
}

#endif
