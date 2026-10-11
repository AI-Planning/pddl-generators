#ifndef GLOBAL_INCLUDE
#define GLOBAL_INCLUDE

#include "../universal-pddl-parser/Instance.h"
#include <fstream>

///////////////////////////////////////////////////
////////////// GLOBAL DEFINITIONS /////////////////
///////////////////////////////////////////////////
typedef std::string String;
typedef std::vector< String > StringVec;
typedef std::vector< StringVec > StringDVec;
typedef std::vector< StringDVec > StringTVec;
typedef std::vector< StringTVec > StringQVec;
typedef std::set< String > StringSet;
typedef std::map< String , StringVec > MapStrSVec;
typedef std::vector< Instance * > InstVec;
typedef std::stringstream SStream;
typedef std::istringstream ISStream;
typedef std::ostringstream OSStream;
typedef std::ifstream IFStream;
typedef std::ofstream OFStream;
typedef std::set< int > IntSet;
typedef std::pair< int, int > PII;
typedef std::set< PII > PIISet;
typedef std::vector< unsigned > UIntVec;

///////////////////////////////////////////////////
/////////////// GLOBAL VARIABLES //////////////////
///////////////////////////////////////////////////
String goal_pred = "DONE-PROGRAMMING";						// goal predicate
String accu_pred = "ACCUMULATOR";							// accumulator predicate
String eval_pred = "DONE-EVALUATING";						// evaluating predicate
String stack_row_pred = "NEXT-STACK-ROW";					// link between stack rows predicate
String stack_top_pred = "TOP-STACK";						// top row of the stack
String stack_state_pred = "STATE-STACK";		   			// state predicate of the stack
String stack_empty_cond_pred = "EMPTY-COND-STACK"; 			// empty condition predicate of the stack
String stack_empty_tgoto_pred = "EMPTY-TRUE-GOTO-STACK"; 	// empty tgoto predicate of the stack
String stack_empty_fgoto_pred = "EMPTY-FALSE-GOTO-STACK"; 	// empty fgoto predicate of the stack
String stack_empty_tact_pred = "EMPTY-TACT-STACK"; 			// empty tact predicate of the stack
String stack_empty_fact_pred = "EMPTY-FACT-STACK"; 			// empty fact predicate of the stack
String available_pred = "AVAILABLE-STATE";					// available state predicate
String next_state_pred = "NEXT-STATE"; 						// next state relation predicate
String true_no_act_pred = "TRUE-NO-ACT";					// predicate for true no action execution
String false_no_act_pred = "FALSE-NO-ACT";	   				// predicate for false no action execution

IntSet effs; 				// non-static fluents
IntSet unclean_effs; 		// avoid unclean derived predicates effects
bool EVAL_DERIVED = false;

// STACK VARIABLES
bool INSTR_WITH_PARAMS;			// Adapt the compilation with instructions with parameters
bool IS_HIGH_LEVEL;				// Adapt the compilation to search high-level state features
bool IS_UNSUPERVISED;			// Adapt the compilation to group programs by classes
unsigned total_stack_pred;		// number of total predicates in the stack
unsigned total_stack_vars;		// number of total variables in the recursive calls
unsigned stack_size;			// max number of procedures in the stack
StringSet stack_predicates;		// stack predicates
StringSet stack_variables;		// variables for recursive calls
StringDVec stack_constants;		// constants for each stack predicate
StringVec stack_procedures;		// Main and procedures in specific stack row
StringVec stack_lines;			// Main/Procedure line in specific stack row
String compiler_type;			// Compiler type ( Planning Programs [ ca, cl, dck, hl] || HFSCs )

// ACTIONS TO INSTRUCTIONS
MapStrSVec act2ins; 			// Variable to map action into programming instructions

///////////////////////////////////////////////////
/////////////// GLOBAL FUNCTIONS //////////////////
///////////////////////////////////////////////////
IntSet preds( Domain *d, Condition * c ) {
	And * a = dynamic_cast< And * >( c );
	Forall * f = dynamic_cast< Forall * >( c );
	Ground * g = dynamic_cast< Ground * >( c );
	Or * o = dynamic_cast< Or * >( c );
	Not * n = dynamic_cast< Not * >( c );
	When * w = dynamic_cast< When * >( c );

	if ( f ) return preds( d, f->cond );
	if ( n ) return preds( d, n->cond );
	if ( w ) return preds( d, w->cond );

	IntSet out;
	if ( g ) out.insert( d->preds.index( g->name ) );
	for ( unsigned i = 0; a && i < a->conds.size(); ++i ) {
		IntSet s = preds( d, a->conds[i] );
		out.insert( s.begin(), s.end() );
	}
	if ( o ) {
		out = preds( d, o->first );
		IntSet s = preds( d, o->second );
		out.insert( s.begin(), s.end() );
	}
	return out;
}



void addStackRow( Condition * c, int par ) {
	And * a = dynamic_cast< And * >( c );
	Forall * f = dynamic_cast< Forall * >( c );
	Ground * g = dynamic_cast< Ground * >( c );
	Or * o = dynamic_cast< Or * >( c );
	Not * n = dynamic_cast< Not * >( c );
	When * w = dynamic_cast< When * >( c );
	Exists * ex = dynamic_cast< Exists * >( c );

	if ( f ) addStackRow( f->cond, par );
	if ( n ) addStackRow( n->cond, par );
	if ( w ) addStackRow( w->pars, par );
	if ( w ) addStackRow( w->cond, par );
	if ( ex ) addStackRow( ex->cond, par );
	if ( g && stack_predicates.find( g->name ) != stack_predicates.end() )
		g->params.push_back( par );
	for ( unsigned i = 0; a && i < a->conds.size(); ++i )
		addStackRow( a->conds[ i ], par );
	if ( o ) {
		addStackRow( o->first, par );
		addStackRow( o->second, par );
	}
}

#endif
