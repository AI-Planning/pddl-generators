
#include "Domain.h"

void When::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( WHEN\n";
	if ( pars ) pars->PDDLPrint( s, indent + 1, ts, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	s << "\n";
	if ( cond ) cond->PDDLPrint( s, indent + 1, ts, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	s << "\n";
	tabindent( s, indent );
	s << ")";
}

void When::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		pars = createCondition( f, d );
		pars->parse( f, ts, d );
	}
	else ++f.c;
		
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		cond = createCondition( f, d );
		cond->parse( f, ts, d );
	}
	else ++f.c;

	f.next();
	f.assert( ")" );
}
