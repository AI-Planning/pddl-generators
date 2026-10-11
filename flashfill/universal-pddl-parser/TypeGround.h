
#ifndef _TYPEGROUND_H_
#define _TYPEGROUND_H_

#include "Ground.h"

class TypeGround : public Ground {

public:

	TypeGround()
		: Ground() {}

	TypeGround( Lifted * l, const IntVec & p = IntVec() )
		: Ground( l, p ) {}

//	TypeGround( const TypeGround * tg )
//		: Ground( tg ) {}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void insert( Domain & d, const StringVec & v );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

};

typedef std::vector< TypeGround * > TypeGroundVec;

#endif
