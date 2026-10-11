
#ifndef _NETWORK_NODE_H_
#define _NETWORK_NODE_H_

#include "ParamCond.h"

class NetworkNode : public ParamCond {
public:
	unsigned lower, upper;
	ParamCondVec templates;
  
	NetworkNode( const std::string & s )
		: ParamCond( s ), lower( 0 ), upper( 0 ) {}

	NetworkNode( const NetworkNode * n, Domain & d )
		: ParamCond( n ), lower( n->lower ), upper( n->upper ) {
		for ( unsigned i = 0; i < n->templates.size(); ++i )
			templates.push_back( ( ParamCond * )n->templates[i]->copy( d ) );
	}

	~NetworkNode() {
		for ( unsigned i = 0; i < templates.size(); ++i )
			delete templates[i];
	}

	void print( std::ostream & stream ) const {
		stream << "Network node ";
		ParamCond::print( stream );
		stream << "  <" << lower << "," << upper << ">";
		for ( unsigned i = 0; i < templates.size(); ++i )
			stream << "\n  Template: " << templates[i];
	}

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void addParams( int m, unsigned n ) {
	}

	Condition * copy( Domain & d ) {
		return new NetworkNode( this, d );
	}
	
};

#endif
