
#ifndef _Season_h_
#define _Season_h_

#include "Team.h"

class Season {
public:
	string series;
	unsigned number;
	vector< Team > teams;

	Season( string ser, unsigned num ) : series( ser ), number( num ) {
		//if ( num < 11 ) parseTable();
		//else
		parseMatches();
	}

	void parseTable() {
		char c;
		string s, x;
		ostringstream os;
		os << series << "/t" << ( number < 10 ? "0" : "" ) << number << ".txt";
		ifstream f( os.str().c_str() );
		while ( getline( f, s ) ) {
			vector< string > u;
			istringstream is( s.substr( s.find( '.' ) + 1 ) );
			for ( is >> x; x.find( '-' ) == string::npos; is >> x )
				u.push_back( x );

			string name = u[ 0 ];
			for ( unsigned i = 1; i + 4 < u.size(); ++i )
				name += " " + u[ i ];

			Team t( name );
			istringstream ms( u[ u.size() - 4 ] );
			ms >> t.m;
			istringstream ws( u[ u.size() - 3 ] );
			ws >> t.w;
			istringstream ts( u[ u.size() - 2 ] );
			ts >> t.t;
			istringstream ls( u[ u.size() - 1 ] );
			ls >> t.l;
			istringstream fas( x );
			fas >> t.f >> c >> t.a;
			t.p = 3*t.w + t.t;
			t.d = t.f - t.a;
			teams.push_back( t );
		}
		f.close();
	}

	void parseMatches() {
		unsigned a, h;
		string s, t, x;
		ostringstream os;
		os << series << "/m" << ( number < 10 ? "0" : "" ) << number << ".txt";
		ifstream f( os.str().c_str() );
		for ( unsigned i = 0; i < 14; ++i ) {
			getline( f, s );
			getline( f, s );
			for ( unsigned j = 0; j < 4; ++j ) {
				getline( f, s );
				vector< string > w;
				istringstream is( s );
				for ( is >> x; x != "-"; is >> x )
					w.push_back( x );

				unsigned k;
				s = w[ 0 ];
				for ( k = 1; k < w.size(); ++k )
					s += " " + w[ k ];

				for ( is >> x; x != "-"; is >> x )
					w.push_back( x );

				t = w[ k ];
				for ( ++k; k + 1 < w.size(); ++k )
					t += " " + w[ k ];

				istringstream hs( w[ w.size() - 1 ] );
				hs >> h;
				is >> a;

				updatematch( s, h, a );
				updatematch( t, a, h );
			}
		}
		f.close();

		for ( vector< Team >::iterator i = teams.begin(); i != teams.end(); ++i ) {
			if ( i->m < 14 ) {
				vector< Team >::iterator j = i;
				for ( ++j; i->m + j->m != 14; ++j );
				j->update( *i );
				j->names.insert( i->names.begin(), i->names.end() );
				teams.erase( i );
			}
		}

		sort( teams.begin(), teams.end() );
	}

	void updatematch( string s, unsigned f, unsigned a ) {
		// hack to deal with "~DrPepper FC"
		if ( s[0] == '~' ) s = s.substr( 1 );

		unsigned i;
		for ( i = 0; i < teams.size() && teams[i].names.find( s ) == teams[i].names.end(); ++i );
		if ( i == teams.size() ) teams.push_back( Team( s ) );
		teams[i].update( f, a );
	}

	Team & getTeam( set< string > & s ) {
		for ( unsigned i = 0; i < teams.size(); ++i )
			for ( set< string >::iterator j = teams[i].names.begin(); j != teams[i].names.end(); ++j )
				if ( s.find( *j ) != s.end() )
					return teams[i];
		return teams[0];
	}

	void tableprint( ostream & stream ) const {
		for ( unsigned i = 0; i < teams.size(); ++i ) {
			teams[i].tableprint( stream );
			stream << "\n";
		}
	}

	void tabprint( ostream & stream ) const {
		for ( unsigned i = 0; i < teams.size(); ++i )
			stream << teams[i] << "\n";
	}
};

ostream & operator<<( ostream & stream, const Season & s ) {
	s.tabprint( stream );
	return stream;
}

#endif
