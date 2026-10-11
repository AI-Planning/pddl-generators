
#include "Domain.h"

void Exists::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( EXISTS\n";

	TokenStruct< std::string > fstruct( ts );

	tabindent( s, indent + 1 );
	printParams( 0, s, fstruct, d );

	if ( cond ) cond->PDDLPrint( s, indent + 1, fstruct, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	s << "\n";
	tabindent( s, indent );
	s << ")";
}

void Exists::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );

	TokenStruct< std::string > es = f.parseTypedList( true, d.types );
	params = d.convertTypes( es.types );
		
	TokenStruct< std::string > estruct( ts );
	estruct.append( es );

	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		cond = createCondition( f, d );
		cond->parse( f, estruct, d );
	}
	else ++f.c;

	f.next();
	f.assert( ")" );
}
