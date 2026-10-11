#ifndef _TASK_H_
#define _TASK_H_

#include "ParamCond.h"

class Task : public ParamCond {

public:

	Task() {}

	Task( const std::string & s )
		: ParamCond( s ) {}

	Task( const ParamCond * c )
		: ParamCond( c ) {}

    void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ){
    	
    }
	
	void SHOPPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ){
		
	}

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void addParams( int m, unsigned n ) {
	}

	Condition * copy( Domain & d ) {
		return new Task( this );
	}

};

typedef std::vector< Task * > TaskVec;

#endif