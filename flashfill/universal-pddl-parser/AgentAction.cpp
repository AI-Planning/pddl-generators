
#include "Domain.h"

void AgentAction::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	s << "( :ACTION " << name << "\n";

	TokenStruct< std::string > astruct;

	std::stringstream ss;
	ss << "?" << d.types[params[0]]->name;
	astruct.insert( ss.str() );

	s << "  :AGENT " << astruct[0];
	if ( d.typed ) s << " - " << d.types[params[0]]->name;
	s << "\n";

	s << "  :PARAMETERS ";

	printParams( 1, s, astruct, d );

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

void AgentAction::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	TokenStruct< std::string > astruct;

	f.next();
	f.assert( ":AGENT" );
	astruct.insert( f.getToken() );
	if ( d.typed ) {
		f.next();
		f.assert( "-" );
		astruct.types.push_back( f.getToken( d.types ) );
	}
	else astruct.types.push_back( "OBJECT" );

	f.next();
	f.assert( ":PARAMETERS" );
	f.assert( "(" );
	astruct.append( f.parseTypedList( true, d.types ) );
	params = d.convertTypes( astruct.types );

	parseConditions( f, astruct, d );
}
