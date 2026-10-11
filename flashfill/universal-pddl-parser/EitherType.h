
#ifndef _EITHERTYPE_H_
#define _EITHERTYPE_H_

#include "Type.h"

class EitherType : public Type {

public:

	EitherType( const std::string & s )
		: Type( s ) {}

	EitherType( const EitherType * t )
		: Type( t ) {}
	
	std::string getName() const {
		std::string out = "EITHER";
		for ( unsigned i = 0; i < subtypes.size(); ++i )
			out += "_" + subtypes[i]->getName();
		return out;
	}

	void PDDLPrint( std::ostream & s ) const {
	}

	Type * copy() {
		return new EitherType( this );
	}

};

#endif
