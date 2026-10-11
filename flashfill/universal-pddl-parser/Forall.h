
#ifndef _FORALL_H_
#define _FORALL_H_

#include "ParamCond.h"

class Forall : public ParamCond {

public:

	Condition * cond;
	Condition * cond1;

	Forall()
		: cond( 0 ), cond1( 0 ) {}

	Forall( const Forall * f, Domain & d )
		: ParamCond( f ), cond( 0 ), cond1( 0 ) {
		if ( f->cond ) cond = f->cond->copy( d );
	}

	~Forall() {
		if ( cond ) delete cond;
		if ( cond1 ) delete cond1;
	}

	void print( std::ostream & s ) const {
		s << "Forall" << params << ":\n";
		if ( cond ) cond->print( s );
		if ( cond1 ) cond1->print( s );
	}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void addParams( int m, unsigned n ) {
		cond->addParams( m, n );
	}

	Condition * copy( Domain & d ) {
		return new Forall( this, d );
	}

};

#endif
