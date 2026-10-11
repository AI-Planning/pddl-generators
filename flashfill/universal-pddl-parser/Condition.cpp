
#include "Domain.h"

Condition * createCondition( Filereader & f, Domain & d ) {
	std::string s = f.getToken();

	if ( s == "=" ) return new Equals;
	if ( s == "AND" ) return new And;
	if ( s == "EXISTS" ) return new Exists;
	if ( s == "FORALL" ) return new Forall;
	if ( s == "INCREASE" ) return new Increase;
	if ( s == "NOT" ) return new Not;
	if ( s == "ONEOF" ) return new Oneof;
	if ( s == "OR" ) return new Or;
	if ( s == "WHEN" ) return new When;

	int i = d.preds.index( s );
	if ( i >= 0 ) return new Ground( d.preds[i] );

	f.tokenExit( s );

	return 0;
}

Condition * createSHOPCondition( Filereader & f, Domain & d ) {
	std::string s = f.getToken();

	if ( s == "AND" ) return new And;
	if ( s == "FORALL" ) return new Forall;
	if ( s == "NOT" ) return new Not;
	if ( s == "OR" ) return new Or;
	
	return new Ground( s );
	
	f.tokenExit( s );

	return 0;
}

