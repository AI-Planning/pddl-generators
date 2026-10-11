
#include "Domain.h"

void Forall::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( FORALL\n";

	TokenStruct< std::string > fstruct( ts );

	tabindent( s, indent + 1 );
	printParams( 0, s, fstruct, d );

	if ( cond ) cond->PDDLPrint( s, indent + 1, fstruct, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	
	if( cond1 ) { s << "\n"; cond1->PDDLPrint( s, indent + 1, fstruct, d ); }

	s << "\n";
	tabindent( s, indent );
	s << ")";
}

void Forall::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );

	TokenStruct< std::string > fs = f.parseTypedList( true, d.types );
	params = d.convertTypes( fs.types );
		
	TokenStruct< std::string > fstruct( ts );
	fstruct.append( fs );

	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		cond = createCondition( f, d );
		cond->parse( f, fstruct, d );
	}
	else ++f.c;

	f.next();
	f.assert( ")" );
}

void Forall::SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );

	TokenStruct< std::string > fs = f.parseTypedList( false );
	
	params = d.convertTypes( fs.types );
		
	TokenStruct< std::string > fstruct( ts );
	fstruct.append( fs );

	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		cond = new And;
		cond->SHOPparse( f, fstruct, d );
	}
	else ++f.c;

	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		cond1 = new And;
		cond1->SHOPparse( f, fstruct, d );
	}
	else ++f.c;

	f.next();
	f.assert( ")" );

}
