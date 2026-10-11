  
#ifndef _HTNMETHOD_H_
#define _HTNMETHOD_H_

#include "Action.h"

class HTNMethod : public Action {
	TaskVec tasks;
public:
  
	HTNMethod( const std::string & s ) : Action( s ) {}
	
	void print( std::ostream & s ) const {
		s << name << params << "\n";
		s << "Pre: " << pre;
		if ( eff ) s << "Eff: " << eff;
	}

	void SHOPPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void parseSHOPConditions( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void parseTaskList( Filereader & f, TokenStruct< std::string > & ts, Domain & d );
};

#endif


