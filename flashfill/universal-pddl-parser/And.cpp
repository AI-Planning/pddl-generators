
#include "Domain.h"

void And::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	tabindent( s, indent );
	s << "( AND\n";
	for ( unsigned i = 0; i < conds.size(); ++i ) {
		conds[i]->PDDLPrint( s, indent + 1, ts, d );
		s << "\n";
	}
	tabindent( s, indent );
	s << ")";
}

void And::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	for ( f.next(); f.getChar() != ')'; f.next() ) {
		f.assert( "(" );
		Condition * condition = createCondition( f, d );
		condition->parse( f, ts, d );
		conds.push_back( condition );
	}
	++f.c;
}

void And::SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	TokenStruct< std::string > fstruct;
	for ( f.next(); f.getChar() != ')'; f.next() ) {
		f.assert( "(" );
		Condition * condition = createSHOPCondition( f, d );
		
		condition->SHOPparse( f, ts, d );
		conds.push_back( condition );
	}
	++f.c;
}
