
#ifndef _FUNCTION_H_
#define _FUNCTION_H_

class Function : public Lifted {

public:

	int returnType;

	Function()
		: Lifted(), returnType( -1 ) {}

	Function( const std::string & s, int type = -1 )
		: Lifted( s ), returnType( type ) {}

	Function( const ParamCond * c )
		: Lifted( c ), returnType( -1 ) {}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );
	
};

#endif
