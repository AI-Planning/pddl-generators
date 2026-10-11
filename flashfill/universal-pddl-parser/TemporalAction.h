
#ifndef _TEMPORALACTION_H_
#define _TEMPORALACTION_H_

#include "Action.h"
#include "And.h"

class Instance;

class Expression {

public:

	virtual ~Expression() {}
	virtual std::string info() = 0;
	virtual void PDDLPrint( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d ) = 0;
	virtual double evaluate() = 0;
	virtual double evaluate( Instance & ins, const StringVec & par ) = 0;
	virtual IntSet params() = 0;

};

class CompositeExpression : public Expression {

public:

	char op;
	Expression * left;
	Expression * right;

	CompositeExpression( char c, Expression * l, Expression * r ) : op( c ), left( l ), right( r ) {}

	~CompositeExpression() {
		delete left;
		delete right;
	}

	std::string info() {
		std::ostringstream os;
		os << "(" << op << " " << left->info() << " " << right->info() << ")";
		return os.str();
	}

	void PDDLPrint( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d ) {
		s << "( " << op << " ";
		left->PDDLPrint( s, ts, d );
		s << " ";
		right->PDDLPrint( s, ts, d );
		s << " )";
	}

	double compute( double x, double y ) {
		double res = 0;
		switch( op ) {
		case '+': res = x + y; break;
		case '-': res = x - y; break;
		case '*': res = x * y; break;
		case '/': res = ( y == 0 ? 0 : x / y ); break;
		}
		return res;
	}

	double evaluate() {
		return compute( left->evaluate(), right->evaluate() );
	}

	double evaluate( Instance & ins, const StringVec & par ) {
		return compute( left->evaluate( ins, par ), right->evaluate( ins, par ) );
	}

	IntSet params() {
		IntSet lpars = left->params();
		IntSet rpars = right->params();
		lpars.insert( rpars.begin(), rpars.end() );
		return lpars;
	}

};

class FunctionExpression : public Expression {

public:

	ParamCond * fun;

	FunctionExpression( ParamCond * c ) : fun( c ) {}

	~FunctionExpression() {
		delete fun;
	}

	std::string info() {
		std::ostringstream os;
		os << "(" << fun->name << fun->params << ")";
		return os.str();
	}

	void PDDLPrint( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d );

	double evaluate() { return 1; }

	double evaluate( Instance & ins, const StringVec & par );

	IntSet params() {
		return IntSet( fun->params.begin(), fun->params.end() );
	}

};

class ValueExpression : public Expression {

public:

	double value;

	ValueExpression( double v ) : value( v ) {}

	std::string info() {
		std::ostringstream os;
		os << value;
		return os.str();
	}

	void PDDLPrint( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d ) {
		s << value;
	}

	double evaluate() { return value; }

	double evaluate( Instance & ins, const StringVec & par ) {
		return value;
	}

	IntSet params() {
		return IntSet();
	}

};

class TemporalAction : public Action {

public:

	Expression * durationExpr;
	// pre_s and eff_s inherited from Action
	And *pre_o, *pre_e, *eff_e;

	TemporalAction( const std::string & s )
		: Action( s ), durationExpr( 0 ), pre_o( 0 ), pre_e( 0 ), eff_e( 0 ) {}

	~TemporalAction() {
		if ( durationExpr ) delete durationExpr;
		if ( pre_o ) delete pre_o;
		if ( pre_e ) delete pre_e;
		if ( eff_e ) delete eff_e;
	}

	void print( std::ostream & s ) const {
		s << name << params << "\n";
		s << "Duration: " << durationExpr->info() << "\n";
		s << "Pre_s: " << pre;
		s << "Eff_s: " << eff;
		s << "Pre_o: " << pre_o;
		s << "Pre_e: " << pre_e;
		s << "Eff_e: " << eff_e;
	}

	double duration() {
		if ( durationExpr ) return durationExpr->evaluate();
		else return 0;
	}

	Expression * parseDuration( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

	void printCondition( std::ostream & s, const TokenStruct< std::string > & ts, Domain & d,
	                     const std::string & t, And * a );

	void PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d );

	void parseCondition( Filereader & f, TokenStruct< std::string > & ts, Domain & d, And * a );

	void parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d );

};

#endif
