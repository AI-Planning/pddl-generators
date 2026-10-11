 
#include "Domain.h"

void HTNOperator::SHOPPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	s << "( :OPERATOR (" << name << " ";
	
    //params
	TokenStruct< std::string > astruct;
	std::stringstream ss;

	printParams( 0, s, astruct, d );
	
	s<<")\n";
	//preconds
	if ( pre ) pre->PDDLPrint( s, 1, astruct, d );
	else s << "\t()";
	s << "\n";

	//del effects
	if ( deleff ) deleff->PDDLPrint( s, 1, astruct, d );
	else s << "\t()";
	s << "\n";

	//add effects
	if ( eff ) eff->PDDLPrint( s, 1, astruct, d );
	else s << "\t()";
	s << "\n";

	s << ")\n";
}

void HTNOperator::SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	TokenStruct< std::string > astruct = f.parseTypedList( false );
	params = d.convertTypes( astruct.types );
	parseSHOPConditions( f, astruct, d );
}

void HTNOperator::parseSHOPConditions( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	
	if ( f.getChar() != ')' ) {
			if(f.getChar() == '('){
				pre = new And;
			}
			else 
				pre = createSHOPCondition( f, d );
			pre->SHOPparse( f, ts, d );
	}
	else ++f.c;
	
	f.next();
	
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
			if(f.getChar() == '('){
				deleff = new And;
			}
			else 
				deleff = createSHOPCondition( f, d );
			deleff->SHOPparse( f, ts, d );
	}
	else ++f.c;

	f.next();

	f.assert( "(" );
	if ( f.getChar() != ')' ) {
			if(f.getChar() == '('){
				eff = new And;
			}
			else 
				eff = createSHOPCondition( f, d );
			eff->SHOPparse( f, ts, d );
	}
	else ++f.c;
		
	f.next();
	f.assert( ")" );
}
