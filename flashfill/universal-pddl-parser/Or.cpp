
#include "Domain.h"

void Or::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( OR\n";
	if ( first ) first->PDDLPrint( s, indent + 1, ts, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	s << "\n";
	if ( second ) second->PDDLPrint( s, indent + 1, ts, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	s << "\n";
	tabindent( s, indent );
	s << ")";
}

void Or::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		first = createCondition( f, d );
		first->parse( f, ts, d );
	}
	else ++f.c;
		
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		second = createCondition( f, d );
		second->parse( f, ts, d );
	}
	else ++f.c;

	f.next();
	f.assert( ")" );
}

void Or::SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		first = createSHOPCondition( f, d );
		first->SHOPparse( f, ts, d );
	}
	else ++f.c;
		
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		second = createSHOPCondition( f, d );
		second->SHOPparse( f, ts, d );
	}
	else ++f.c;

	f.next();
	f.assert( ")" );
}
