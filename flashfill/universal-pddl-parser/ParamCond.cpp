
#include "Domain.h"

void ParamCond::printParams( unsigned first, std::ostream & s, TokenStruct< std::string > & ts, Domain & d ) {
	s << "(";
	for ( unsigned i = first; i < params.size(); ++i ) {
		std::stringstream ss;
		ss << "?" << d.types[params[i]]->getName() << ts.size();
		ts.insert( ss.str() );
		s << " " << ss.str();
		if ( d.typed ) s << " - " << d.types[params[i]]->name;
	}
	s << " )\n";
}
