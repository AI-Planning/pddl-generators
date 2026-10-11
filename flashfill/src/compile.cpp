/****** COMPILER INCLUDES ***********/
#include "include.h"
#include "domain_creator.h"
#include "types.h"
#include "predicates.h"
#include "actions.h"
#include "instance_creator.h"
#include "derived_actions.h"
#include "sequential_actions.h"
#include "goto_actions.h"
#include "terminal_actions.h"
#include "call_actions.h"
#include "choice_actions.h"
#include "hl_cond_actions.h"
#include "unsupervised_actions.h"
#include "cfg_actions.h"

/****** COMPILER GLOBAL VARIABLES ***********/
Domain * d;						// Original domain
Domain * cd;					// Compiled domain
Instance * cins;				// Compiled instance
InstVec ins;					// Vector of instances

unsigned max_bound;				// Max number of lines for the planning program
unsigned bound;					// Max number of lines for this procedure
unsigned total_instances;		// Number of total instances
unsigned procedures;			// Number of procedures
unsigned novariables;			// Number of variables
unsigned noslots;				// Number of slots
unsigned noclasses;				// Number of classes
bool to_program;				// Boolean to know if program instructions can be called

// PREDICATE & CONSTANT VARIABLES
StringVec rows;					// Constant objects ROWs for the stack
StringVec tests;				// Tests predicates
StringVec empty_lines;			// Empty lines predicates
StringVec select_programs;		// Select programs for CFG procedures
//StringVec allowed_lines;		// Lines allowed to program/repeat for CFG
StringDVec allowed_lines;		// Lines allowed to program/repeat for CFG
StringDVec endinstr;			// End instructions predicates
StringTVec calls;				// Calls predicates
StringTVec instr;				// Instructions predicates
StringTVec conds;				// Condition predicates
StringQVec hl_conds;			// High-Level state condition predicates
StringTVec gotos;				// Gotos predicates
StringTVec choiceInstr;			// Choice instructions

StringVec constant_slots;		// slots as constants
StringVec constant_vars;		// variables as constants
StringVec constant_programs;	// programs as constants
StringVec constant_states;		// states as constants
StringVec empty_slots;			// empty slot predicates

StringDVec fsc_true_calls_preds;  	// true programming calls predicates
StringDVec fsc_false_calls_preds; 	// false programming calls predicates
StringVec fsc_accumulator_preds;  	// accumulator predicates
StringVec fsc_evaluating_preds;	  	// evaluating predicates
StringVec fsc_action_preds;		  	// action predicates
StringVec fsc_noconds_preds;		// states without conditions
StringVec fsc_tgotos;				// gotos when true
StringVec fsc_fgotos;				// gotos when false
StringDVec fsc_conds_preds;			// condition predicates
StringDVec fsc_tacts;				// actions when true
StringDVec fsc_facts;				// actions when false
StringVec true_no_act_preds;		// true no-action predicates
StringVec false_no_act_preds; 		// false no-action predicates

/****** COMPILER CLASSES ***********/
Types *types;
Predicates *predicates;
Actions *actions;
SequentialActions *sequential_actions;
GotoActions *goto_actions;
TerminalActions *terminal_actions;
CallActions *call_actions;
ChoiceActions *choice_actions;
DerivedActions *derived_actions;
HLCondActions *hl_cond_actions;
UActions	*u_actions;
CFGActions *cfg_actions;

/****** COMPILER FUNCTIONS ***********/
void init(int argc, char *argv[]){
	// check input correctness
	if ( argc < 14 ) {
		std::cout << "Usage: ./bin/compile <compiler_type> <domain.pddl> <num of tasks> <task\\s.pddl> <max_bound> <current_bound> <procedures> <num of predicates to the stack> <predicate\\s> <num of objects to the call> <object\\s> <stack size> <to program?> <variable\\s> <slot\\s>\n";
		std::cout << "Writes domain file to standard output and instance file to standard error\n";
		exit( 1 );
	}

	int offset = 1;

	// read the compiler type
	compiler_type = String( argv[ offset++ ] );

	// read input domain
	d = new Domain( argv[ offset++ ] ); 
		StringTVec choiceInstr;

	// extract the total number of instances
	ISStream is( argv[ offset++ ] );
	is >> total_instances;
	
	// read input instances
	for(unsigned i = 0 ; i < total_instances; i++){
		ins.push_back( new Instance( *d, argv[ offset++ ] ) );
	}

	// extract max number of lines per procedure and main (bound)
	is.clear();
	is.str( argv[ offset++ ] );
	is >> max_bound;
	is.clear();
	is.str( argv[ offset++ ] );
	is >> bound;

	// extract number of procedures
	is.clear();
	is.str( argv[ offset++ ] );	
	is >> procedures;	

	// extract number of predicates in stack
	is.clear();
	is.str( argv[ offset++ ] );
	is >> total_stack_pred;

	INSTR_WITH_PARAMS = ( total_stack_pred > 0 );

	// read input predicates
	for( unsigned i = 0; i < total_stack_pred; i++ ){
		stack_predicates.insert( String( argv[ offset++ ] ) );
	}

	// extract variables for recursive calls
	is.clear();
	is.str( argv[ offset++ ] );
	is >> total_stack_vars;

	// read input variables for recursive calls
	for( unsigned i = 0; i < total_stack_vars; i++ ){
		stack_variables.insert( String( argv[ offset++ ] ) );

	}
	
	// assigning pile size by default
	is.clear();
	is.str( argv[ offset++ ] );
	is >> stack_size;

	// know if we have to program or not
	is.clear();
	is.str( argv[ offset++ ] );
	is >> to_program;

	// Number of variables for searching high-level state features
	is.clear();
	is.str( argv[ offset++ ] );
	is >> novariables;

	// Number of slots for searching high-level state features
	is.clear();
	is.str( argv[ offset++ ] );
	is >> noslots;
	
	// Number of classes
	is.clear();
	is.str( argv[ offset++ ] );
	is >> noclasses;

	IS_HIGH_LEVEL = ( novariables > 0 );
	IS_UNSUPERVISED = ( noclasses > 0 );

	// PREPROCESSING GOTOS	
	if( IS_HIGH_LEVEL ){
		// Any predicate is a potential condition!!!
		for ( unsigned i = 0; i < d->preds.size(); ++i ) {
			if ( d->preds[i]->name.substr( 0, 6 ) == "ASSIGN" ||
				 d->preds[i]->name.substr( 0, 8 ) == "INCLUDES" )
			effs.insert( i );
		}	
	}
	else{
		// Detect non-static fluents
		for ( unsigned i = 0; i < d->actions.size(); ++i ) {
			IntSet s = preds( d, d->actions[i]->eff );
			effs.insert( s.begin(), s.end() );
		}
		// Avoid delete derived predicates in the effects
		for ( unsigned i = 0; i < d->derived.size(); ++i ){
			unclean_effs.insert( d->preds.index( d->derived[i]->name ) );
			effs.insert( d->preds.index( d->derived[i]->name ) );
		}
	}
}

void create_compiled_domain( String &name, bool condeffects = true, bool typed = true, bool cons = true, bool costs = true){
	cd = DomainCreator::create( name, condeffects, typed, cons, costs );	
}

void create_instance( ){
	if( compiler_type == "PLPR" ){
		cins = InstanceCreator::create( d, cd, ins[ 0 ], rows, empty_lines, procedures, to_program, true, constant_slots, noclasses );
	}
	else if( compiler_type == "HFSC" ){
		cins = InstanceCreator::createFSCInstance( d, cd, ins[ 0 ], procedures, constant_states, rows, bound, to_program );
	}
	else if( compiler_type == "CFG" ){
		cins = InstanceCreator::createCFGInstance( d, cd, ins[ 0 ], rows, empty_lines, to_program, true, select_programs, bound );
	}
}

void create_classes( ){
	types = new Types();
	predicates = new Predicates();
	actions = new Actions( d, cd );
	sequential_actions = new SequentialActions( d, cd );
	goto_actions = new GotoActions( d, cd );
	terminal_actions = new TerminalActions( d, cd );
	call_actions = new CallActions( d, cd );
	choice_actions = new ChoiceActions( d, cd );
	derived_actions = new DerivedActions( d, cd );
	hl_cond_actions = new HLCondActions( d, cd );
	u_actions = new UActions( d, cd );
	cfg_actions = new CFGActions( d, cd );
}

void add_types(){ 
	types->copyTypes( d, cd );
	types->createType( cd, "STACKROW" );
	types->moveObjectsToConstants( cd );
	types->clearObjects( cd );
	types->addValuedConstants( cd, "STACKROW", "ROW", rows, stack_size );
	if( compiler_type == "PLPR" ){
		if( IS_HIGH_LEVEL ){
			types->createType( cd, "CONDITION-SLOT" );
			types->createType( cd, "CONDITION-VARIABLE" );
			types->addValuedConstants( cd, "CONDITION-SLOT", "SLOT", constant_slots, noslots + 1 );
			//types->addValuedConstants( cd, "CONDITION-VARIABLE", "CVAR", constant_vars, novariables + 1, 1 );
			types->addValuedConstants( cd, "CONDITION-VARIABLE", "CVAR", constant_vars, novariables + 1 );
		}
		if( IS_UNSUPERVISED ){
			types->createType( cd, "PROGRAM" );
			types->addValuedConstants( cd, "PROGRAM", "PROG", constant_programs, noclasses);	
		}
	}
	else if( compiler_type == "HFSC" ){
		types->createType( cd, "STACKSTATE" );
		types->addValuedConstants( cd, "STACKSTATE", "STATE", constant_states, max_bound + 1 );
	}
	else if( compiler_type == "CFG" ){
		//types->createType( cd, "PROGRAM" );
		//types->addValuedConstants( cd, "PROGRAM", "PROG", constant_programs, bound+1 , 0 );
	}
}

void add_predicates(){
	predicates->copyPredicates( d, cd );

	if( compiler_type == "PLPR" ){
		predicates->createPredicates( d, cd, procedures, max_bound, total_instances, tests, empty_lines, endinstr, calls, instr, choiceInstr );
		predicates->createTValuedPredicates( cd, "GOTO", gotos, procedures + 1, max_bound, max_bound + 1 );
		//[PERFORMANCE] Not-gotos
		/*StringTVec notgotos;				// notGotos predicates
		predicates->createTValuedPredicates( cd, "NOT-GOTO", notgotos, procedures + 1, max_bound, max_bound + 1 );*/
		//[END-OF-PERFORMANCE]
		if( IS_HIGH_LEVEL ){
			hl_conds.resize( d->preds.size() );
			for( int eff : effs ){
				StringVec parameters = d->typeList( d->preds[ eff ] );
				for ( unsigned j = 1; j < parameters.size(); ++j )
					parameters[ j ] = "CONDITION-VARIABLE";
				String name = "COND-" + d->preds[ eff ]->name;
				predicates->createTValuedPredicates( cd, name, hl_conds[ eff ], procedures + 1, bound, noslots + 1, 0, 0, 1, parameters);
			}

			// Add predicates related to slots
			predicates->createPredicate( cd, "CURRENT-SLOT", StringVec( 1, "CONDITION-SLOT" ) );
			predicates->createValuedPredicates( cd, "EMPTYSLOT", empty_slots, bound, 0, StringVec( 1, "CONDITION-SLOT" ) );

			StringVec parameters( 1, "CONDITION-SLOT" );
			parameters.insert( parameters.end(), novariables, "VALUE" );
			predicates->createPredicate( cd, "POSSIBLE-VALUE", parameters );
		}
		else{
			predicates->createTValuedPredicates( d, cd, conds, procedures + 1, max_bound, 0, 0 );
		}

		if( IS_UNSUPERVISED ){
			predicates->createPredicate( cd, "INS-SELECT-PROGRAM" );
			predicates->createPredicate( cd, "CHOSEN-PROGRAM", StringVec( 2, "PROGRAM" ) );	
		}
	}
	else if( compiler_type == "HFSC" ){
		// Add empty predicates	
		predicates->createPredicate( cd, stack_empty_cond_pred, StringVec( 1, "STACKSTATE" ) );
		predicates->createPredicate( cd, stack_empty_tgoto_pred, StringVec( 1, "STACKSTATE" ) );
		predicates->createPredicate( cd, stack_empty_fgoto_pred, StringVec( 1, "STACKSTATE" ) );
		predicates->createPredicate( cd, stack_empty_tact_pred, StringVec( 1, "STACKSTATE" ) );
		predicates->createPredicate( cd, stack_empty_fact_pred, StringVec( 1, "STACKSTATE" ) );

		// Add true and false predicates for no action instructions
		//predicates->createPredicate( cd, true_no_act_pred, StringVec( 1, "STACKSTATE" ) );
		//predicates->createPredicate( cd, false_no_act_pred, StringVec( 1, "STACKSTATE" ) );
		predicates->createValuedPredicates( cd, true_no_act_pred, true_no_act_preds, procedures + 1, 0, StringVec( 1, "STACKSTATE" ) );
		predicates->createValuedPredicates( cd, false_no_act_pred, false_no_act_preds, procedures + 1, 0, StringVec( 1, "STACKSTATE" ) );

		// Add stack state predicates
		predicates->createPredicate( cd, stack_state_pred, StringVec() = { "STACKSTATE", "STACKROW" } );

		// Link stack rows
		predicates->createPredicate( cd, stack_row_pred, StringVec( 2, "STACKROW" ) );

		// Add top stack predicate
		predicates->createPredicate( cd, stack_top_pred, StringVec( 1, "STACKROW" ) );

		// Add predicates for each pair procedure-row in the stack
		predicates->createValuedPredicates( cd, "STACK-PROCEDURE", stack_procedures, procedures + 1, 0, StringVec( 1, "STACKROW" ) );
	
		// Add predicates for each test
		predicates->createValuedPredicates( cd, "TEST", tests, total_instances );

		// Add calls predicates for procedures
		StringVec call_params( stack_variables.begin(), stack_variables.end());
		call_params.push_back( "STACKSTATE" );
		fsc_true_calls_preds.resize( procedures + 1 );
		fsc_false_calls_preds.resize( procedures + 1 );
		for(unsigned p_from = 0; p_from < procedures + 1; p_from++){
			fsc_true_calls_preds[ p_from ].resize( procedures + 1 );
			fsc_false_calls_preds[ p_from ].resize( procedures + 1 );
			for( unsigned p_to = 0; p_to < procedures + 1; p_to++ ){
				SStream ss;
				ss << "TRUE-CALL-" << p_from << "-" << p_to ;
				fsc_true_calls_preds[ p_from ][ p_to ] = ss.str();
				ss.str( "" );
				ss << "FALSE-CALL-" << p_from << "-" << p_to ;
				fsc_false_calls_preds[p_from ][ p_to ] = ss.str();
				if( p_to >= p_from ){
					predicates->createPredicate( cd, fsc_true_calls_preds[ p_from ][ p_to ], call_params );
					predicates->createPredicate( cd, fsc_false_calls_preds[ p_from ][ p_to ], call_params );
				}
			}
		}

		// Add evaluation predicates
		predicates->createValuedPredicates( cd, "ACCUMULATOR",    fsc_accumulator_preds, procedures + 1, 0, StringVec( 1, "STACKROW" ) );
		predicates->createValuedPredicates( cd, "DONE-EVALUATING", fsc_evaluating_preds, procedures + 1, 0, StringVec( 1, "STACKROW" ) );
		predicates->createValuedPredicates( cd, "DONE-APPLYING",       fsc_action_preds, procedures + 1, 0, StringVec( 1, "STACKROW" ) );

		// Add goal predicate
		predicates->createPredicate( cd, goal_pred );

		// Add available state to program
		predicates->createPredicate( cd, available_pred, StringVec( 1, "STACKSTATE" ) );

		// Add next state relations
		predicates->createPredicate( cd, next_state_pred, StringVec( 2, "STACKSTATE" ) );
	
		// Add predicates for conditions and jumps
		predicates->createValuedPredicates( cd, "NCOND", fsc_noconds_preds, procedures + 1, 0, StringVec( 1, "STACKSTATE" ) );
		predicates->createValuedPredicates( cd, "TRUE-GOTO", fsc_tgotos, procedures + 1, 0, StringVec( 2, "STACKSTATE" ) );
		predicates->createValuedPredicates( cd, "FALSE-GOTO", fsc_fgotos, procedures + 1, 0, StringVec( 2, "STACKSTATE" ) );
		
		fsc_conds_preds.resize(  d->preds.size() );

		for ( IntSet::iterator i = effs.begin(); i != effs.end(); ++i ) {
			fsc_conds_preds[ *i ].resize( procedures + 1 );
			for( unsigned p = 0; p < procedures + 1; p++ ){
				SStream ss;
				ss << "COND-" << d->preds[ *i ]->name << "-" << p;
				fsc_conds_preds[ *i ][ p ] = ss.str();
				StringVec params_aux = d->typeList( d->preds[ *i ] );
				params_aux.push_back( "STACKSTATE" );
				predicates->createPredicate( cd, fsc_conds_preds[ *i ][ p ], params_aux );
			}
		}	

		fsc_tacts.resize(  d->actions.size() );
		fsc_facts.resize(  d->actions.size() );

		for ( unsigned i = 0; i < d->actions.size(); ++i ) {
			fsc_tacts[ i ].resize( procedures + 1 );
			fsc_facts[ i ].resize( procedures + 1 );
			for( unsigned p = 0; p < procedures + 1; p++ ) {
				SStream ss;
				ss << "TRUE-" << d->actions[ i ]->name << "-" << p;
				fsc_tacts[ i ][ p ] = ss.str();
				StringVec params_aux = d->typeList( d->actions[ i ] );
				params_aux.push_back( "STACKSTATE" );
				predicates->createPredicate( cd, fsc_tacts[ i ][ p ], params_aux );

				ss.str( "" );
				ss << "FALSE-" << d->actions[ i ]->name << "-" << p;
				fsc_facts[ i ][ p ] = ss.str();
				predicates->createPredicate( cd, fsc_facts[ i ][ p ], params_aux );
			}
		}

		predicates->createDValuedPredicates( cd, "END-COND", endinstr, procedures + 1, max_bound + 1 );

	}
	else if( compiler_type == "CFG" ){
		predicates->createPredicates( d, cd, procedures, max_bound, total_instances, tests, empty_lines, endinstr, calls, instr, choiceInstr );
		//predicates->createPredicate( cd, "INS-SELECT-PROGRAM" );
		predicates->createValuedPredicates( cd, "INS-SELECT-PROGRAM", select_programs, procedures + 1 );
		//predicates->createValuedPredicates( cd, "ALLOWED-LINE", allowed_lines, bound + 1  );
		predicates->createDValuedPredicates( cd, "ALLOWED-LINE", allowed_lines, procedures + 1, max_bound + 1 );
	}
}

void add_cost_function(bool total_cost = true){
	// Add total-cost
	if( total_cost ){
		cd->createFunction( "TOTAL-COST", -1 );
	}
}

void add_derived_evaluation(){
	derived_actions->createEvalActions( "DERIVED-EVALUATION" );
}

void add_program_actions(){
	if( compiler_type == "PLPR" ){
		sequential_actions->createProgrammingActions( procedures, bound, empty_lines, instr );
	}
	else if( compiler_type == "HFSC" ){
		sequential_actions->createFSCProgrammingActions( procedures, fsc_accumulator_preds, fsc_evaluating_preds, fsc_tacts, fsc_facts, true_no_act_preds, false_no_act_preds  );
	}
	else if( compiler_type == "CFG" ){
		sequential_actions->createProgrammingActions( procedures, bound, empty_lines, instr );
	}
}

void add_repeat_actions(){
	if( compiler_type == "PLPR" ){
		sequential_actions->createRepeatActions( procedures, max_bound, instr );
	}
	else if( compiler_type == "HFSC" ){
		sequential_actions->createRepeatFSCActions( procedures, fsc_accumulator_preds, fsc_evaluating_preds, fsc_action_preds, fsc_tacts, fsc_facts, true_no_act_preds, false_no_act_preds );
	}
	else if( compiler_type == "CFG" ){
		sequential_actions->createRepeatActions( procedures, max_bound, instr );
	}
}

void add_program_goto_actions(){ // goto actions
	if( compiler_type == "PLPR" ){
		goto_actions->createProgrammingActions( procedures, bound, empty_lines, conds, gotos, empty_slots, noclasses );
	}
	else if( compiler_type == "HFSC" ){
		goto_actions->createFSCProgrammingActions( procedures, fsc_evaluating_preds, fsc_accumulator_preds, fsc_action_preds, fsc_noconds_preds, fsc_conds_preds, fsc_tgotos, fsc_fgotos );
	}
}

void add_do_goto_actions(){
	if( compiler_type == "PLPR" ){
		goto_actions->createRepeatActions( procedures, max_bound, gotos, conds, noclasses );
	}
	else if( compiler_type == "HFSC" ){
		goto_actions->createFSCTrueGotoActions( procedures, fsc_evaluating_preds, fsc_accumulator_preds, fsc_action_preds, fsc_tgotos);
		goto_actions->createFSCFalseGotoActions( procedures, fsc_evaluating_preds, fsc_accumulator_preds, fsc_action_preds, fsc_fgotos);
		goto_actions->createFSCEvalConditions( procedures, fsc_evaluating_preds, fsc_accumulator_preds, fsc_action_preds, fsc_noconds_preds, fsc_conds_preds );
	}
}

void add_program_end(){
	if( compiler_type == "HFSC" ){
		terminal_actions->createFSCProgrammingActions( procedures, bound, endinstr, constant_states );
	}
	else{
		terminal_actions->createProgrammingActions( procedures, bound, empty_lines, endinstr, allowed_lines );	
	}
}

void add_repeat_end_actions(){
	if( compiler_type == "PLPR" ){
		terminal_actions->createRepeatActions(	ins, procedures, total_instances, max_bound, endinstr, tests );
	}
	else if( compiler_type == "HFSC" ){
		terminal_actions->createFSCRepeatActions( ins, procedures, total_instances, bound, max_bound, constant_states, tests, endinstr );
	}
	else if( compiler_type == "CFG" ){
		terminal_actions->createRepeatActions(	ins, procedures, total_instances, max_bound, endinstr, tests );
	}
}

void add_program_call_actions(){
	if( compiler_type == "PLPR" ){
		call_actions->createProgrammingActions( procedures, bound, empty_lines, calls );
	}
	else if( compiler_type == "HFSC" ){
		call_actions->createFSCProgrammingActions( procedures, fsc_evaluating_preds, fsc_accumulator_preds, fsc_true_calls_preds, fsc_false_calls_preds );
	}
	else if( compiler_type == "CFG" ){
		call_actions->createProgrammingActions( procedures, bound, empty_lines, calls );
	}
}
	
void add_repeat_call_actions(){
	if( compiler_type == "PLPR" ){
		call_actions->createRepeatActions( procedures, max_bound, calls );
	}
	else if( compiler_type == "HFSC" ){
		call_actions->createFSCRepeatActions( procedures, constant_states, fsc_accumulator_preds, fsc_evaluating_preds, fsc_action_preds, fsc_true_calls_preds, fsc_false_calls_preds );	
	}
	else if( compiler_type == "CFG" ){
		call_actions->createRepeatActions( procedures, max_bound, calls );
	}
}

void add_program_choice_actions(){
	choice_actions->createProgrammingActions( choiceInstr, procedures, bound, empty_lines );
}
	
void add_repeat_choice_actions(){
	choice_actions->createRepeatActions( choiceInstr, procedures, max_bound );
}

void add_program_cond_actions(){ // Cond actions
	hl_cond_actions->createProgrammingActions( procedures, bound, noslots, empty_slots, hl_conds, constant_slots  );
}

void add_step_conditions(){
	hl_cond_actions->createDoStepsConditions( procedures, bound, noslots, constant_slots, constant_vars, novariables, hl_conds );
}

void add_eval_high_level_conditions(){
	hl_cond_actions->evalHighLevelActions( procedures, bound, constant_slots, noslots, novariables );
}


void print_domain(){
	cd->PDDLPrint( std::cout );
}

void print_instance(){
	cins->PDDLPrint( std::cerr );
}

void print_act2ins(){
	// Print actions to instructions file
	OFStream ofs( "act2ins.txt" );
	for( MapStrSVec::iterator it = act2ins.begin(); it != act2ins.end(); it++){
		ofs << it->first; // Action
		for( String instruction : (it->second) ){
			ofs << " " << instruction;
		}
		ofs << std::endl;
	}
	ofs.close();
}

void clean_variables(){
	delete cins;
	delete cd;
	for(unsigned i = 0; i < total_instances; i++)
		delete ins[i];
	delete d;
	if( types ) 
		delete types;
	if( predicates )
		delete predicates;
	if( sequential_actions )
		delete sequential_actions;
	if( terminal_actions )
		delete terminal_actions;
	if( call_actions )
		delete call_actions;
	if( choice_actions )
		delete choice_actions;
	if( derived_actions )
		delete derived_actions;
	if( hl_cond_actions )
		delete hl_cond_actions;
	if( u_actions )
		delete u_actions;
	if( cfg_actions );
		delete cfg_actions;
	if( actions )
		delete actions;
	
}

void planning_programs_compilation( ){
	if( EVAL_DERIVED && !IS_HIGH_LEVEL )
		add_derived_evaluation();

	// PROGRAMMING ACTIONS
	if( to_program ){
		add_program_actions(); 
		add_program_goto_actions(); 
		if( !IS_UNSUPERVISED ){
			add_program_end();
			add_program_call_actions(); 
			add_program_choice_actions();
		} 
		//std::cout << "BEF IS HIGH LVL" << std::endl;
		if( IS_HIGH_LEVEL ){ 
			//std::cout << "BEF PROGRAM COND" << std::endl;
			add_program_cond_actions(); 
			//std::cout << "AFT PROGRAM COND" << std::endl;
		}
	}

	// REPEAT ACTIONS
	add_repeat_actions(); 
	add_do_goto_actions();
	add_repeat_end_actions();
	if( !IS_UNSUPERVISED ){
		add_repeat_call_actions(); 
		add_repeat_choice_actions(); 
	}

	if( IS_HIGH_LEVEL ){
		add_step_conditions(); 
		add_eval_high_level_conditions();
	}
	if( IS_UNSUPERVISED ){
		u_actions->createRepeatActions( bound, noclasses, constant_programs );
	}
}

void hierachical_fsc_compilation(){
	if( to_program ){
		add_program_actions();
		add_program_end();
		add_program_goto_actions(); 
		add_program_call_actions(); 
	}

	add_repeat_actions();
	add_do_goto_actions();
	add_repeat_end_actions();
	add_repeat_call_actions();
}

void cfg_compilation(){
	if( to_program ){ 
		add_program_actions();
		add_program_end();
		add_program_call_actions();
	}
	
	add_repeat_actions();
	add_repeat_end_actions();
	add_repeat_call_actions();
	cfg_actions->createRepeatActions( procedures + 1, max_bound, allowed_lines, select_programs );
}

/****** COMPILER MAIN ***********/
int main( int argc, char *argv[] ) {

	// INITIALIZE; COMPILED DOMAIN & CLASSES
	init(argc, argv);
	create_compiled_domain( d->name );
	create_classes(); 
	add_types(); 
	add_predicates();
	create_instance(); 
	add_cost_function();

	if( compiler_type == "PLPR" )
		planning_programs_compilation();
	else if( compiler_type == "HFSC" )
		hierachical_fsc_compilation();
	else if( compiler_type == "CFG" )
		cfg_compilation();
	
	// PRINT DOMAIN, INSTANCE & ACTIONS TO INSTRUCTIONS
	print_domain(); 
	print_instance(); 
	print_act2ins();

	// CLEAN THE COMPILATOR
	clean_variables();
	
}
