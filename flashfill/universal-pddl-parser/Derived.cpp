
#include "Domain.h"

Derived::Derived( const Derived * z, Domain & d )
	: Lifted( z ), cond( 0 ), lifted( d.preds.get( z->name ) ) {
	if ( z->cond ) cond = z->cond->copy( d );
}

void Derived::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	s << "( :DERIVED ( " << name;

	TokenStruct< std::string > dstruct( ts );

	for ( unsigned i = 0; i < params.size(); ++i ) {
		std::stringstream ss;
		ss << "?" << d.types[params[i]]->getName() << dstruct.size();
		dstruct.insert( ss.str() );
		s << " " << ss.str();
		if ( d.typed ) s << " - " << d.types[params[i]]->name;
	}
	s << " )\n";

	if ( cond ) cond->PDDLPrint( s, 1, dstruct, d );

	s << "\n)\n";
}

void Derived::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	name = f.getToken( d.preds );
	TokenStruct< std::string > dstruct = f.parseTypedList( true, d.types );
	params = d.convertTypes( dstruct.types );

	f.next();
	f.assert( "(" );
	cond = createCondition( f, d );
	cond->parse( f, dstruct, d );

	f.next();
	f.assert( ")" );
}
