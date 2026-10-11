
#include "Domain.h"

Increase::Increase( const Increase * i, Domain & d )
	: value( i->value ), ground( 0 ) {
	if ( i->ground ) ground = new Ground( d.funcs.get( i->ground->name ) );
}

void Increase::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( INCREASE ( TOTAL-COST ) ";
	if ( ground ) ground->PDDLPrint( s, 0, ts, d );
	else s << ( int )value;
	s << " )";
}

void Increase::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	f.assert( "TOTAL-COST" );
	f.assert( ")" );
	if ( f.getChar() == '(' ) {
		f.assert( "(" );
		ground = new Ground( d.funcs.get( f.getToken( d.funcs ) ) );
		ground->parse( f, ts, d );
	}
	else {
		std::stringstream ss( f.getToken() );
		ss >> value;
	}
	f.next();
	f.assert( ")" );
}
