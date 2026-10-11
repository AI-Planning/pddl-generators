
#include "Domain.h"

void Action::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	s << "( :ACTION " << name << "\n";

	s << "  :PARAMETERS ";

	TokenStruct< std::string > astruct;

	printParams( 0, s, astruct, d );

	s << "  :PRECONDITION\n";
	if ( pre ) pre->PDDLPrint( s, 1, astruct, d );
	else s << "\t()";
	s << "\n";

	s << "  :EFFECT\n";
	if ( eff ) eff->PDDLPrint( s, 1, astruct, d );
	else s << "\t()";
	s << "\n";

	s << ")\n";
}

void Action::parseConditions( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( ":" );
	std::string s = f.getToken();
	if ( s == "PRECONDITION" ) {
		f.next();
		f.assert( "(" );
		if ( f.getChar() != ')' ) {
			pre = createCondition( f, d );
			pre->parse( f, ts, d );
		}
		else ++f.c;

		f.next();
		f.assert( ":" );
		s = f.getToken();
	}
	if ( s != "EFFECT" ) f.tokenExit( s );
		
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		eff = createCondition( f, d );
		eff->parse( f, ts, d );
	}
	else ++f.c;
	f.next();
	f.assert( ")" );
}

void Action::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( ":PARAMETERS" );
	f.assert( "(" );

	TokenStruct< std::string > astruct = f.parseTypedList( true, d.types );
	params = d.convertTypes( astruct.types );

	parseConditions( f, astruct, d );
}

CondVec Action::precons() {
	And * a = dynamic_cast< And * >( pre );
	if ( a ) return a->conds;

	CondVec precons;
	if ( pre ) precons.push_back( pre );
	return precons;
}

GroundVec Action::addEffects() {
	GroundVec adds;
	And * a = dynamic_cast< And * >( eff );
	for ( unsigned i = 0; a && i < a->conds.size(); ++i ) {
		Ground * g = dynamic_cast< Ground * >( a->conds[i] );
		if ( g ) adds.push_back( g );
	}

	Ground * g = dynamic_cast< Ground * >( eff );
	if ( g ) adds.push_back( g );

	return adds;
}

GroundVec Action::deleteEffects() {
	GroundVec deletes;
	And * a = dynamic_cast< And * >( eff );
	for ( unsigned i = 0; a && i < a->conds.size(); ++i ) {
		Not * n = dynamic_cast< Not * >( a->conds[i] );
		if ( n ) deletes.push_back( n->cond );
	}

	Not * n = dynamic_cast< Not * >( eff );
	if ( n ) deletes.push_back( n->cond );

	return deletes;
}
