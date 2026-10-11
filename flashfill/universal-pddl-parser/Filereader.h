
#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include "TokenStruct.h"
#include "Type.h"

class Domain;

class Filereader {

public:

	std::string s;      // current line of file
	std::ifstream f;    // file input stream
	unsigned r, c;      // current row and column of file
  
	Filereader( const std::string & file ) : f( file.c_str() ), r( 1 ), c( 0 ) {
		std::getline( f, s );
		next();
	}

	~Filereader() {
		f.close();
	}

	// characters to be ignored
	bool ignore( char c ) {
		return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f';
	}

	// parenthesis
	bool paren( char c ) {
		return c == '(' || c == ')' || c == '{' || c == '}';
	}

	// current character
	char getChar() {
		return s[c];
	}

	// print line and column
	void printLine() {
		std::cout << "Line " << r << ", column " << c+1 << ": ";
	}

	void tokenExit( const std::string & t ) {
		c -= t.size();
		printLine();
		std::cout << t << " does not name a known token\n";
		exit( 1 );
	}

	// get next non-ignored character
	void next() {
		for ( ; c < s.size() && ignore( s[c] ); ++c );
		while ( c == s.size() || s[c] == ';' ) {
			++r;
			c = 0;
			if ( f.eof() ) {
				printLine();
				std::cout << "Unexpected EOF\n";
				exit(1);
			}
			std::getline( f, s );
			for ( ; c < s.size() && ignore( s[c] ); ++c );
		}
	}

	// get token converted to uppercase
	std::string getToken() {
		std::ostringstream os;
		while ( c < s.size() && !ignore( s[c] ) && !paren( s[c] ) && s[c] != ',' )
			os << ( 97 <= s[c] && s[c] <= 122 ? (char)( s[c++] - 32 ) : s[c++] );
		return os.str();
	}

	// get token converted to uppercase
	// check that the token exists
	template < typename T >
	std::string getToken( const TokenStruct< T > & ts ) {
		std::string t = getToken();
		if ( ts.index( t ) < 0 )
			tokenExit( t );
		return t;
	}

	// assert syntax
	void assert( const std::string & t ) {
		unsigned b = 0;
		for ( unsigned k = 0; c + k < s.size() && k < t.size(); ++k )
			b += s[c + k] == t[k] || 
			     ( 97 <= s[c + k] && s[c + k] <= 122 && s[c + k] == t[k] + 32 );
		if ( b < t.size() ) {
			printLine();
			std::cout << t << " expected\n";
			exit( 1 );
		}
		c += t.size();
		next();
	}

	// parse the name of a domain or instance
	std::string parseName( const std::string & u ) {
		std::string out;
		std::string t[5] = { "(", "DEFINE", "(", u, ")" };
		for ( unsigned i = 0; i < 5; ++i ) {
			assert( t[i] );
			if ( i == 3 ) {
				out = getToken();
				next();
			}
		}
		return out;
	}

	// parse htn name of a domain
	std::string parseHTNDomainName() {
		std::string out;
		std::string t[3] = { "(", "DEFDOMAIN" , "(" };
		for ( unsigned i = 0; i < 3; ++i ) {
			assert( t[i] );
			if ( i == 1 ) {
				out = getToken();
				next();
			}
		}
		return out;
	}

	// parse htn name of a problem
	std::string parseHTNProblemName() {
		std::string out;
		std::string t[4] = { "(", "DEFPROBLEM", "PROBLEM" , "(" };
		for ( unsigned i = 0; i < 4; ++i ) {
			assert( t[i] );
			if ( i == 2 ) {
				out = getToken();
				next();
			}
		}
		return out;
	}
	
	// parse a typed list
	// if check is true, checks that types exist
	TokenStruct< std::string > parseTypedList( bool check, const TokenStruct< Type * > & ts = TokenStruct< Type * >(), const std::string & lt = "" ) {
		unsigned k = 0;
		TokenStruct< std::string > out;
		for ( next(); getChar() != ')' && lt.find( getChar() ) == std::string::npos; next() ) {
			if ( getChar() == '-' ) {
				assert( "-" );

				std::string t;
				// check if the type is "EITHER"
				if ( getChar() == '(' ) {
					assert( "(" );
					assert( "EITHER" );

					t = "( EITHER";
					for ( ; getChar() != ')'; next() ) {
						if ( check ) t += " " + getToken( ts );
						else t += " " + getToken();
					}
					t += " )";
					++c;
				}
				else if ( check ) t = getToken( ts );
				else t = getToken();

				out.types.insert( out.types.end(), out.size() - k, t );
				k = out.size();
			}
			else if ( getChar() == '(' ) {
				assert( "(" );
				assert( ":PRIVATE" );
				getToken();
				out.append( parseTypedList( check, ts ) );
			}
			else out.insert( getToken() );
		}
		if ( k < out.size() ) out.types.insert( out.types.end(), out.size() - k, check ? "OBJECT" : "" );
		++c;

		return out;
	}

};

#endif
