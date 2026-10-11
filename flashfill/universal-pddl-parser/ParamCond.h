
#ifndef _PARAMCOND_H_
#define _PARAMCOND_H_

#include "Condition.h"

class ParamCond : public Condition {

public:

	std::string name;
	IntVec params;

	ParamCond() {}

	ParamCond( const std::string & s, const IntVec & p = IntVec() )
		: name( s ), params( p ) {}

	ParamCond( const ParamCond * c )
		: name( c->name ), params( c->params ) {}

	std::string c_str() const {
		return name;
	}

	void print( std::ostream & stream ) const {
		stream << name << params << "\n";
	}

	void printParams( unsigned first, std::ostream & s, TokenStruct< std::string > & ts, Domain & d );

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ){
		
	}

};

typedef std::vector< ParamCond * > ParamCondVec;

#endif
