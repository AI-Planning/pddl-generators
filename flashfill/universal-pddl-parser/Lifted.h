
#ifndef _LIFTED_H_
#define _LIFTED_H_

#include "ParamCond.h"

class Lifted : public ParamCond {

public:

	Lifted() {}

	Lifted( const std::string & s )
		: ParamCond( s ) {}

	Lifted( const ParamCond * c )
		: ParamCond( c ) {}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void addParams( int m, unsigned n ) {
	}

	Condition * copy( Domain & d ) {
		return new Lifted( this );
	}

};

typedef std::vector< Lifted * > LiftedVec;

#endif
