 
#ifndef _HTNOPERATOR_H_
#define _HTNOPERATOR_H_

#include "Action.h"

class HTNOperator : public Action {
	Condition * deleff;
public:
  
	HTNOperator( const std::string & s ) : Action( s ), deleff(0) {}

	virtual ~HTNOperator() {
		if ( deleff ) delete deleff;
	}

	void print( std::ostream & s ) const {
		s << name << params << "\n";
		s << "Pre: " << pre;
		if ( eff ) s << "Eff: " << eff;
		if ( deleff ) s << "DelEff: " << deleff;
	}

	void SHOPPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void parseSHOPConditions( Filereader & f, TokenStruct< std::string > & ts, Domain & d );
};

#endif

