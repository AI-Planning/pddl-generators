
#ifndef _INCREASE_H_
#define _INCREASE_H_

#include "Ground.h"

// RIGHT NOW ONLY INCREASES TOTAL-COST !!!

class Increase : public Condition {

public:

	int value;
	Ground * ground;

	Increase( int val = 1 )
		: value( val ), ground( 0 ) {}

	Increase( Function * f, const IntVec & p = IntVec() )
		: value( 0 ), ground( new Ground( f, p ) ) {}

	Increase( const Increase * i, Domain & d );

	~Increase() {
		if ( ground ) delete ground;
	}

	void print( std::ostream & s ) const {
		s << "Increase ";
		if ( ground ) ground->print( s );
		else s << value;
		s << "\n";
	}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ){
		
	}

	void addParams( int m, unsigned n ) {
	}

	Condition * copy( Domain & d ) {
		return new Increase( this, d );
	}

};

#endif
