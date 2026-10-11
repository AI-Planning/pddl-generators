
#ifndef _EQUALS_H_
#define _EQUALS_H_

#include "Ground.h"

class Equals : public Ground {

public:

	Equals( const IntVec & p = IntVec() )
		: Ground( "=", p ) {}

	Equals( const Equals * e )
		: Ground( "=", e->params ) {}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	Condition * copy( Domain & d ) {
		return new Equals( this );
	}

};

#endif
