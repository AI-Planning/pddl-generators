
#include "Instance.h"

void FunctionExpression::PDDLPrint( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d ) {
	ParamCond * c = d.funcs[d.funcs.index( fun->name )];

	s << "( " << fun->name;
	IntVec v( c->params.size() );
	for ( unsigned i = 0; i < v.size(); ++i ) {
		if ( ts.size() && fun->params[i] >= 0 ) s << " " << ts[fun->params[i]];
		else s << " " << d.types[c->params[i]]->object( fun->params[i] ).first;
	}
	s << " )";
}

double FunctionExpression::evaluate( Instance & ins, const StringVec & par ) {
	ParamCond * c = ins.d.funcs[ins.d.funcs.index( fun->name )];

	IntVec v( c->params.size() );
	for ( unsigned i = 0; i < v.size(); ++i ) {
		std::pair< bool, unsigned > p = ins.d.types[c->params[i]]->parseObject( par[fun->params[i]] );
		if ( p.first ) v[i] = p.second;
		else {
			std::pair< bool, int > q = ins.d.types[c->params[i]]->parseConstant( par[fun->params[i]] );
			if ( q.first ) v[i] = q.second;
			else return 1;
		}
	}
	
	for ( unsigned i = 0; i < ins.init.size(); ++i )
		if ( ins.init[i]->name == c->name && ins.init[i]->params == v )
			return ((GroundFunc<double> *)ins.init[i])->value;
	return 1;
}

Expression * TemporalAction::parseDuration( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	if ( f.getChar() == '(' ) {
		++f.c;
		f.next();
		std::string s = f.getToken();
		if ( s == "+" || s == "-" || s == "*" || s == "/" ) {
			f.next();
			Expression * left = parseDuration( f, ts, d );
			Expression * right = parseDuration( f, ts, d );
			f.assert( ")" );
			return new CompositeExpression( s[0], left, right );
		}
		else {
			f.c -= s.size();
			Function * fun = d.funcs.get( f.getToken( d.funcs ) );
			ParamCond * c = new Lifted( fun );
			for ( unsigned i = 0; i < fun->params.size(); ++i ) {
				f.next();
				c->params[i] = ts.index( f.getToken( ts ) );
			}
			f.next();
			f.assert( ")" );
			return new FunctionExpression( c );
		}
	}
	else {
		double d;
		std::string s = f.getToken();
		std::istringstream is( s );
		is >> d;
		return new ValueExpression( d );
	}
}

void TemporalAction::printCondition( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d,
                                     const std::string & t, And * a ) {
	for ( unsigned i = 0; a && i < a->conds.size(); ++i ) {
		s << "\t\t( " << t << " ";
		a->conds[i]->PDDLPrint( s, 0, ts, d );
		s << " )\n";
	}
}

void TemporalAction::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	s << "( :DURATIVE-ACTION " << name << "\n";

	s << "  :PARAMETERS ";

	TokenStruct< std::string > astruct;

	printParams( 0, s, astruct, d );

	s << "  :DURATION ( = ?DURATION ";
	if ( durationExpr ) durationExpr->PDDLPrint( s, astruct, d );
	else s << "1";
	s << " )\n";

	s << "  :CONDITION\n";
	s << "\t( AND\n";
	printCondition( s, astruct, d, "AT START", (And *)pre );
	printCondition( s, astruct, d, "OVER ALL", pre_o );
	printCondition( s, astruct, d, "AT END", pre_e );
	s << "\t)\n";

	s << "  :EFFECT\n";
	s << "\t( AND\n";
	printCondition( s, astruct, d, "AT START", (And *)eff );
	printCondition( s, astruct, d, "AT END", eff_e );
	s << "\t)\n";

	s << ")\n";
}

void TemporalAction::parseCondition( Filereader & f, TokenStruct< std::string > & ts, Domain & d, And * a ) {
	f.next();
	f.assert( "(" );
	Condition * c = createCondition( f, d );
	c->parse( f, ts, d );
	a->conds.push_back( c );
}

void TemporalAction::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( ":PARAMETERS" );
	f.assert( "(" );

	TokenStruct< std::string > astruct = f.parseTypedList( true, d.types );
	params = d.convertTypes( astruct.types );

	f.next();
	f.assert( ":DURATION" );
	f.assert( "(" );
	f.assert( "=" );
	f.assert( "?DURATION" );
	durationExpr = parseDuration( f, astruct, d );
	f.next();
	f.assert( ")" );

	f.next();
	f.assert( ":" );
	std::string s = f.getToken();
	if ( s == "CONDITION" ) {
		pre = new And;
		pre_o = new And;
		pre_e = new And;
		f.next();
		f.assert( "(" );
		if ( f.getChar() != ')' ) {
			s = f.getToken();
			if ( s == "AND" ) {
				for ( f.next(); f.getChar() != ')'; f.next() ) {
					f.assert( "(" );
					s = f.getToken();
					f.next();
					std::string t = f.getToken();

					if ( s == "AT" && t == "START" )
						parseCondition( f, astruct, d, (And *)pre );
					else if ( s == "OVER" && t == "ALL" )
						parseCondition( f, astruct, d, pre_o );
					else if ( s == "AT" && t == "END" )
						parseCondition( f, astruct, d, pre_e );
					else f.tokenExit( s + " " + t );

					f.next();
					f.assert( ")" );
				}
				++f.c;
			}
			else {
				f.next();
				std::string t = f.getToken();

				if ( s == "AT" && t == "START" )
					parseCondition( f, astruct, d, (And *)pre );
				else if ( s == "OVER" && t == "ALL" )
					parseCondition( f, astruct, d, pre_o );
				else if ( s == "AT" && t == "END" )
					parseCondition( f, astruct, d, pre_e );
				else f.tokenExit( s + " " + t );

				f.next();
				f.assert( ")" );
			}
		}
		else ++f.c;

		f.next();
		f.assert( ":" );
		s = f.getToken();
	}
	if ( s != "EFFECT" ) f.tokenExit( s );

	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
		eff = new And;
		eff_e = new And;

		s = f.getToken();
		if ( s == "AND" ) {
			for ( f.next(); f.getChar() != ')'; f.next() ) {
				f.assert( "(" );
				s = f.getToken();
				f.next();
				std::string t = f.getToken();

				if ( s == "AT" && t == "START" )
					parseCondition( f, astruct, d, (And *)eff );
				else if ( s == "AT" && t == "END" )
					parseCondition( f, astruct, d, eff_e );
				else f.tokenExit( s + " " + t );

				f.next();
				f.assert( ")" );
			}
			++f.c;
		}
		else {
			f.next();
			std::string t = f.getToken();

			if ( s == "AT" && t == "START" )
				parseCondition( f, astruct, d, (And *)eff );
			else if ( s == "AT" && t == "END" )
				parseCondition( f, astruct, d, eff_e );
			else f.tokenExit( s + " " + t );

			f.next();
			f.assert( ")" );
		}
	}
	else ++f.c;

	f.next();
	f.assert( ")" );
}
