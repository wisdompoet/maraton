
#ifndef _Statistics_h_
#define _Statistics_h_

#include "Series.h"

#define TOPLIST 100

// different ways to sort teams

bool seasonsort( const Team & s, const Team & t ) {
	return t.m < s.m || ( t.m == s.m && ( t.p < s.p || ( t.p == s.p && ( t.d < s.d || ( t.d == s.d && t.f < s.f ) ) ) ) );
}

bool medalsort( const Team & s, const Team & t ) {
	return t.first < s.first || ( t.first == s.first && ( t.second < s.second || ( t.second == s.second && ( t.third < s.third ) ) ) );
}

bool streaksort( const pair< string, pair< unsigned, unsigned > > & p, const pair< string, pair< unsigned, unsigned > > & q ) {
	return q.second.first < p.second.first;
}

bool winsort( const pair< string, vector< unsigned > > & p, const pair< string, vector< unsigned > > & q ) {
	return q.second.size() < p.second.size();
}

// main class to generate statistics

class Statistics {
public:
	vector< Team > teams;
	vector< Series > series;
	map< string, unsigned > teamMap;

	Statistics( unsigned last ) {
		series.push_back( Series( "A", 1, 2, last ) );
		if ( 10 < last ) {
			for ( int i = 0; i < 4; ++i ) {
				stringstream ss;
				ss << "I" << (char)( 'a' + i );
				series.push_back( Series( ss.str(), 2, 11, last ) );
			}
			for ( int i = 0; i < 16; ++i ) {
				stringstream ss;
				ss << "II" << (char)( 'a' + i );
				series.push_back( Series( ss.str(), 3, 11, last ) );
			}
			// other series
		}

		for ( unsigned i = 0; i < series.size(); ++i )
			for ( unsigned j = 0; j < series[i].seasons.size(); ++j )
				for ( unsigned k = 0; k < series[i].seasons[j].teams.size(); ++k )
					processTeam( i, j, k );
	}

	void processTeam( unsigned ser, unsigned sea, unsigned tea ) {
		Team & newTeam = series[ser].seasons[sea].teams[tea];
		unsigned oldTeam = teams.size();
		for ( set< string >::iterator i = newTeam.names.begin(); i != newTeam.names.end(); ++i ) {
			map< string, unsigned >::iterator j = teamMap.find( *i );
			if ( j != teamMap.end() ) {
				oldTeam = j->second;
				break;
			}
		}
		if ( oldTeam == teams.size() ) {
			for ( set< string >::iterator i = newTeam.names.begin(); i != newTeam.names.end(); ++i )
				teamMap[*i] = oldTeam;
			teams.push_back( Team( newTeam.names ) );
		}
		teams[oldTeam].update( newTeam );
		teams[oldTeam].names.insert( newTeam.names.begin(), newTeam.names.end() );
		teams[oldTeam].series[series[ser].seasons[sea].number] = ser;
	}

	void longestStreak( unsigned level ) {
		vector< pair< std::string, pair< unsigned, unsigned > > > streaks;
		for ( unsigned i = 0; i < teams.size(); ++i ) {
			pair< unsigned, unsigned > best( 0, 0 );
			map< unsigned, unsigned >::iterator j = teams[i].series.begin();
			while ( j != teams[i].series.end() ) {
				if ( series[j->second].level <= level ) {
					unsigned streak = 0;
					unsigned old = j->first;
					Team t( teams[i].names );
					while ( j != teams[i].series.end() && j->first <= old + 1 && series[j->second].level <= level ) {
						old = j->first;
						++j;
						++streak;
					}
					if ( best.first < streak )
						best = make_pair( streak, old );
				}
				else ++j;
			}
			streaks.push_back( make_pair( *teams[i].names.begin(), best ) );
		}
		sort( streaks.begin(), streaks.end(), streaksort );
		std::cout << "[table]\n";
		for ( unsigned i = 0; i < TOPLIST; ++i ) {
			std::cout << "[tr][td]" << i + 1 << ".[/td][td]" << streaks[i].first << "[/td][td]" << streaks[i].second.first << "[/td][td]" << ( streaks[i].second.second - streaks[i].second.first + 1 ) << "-" << streaks[i].second.second << "[/td][/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void mostWinners( unsigned ser ) {
		map< string, vector< unsigned > > winMap;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			winMap[*series[ser].seasons[i].teams[0].names.begin()].push_back( series[ser].seasons[i].number );

		vector< pair< string, vector< unsigned > > > winners( winMap.begin(), winMap.end() );
		sort( winners.begin(), winners.end(), winsort );
		std::cout << "[table]\n";
		for ( unsigned i = 0; i < winners.size(); ++i ) {
			std::cout << "[tr][td]" << i + 1 << ".[/td][td]" << winners[i].first << "[/td][td]" << winners[i].second.size() << "[/td][td]" << winners[i].second[0];
			for ( unsigned j = 1; j < winners[i].second.size(); ++j )
				std::cout << "," << winners[i].second[j];
			std::cout << "[/td][/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void marathon( unsigned ser ) {
		vector< Team > participants;
		map< string, unsigned > partMap;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			for ( unsigned j = 0; j < series[ser].seasons[i].teams.size(); ++j ) {
				Team & t = series[ser].seasons[i].teams[j];
				Team & u = teams[teamMap[*t.names.begin()]];
			
				map< string, unsigned >::iterator k = partMap.find( *u.names.begin() );
				if ( k == partMap.end() ) {
					k = partMap.insert( partMap.begin(), make_pair( *u.names.begin(), participants.size() ) );
					participants.push_back( Team( u.names ) );
				}
				participants[k->second].update( t );
			}

		sort( participants.begin(), participants.end() );
		std::cout << "[table]\n";
		for ( unsigned i = 0; i < participants.size(); ++i ) {
			std::cout << "[tr][td]" << i + 1 << ".[/td]";
			participants[i].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void medalCount( unsigned ser ) {
		vector< Team > participants;
		map< string, unsigned > partMap;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			for ( unsigned j = 0; j < 3; ++j ) {
				Team & t = series[ser].seasons[i].teams[j];
				Team & u = teams[teamMap[*t.names.begin()]];
			
				map< string, unsigned >::iterator k = partMap.find( *u.names.begin() );
				if ( k == partMap.end() ) {
					k = partMap.insert( partMap.begin(), make_pair( *u.names.begin(), participants.size() ) );
					participants.push_back( Team( u.names ) );
				}
				if ( j == 0 ) participants[k->second].first++;
				if ( j == 1 ) participants[k->second].second++;
				if ( j == 2 ) participants[k->second].third++;
			}

		sort( participants.begin(), participants.end(), medalsort );
		std::cout << "[table]\n";
		for ( unsigned i = 0; i < participants.size(); ++i ) {
			std::cout << "[tr][td]" << i + 1 << ".[/td]";
			participants[i].medalprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void medals( unsigned ser ) {
		vector< Team > participants;
		map< string, unsigned > partMap;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			for ( unsigned j = 0; j < 4; ++j ) {
				Team & t = series[ser].seasons[i].teams[j];
				Team & u = teams[teamMap[*t.names.begin()]];
			
				map< string, unsigned >::iterator k = partMap.find( *u.names.begin() );
				if ( k == partMap.end() ) {
					k = partMap.insert( partMap.begin(), make_pair( *u.names.begin(), participants.size() ) );
					participants.push_back( Team( u.names ) );
				}
				participants[k->second].update( t );
			}

		sort( participants.begin(), participants.end(), seasonsort );
		std::cout << "[table]\n";
		for ( unsigned i = 0; i < participants.size(); ++i ) {
			std::cout << "[tr][td]" << i + 1 << ".[/td]";
			participants[i].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void windifference( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i ) {
			unsigned one = 10000*series[ser].seasons[i].teams[0].p + 100*series[ser].seasons[i].teams[0].d + series[ser].seasons[i].teams[0].f;
			unsigned two = 10000*series[ser].seasons[i].teams[1].p + 100*series[ser].seasons[i].teams[1].d + series[ser].seasons[i].teams[1].f;

			s.insert( make_pair( one - two, i ) );
		}

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::iterator i = s.begin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++i ) {
			std::cout << "[tr][td]" << k + 1 << ".[/td][td]" << series[ser].seasons[i->second].number << "[/td][td]" << *series[ser].seasons[i->second].teams[0].names.begin() << "[/td][td]";
			std::cout << series[ser].seasons[i->second].teams[0].p - series[ser].seasons[i->second].teams[1].p << " poäng[/td][td]" << series[ser].seasons[i->second].teams[0].d - series[ser].seasons[i->second].teams[1].d << " mål";
			std::cout << "[/td][/tr]\n";
		}
		std::cout << "[/table]\n";

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]" << k + 1 << ".[/td][td]" << series[ser].seasons[j->second].number << "[/td][td]" << *series[ser].seasons[j->second].teams[0].names.begin() << "[/td][td]";
			std::cout << series[ser].seasons[j->second].teams[0].p - series[ser].seasons[j->second].teams[1].p << " poäng[/td][td]" << series[ser].seasons[j->second].teams[0].d - series[ser].seasons[j->second].teams[1].d << " mål";
			std::cout << "[/td][/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void points( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( series[ser].seasons[i].teams[0].p, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::iterator i = s.begin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++i ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[i->second].number << "[/td]";
			series[ser].seasons[i->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void goalsfor( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( series[ser].seasons[i].teams[0].f, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::iterator i = s.begin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++i ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[i->second].number << "[/td]";
			series[ser].seasons[i->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void goalsagainst( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( series[ser].seasons[i].teams[0].a, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::iterator i = s.begin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++i ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[i->second].number << "[/td]";
			series[ser].seasons[i->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void goaldifference( unsigned ser ) {
		set< pair< int, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( series[ser].seasons[i].teams[0].f - series[ser].seasons[i].teams[0].a, i ) );

		std::cout << "[table]\n";
		set< pair< int, unsigned > >::iterator i = s.begin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++i ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[i->second].number << "[/td]";
			series[ser].seasons[i->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";

		std::cout << "[table]\n";
		set< pair< int, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]1.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[0].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void tiedgames( unsigned ser ) {
		set< pair< unsigned, pair< unsigned, unsigned > > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			for ( unsigned j = 0; j < 8; ++j )
				s.insert( make_pair( series[ser].seasons[i].teams[j].t, make_pair( i, j ) ) );

		std::cout << "[table]\n";
		set< pair< unsigned, pair< unsigned, unsigned > > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]" << j->second.second + 1 << ".[/td][td]" << series[ser].seasons[j->second.first].number << "[/td]";
			series[ser].seasons[j->second.first].teams[j->second.second].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void bestsecond( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( 10000*series[ser].seasons[i].teams[1].p + 100*series[ser].seasons[i].teams[1].d + series[ser].seasons[i].teams[1].f, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]2.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[1].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void bestthird( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( 10000*series[ser].seasons[i].teams[2].p + 100*series[ser].seasons[i].teams[2].d + series[ser].seasons[i].teams[2].f, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]3.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[2].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void bestfifth( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( 10000*series[ser].seasons[i].teams[4].p + 100*series[ser].seasons[i].teams[4].d + series[ser].seasons[i].teams[4].f, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]5.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[4].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void worstsixth( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( 10000*series[ser].seasons[i].teams[5].p + 100*series[ser].seasons[i].teams[5].d + series[ser].seasons[i].teams[5].f, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::iterator i = s.begin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++i ) {
			std::cout << "[tr][td]6.[/td][td]" << series[ser].seasons[i->second].number << "[/td]";
			series[ser].seasons[i->second].teams[5].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void bestseventh( unsigned ser ) {
		set< pair< unsigned, unsigned > > s;
		for ( unsigned i = 0; i < series[ser].seasons.size(); ++i )
			s.insert( make_pair( 10000*series[ser].seasons[i].teams[6].p + 100*series[ser].seasons[i].teams[6].d + series[ser].seasons[i].teams[6].f, i ) );

		std::cout << "[table]\n";
		set< pair< unsigned, unsigned > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			std::cout << "[tr][td]7.[/td][td]" << series[ser].seasons[j->second].number << "[/td]";
			series[ser].seasons[j->second].teams[6].tableprint( cout );
			std::cout << "[/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void sequencewins( unsigned ser ) {
		map< std::string, unsigned > wins;
		for ( unsigned j = 0; j < 1; ++j )
			wins[*series[ser].seasons[0].teams[j].names.begin()] = 1;

		set< pair< unsigned, pair< unsigned, std::string > > > s;
		for ( unsigned i = 1; i < series[ser].seasons.size(); ++i ) {
			set< std::string > top;
			for ( unsigned j = 0; j < 1; ++j ) {
				top.insert( *series[ser].seasons[i].teams[j].names.begin() );
				wins[*series[ser].seasons[i].teams[j].names.begin()]++;
			}
			for ( map< std::string, unsigned >::iterator k = wins.begin(); k != wins.end(); ++k )
				if ( top.find( k->first ) == top.end() ) {
					s.insert( make_pair( k->second, make_pair( i, k->first ) ) );
					wins.erase( k );
				}
		}
		for ( map< std::string, unsigned >::iterator k = wins.begin(); k != wins.end(); ++k )
			s.insert( make_pair( k->second, make_pair( series[ser].seasons.size(), k->first ) ) );

		std::cout << "[table]\n";
		set< pair< unsigned, pair< unsigned, std::string > > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			unsigned num = series[ser].seasons[j->second.first - 1].number;
			std::cout << "[tr][td]" << k + 1 << ".[/td][td]" << j->second.second << "[/td][td]" << j->first << "[/td][td]" << ( num - j->first + 1 ) << "-" << num << "[/td][/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void sequencemedals( unsigned ser ) {
		map< std::string, unsigned > wins;
		for ( unsigned j = 0; j < 4; ++j )
			wins[*series[ser].seasons[0].teams[j].names.begin()] = 1;

		set< pair< unsigned, pair< unsigned, std::string > > > s;
		for ( unsigned i = 1; i < series[ser].seasons.size(); ++i ) {
			set< std::string > top;
			for ( unsigned j = 0; j < 4; ++j ) {
				top.insert( *series[ser].seasons[i].teams[j].names.begin() );
				wins[*series[ser].seasons[i].teams[j].names.begin()]++;
			}
			for ( map< std::string, unsigned >::iterator k = wins.begin(); k != wins.end(); ++k )
				if ( top.find( k->first ) == top.end() ) {
					s.insert( make_pair( k->second, make_pair( i, k->first ) ) );
					wins.erase( k );
				}
		}
		for ( map< std::string, unsigned >::iterator k = wins.begin(); k != wins.end(); ++k )
			s.insert( make_pair( k->second, make_pair( series[ser].seasons.size(), k->first ) ) );

		std::cout << "[table]\n";
		set< pair< unsigned, pair< unsigned, std::string > > >::reverse_iterator j = s.rbegin();
		for ( unsigned k = 0; k < TOPLIST; ++k, ++j ) {
			unsigned num = series[ser].seasons[j->second.first - 1].number;
			std::cout << "[tr][td]" << k + 1 << ".[/td][td]" << j->second.second << "[/td][td]" << j->first << "[/td][td]" << ( num - j->first + 1 ) << "-" << num << "[/td][/tr]\n";
		}
		std::cout << "[/table]\n";
	}

	void appearances( unsigned ser ) {
		map< unsigned, vector< unsigned > > apps;
		for ( unsigned i = 0; i < teams.size(); ++i )
			apps[teams[i].appearances( ser )].push_back( i );

		unsigned i = 0;
		std::cout << "[table]\n";
		for ( map< unsigned, vector< unsigned > >::reverse_iterator it = apps.rbegin(); i < 2; ++i, ++it )
			for ( unsigned j = 0; j < it->second.size(); ++j ) {
				std::cout << "[tr][td]" << *teams[it->second[j]].names.begin() << "[/td][td]" << it->first << "[/td][/tr]\n";
			}
		std::cout << "[/table]\n";
	}
	
};

#endif
