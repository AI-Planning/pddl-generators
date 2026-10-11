#include "Domain.h"

void Task::SHOPPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( " << name << " ";
	for ( unsigned i = 0; i < params.size(); ++i ) {
		if ( ts.size() && params[i] >= 0 && (unsigned)params[i] < ts.size() ) s <<" " << ts[params[i]];
		else if (params[i] >= 0 && (unsigned)params[i] >= ts.size()) s << " ?" << params[i]; 
	}
	s << " )";
}


void Task::SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	TokenStruct< std::string > lstruct = f.parseTypedList( false );
	params = d.convertTypes( lstruct.types );

	for ( unsigned i = 0; i < lstruct.size(); ++i ) {
		std::string s = lstruct[i];
		int k = ts.index( s );
		if ( k >= 0 ) params[i] = k;
		else  { 
			params[i] = ts.insert(s); 
		}
	}
}
