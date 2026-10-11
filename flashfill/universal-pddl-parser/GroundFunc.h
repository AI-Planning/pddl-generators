
#ifndef _GROUNDFUNC_H_
#define _GROUNDFUNC_H_

#include "TypeGround.h"

template < typename T >
class GroundFunc : public TypeGround {

public:

	T value;

	GroundFunc()
		: TypeGround(), value( 0 ) {}

	GroundFunc( Lifted * l, const T & val = T( 0 ) )
		: TypeGround( l ), value( val ) {}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

};

#endif
