
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main( int argc, char *argv[] ) {
	for ( unsigned i = 11; i <= 56; ++i ) {
		ostringstream read, write;
		read << argv[1] << "/m" << i << ".html";
		write << argv[1] << "/m" << i << ".txt";

		ifstream fread( read.str().c_str() );
		ofstream fwrite( write.str().c_str() );

		for ( unsigned i = 0; i < 14; ++i ) {
			string s;
			for ( getline( fread, s ); s.substr( 0, 3 ) != "Omg"; getline( fread, s ) );
			fwrite << s << "\n";
			getline( fread, s );
			fwrite << s << "\n";
			for ( unsigned j = 0; j < 4; ++j ) {
				getline( fread, s );
				fwrite << s;
				getline( fread, s );
				getline( fread, s );
				fwrite << s << "\n";
				getline( fread, s );
				getline( fread, s );
			}
		}

		fread.close();
		fwrite.close();
	}
}
