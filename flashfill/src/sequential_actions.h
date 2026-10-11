#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "actions.h"

class SequentialActions : public Actions {
	public:
	SequentialActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}

	void createProgrammingActions( unsigned procedures, unsigned lines, StringVec& empty_lines, StringTVec& instruction ){
		for( unsigned action_id = 0; action_id < od->actions.size(); action_id++ ){
			// Get original action data
			Action *oact = od->actions[ action_id ];
			String oname = oact->name;
			unsigned osize = oact->params.size();
			StringVec parameters = od->typeList( oact );
			parameters.push_back( "STACKROW" );

			// Prepare compiled sequential programming actions
			String prefix_name = "PROGRAM-" + oname; 
			createValuedActions( prefix_name, lines, 0, parameters ); 
			StringVec action_names = getNames( prefix_name );

			for( unsigned line = 0; line < action_names.size(); line++ ){
				String name = action_names[ line ];

				copyPreconditions( oname, name ); 
				addParameters( name, osize, 1 ); 
					
				addPrecondition( name, empty_lines[ line ] ); 
				addPrecondition( name, stack_lines[ line ], false, IntVec( 1, osize ) ); 
				addPrecondition( name, "TOP-STACK", 		 false, IntVec( 1, osize ) ); 
				addPrecondition( name, "STACK-MAIN", 		 false, IntVec( 1, osize ) ); 
			
				addEffect( name, empty_lines[ line ], true );
				addEffect( name, instruction[ action_id ][ procedures ][ line ], false, incvec( 0, (IS_HIGH_LEVEL | INSTR_WITH_PARAMS?osize:0) ) );
			
				addCost( name, 1001 );
			
				addActionToInstruction( name, instruction[ action_id ][ procedures ][ line ] );	
			}
		}
	}
	
	void createRepeatActions( unsigned procedures, unsigned lines, StringTVec& instruction ){
		for( unsigned action_id = 0; action_id < od->actions.size(); action_id++ ){
			// Get original action data
			Action *oact = od->actions[ action_id ];
			String oname = oact->name;
			unsigned osize = oact->params.size();
			StringVec parameters = od->typeList( oact );
			parameters.push_back( "STACKROW" );
		
			// Prepare new action
			String name_prefix = "REPEAT-" + oname;
			createDValuedActions( name_prefix, procedures + 1, lines, 0, 0, parameters );
			StringVec action_prefixes = getNames( name_prefix );

			for( unsigned procedure = 0; procedure < action_prefixes.size(); procedure++ ){
				StringVec action_names = getNames( action_prefixes[ procedure ] );
				for( unsigned line = 0; line < action_names.size(); line++ ){
					String name = action_names[ line ];

					Action *act = getAction( cd, name );
					copyPreconditions( oname, name ); 
					addParameters( name, osize, 1 ); 

					addPrecondition( name, "TOP-STACK", false, IntVec( 1 , osize ) );
					addPrecondition( name, stack_procedures[ procedure ], false, IntVec( 1 , osize ) );
					addPrecondition( name, stack_lines[ line ], false, IntVec( 1 , osize ) );
					addPrecondition( name, instruction[ action_id ][ procedure ][ line ], false, incvec( 0 , ( IS_HIGH_LEVEL | INSTR_WITH_PARAMS ? osize : 0 ) ) ); 
			
					copyEffects( oname, name );
					addParameters( name, osize, 1, false );
					addEffect( name, stack_lines[ line ], true, IntVec( 1, osize ) );
					addEffect( name, stack_lines[ line + 1 ], false, IntVec( 1, osize ) );

					addStackRow( act->pre, osize );
					addStackRow( act->eff, osize );

					addCost( name, 1 );
				}
			}
		}
	}

	void createFSCProgrammingActions( unsigned procedures, const StringVec &accumulator_preds, const StringVec &evaluating_preds, const StringDVec &tacts, const StringDVec &facts, const StringVec& true_no_act_preds, const StringVec & false_no_act_preds ){
		// Program NO-ACT in True transition
		SStream ss;
		ss << "PROGRAM-TRUE-NO-ACT-" << procedures;
		String name = ss.str();

		createAction( name, StringVec() = { "STACKSTATE", "STACKROW" } );

		addPrecondition( name, stack_top_pred, false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_top_pred, false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_state_pred, false, incvec( 0, 2 ) );
		addPrecondition( name, stack_empty_tact_pred, false, IntVec( 1, 0 ) );
		addPrecondition( name, evaluating_preds[ procedures ], false, IntVec( 1, 1 ) );
		addPrecondition( name, accumulator_preds[ procedures ], false, IntVec( 1, 1 ) );

		addEffect( name, stack_empty_tact_pred, true, IntVec( 1, 0 ) );
		addEffect( name, true_no_act_preds[ procedures ], false, incvec( 0, 1 ) );

		// Add instruction
		addActionToInstruction( name, true_no_act_preds[ procedures ] );

		// Program NO-ACT in False transition
		ss.str("");
		ss << "PROGRAM-FALSE-NO-ACT-" << procedures;
		name = ss.str();

		createAction( name, StringVec() = { "STACKSTATE", "STACKROW" }  );
		
		addPrecondition( name, stack_top_pred, false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_state_pred, false, incvec( 0, 2 ) );
		addPrecondition( name, stack_empty_fact_pred, false, IntVec( 1, 0 ) );
		addPrecondition( name, evaluating_preds[ procedures ], false, IntVec( 1, 1 ) );
		addPrecondition( name, accumulator_preds[ procedures ], true, IntVec( 1, 1 ) );

		addEffect( name, stack_empty_fact_pred, true, IntVec( 1, 0 ) );
		addEffect( name, false_no_act_preds[ procedures ], false, incvec( 0, 1 ) );

		// Add instruction
		addActionToInstruction( name, false_no_act_preds[ procedures ]  );

		// Program actions
		for ( unsigned i = 0; i < od->actions.size(); ++i ){

			// Program true actions
			ss.str("");
			ss << "PROGRAM-TRUE-" << od->actions[ i ]->name << "-" << procedures;
			name = ss.str();
			
			StringVec params = od->typeList( od->actions[i] );
			params.push_back( "STACKSTATE" );
			params.push_back( "STACKROW" );
			unsigned par_size = params.size();
			
			createAction( name, params );
			
			copyPreconditions( od->actions[ i ]->name, name );

			addPrecondition( name, stack_top_pred, false, IntVec( 1 , par_size - 1 ) );
			addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, par_size - 1 ) );
			addPrecondition( name, stack_state_pred, false, incvec( par_size - 2 , par_size ) );
			addPrecondition( name, stack_empty_tact_pred, false, IntVec( 1 , par_size - 2 ) );
			addPrecondition( name, evaluating_preds[ procedures ], false, IntVec( 1 , par_size - 1 ) );
			addPrecondition( name, accumulator_preds[ procedures ], false, IntVec( 1 , par_size - 1 ) );

			addEffect( name, stack_empty_tact_pred, true, IntVec( 1 , par_size - 2 ) );
			addEffect( name, tacts[ i ][ procedures ], false, incvec( 0, par_size - 1) );

			// Add instruction
			addActionToInstruction( name, tacts[ i ][ procedures ]  );

			// Program false actions
			ss.str( "" );
			ss << "PROGRAM-FALSE-" << od->actions[ i ]->name << "-" << procedures;
			name = ss.str();
			
			createAction( name, params );
			
			copyPreconditions( od->actions[ i ]->name, name );
			
			addPrecondition( name, stack_top_pred, false, IntVec( 1 , par_size - 1 ) );
			addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, par_size - 1 ) );
			addPrecondition( name, stack_state_pred, false, incvec( par_size - 2 , par_size ) );
			addPrecondition( name, stack_empty_fact_pred, false, IntVec( 1 , par_size - 2 ) );
			addPrecondition( name, evaluating_preds[ procedures ], false, IntVec( 1 , par_size - 1 ) );
			addPrecondition( name, accumulator_preds[ procedures ], true, IntVec( 1 , par_size - 1 ) );

			addEffect( name, stack_empty_fact_pred, true, IntVec( 1 , par_size - 2 ) );
			addEffect( name, facts[ i ][ procedures ], false, incvec( 0 , par_size - 1 ) );
		
			// Add instruction
			addActionToInstruction( name, facts[ i ][ procedures ]  );
		}
	}

	void createRepeatFSCNoAct( const String &name, int proc, bool is_true, const StringVec &accumulator_preds, const StringVec &evaluating_preds, const StringVec &action_preds, const StringVec& true_no_act_preds, const StringVec & false_no_act_preds ){
		// Repeat true and false no actions
		createAction( name, StringVec() = { "STACKSTATE", "STACKROW" } );

		addPrecondition( name, stack_top_pred, false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_procedures[ proc ], false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_state_pred, false, incvec( 0, 2 ) );
		addPrecondition( name, evaluating_preds[ proc ], false, IntVec( 1, 1 ) );
		addPrecondition( name, action_preds[ proc ], true, IntVec( 1, 1 ) );
		if( is_true ){
			addPrecondition( name, accumulator_preds[ proc ], false, IntVec( 1, 1 ) );
			addPrecondition( name, true_no_act_preds[ proc ], false, incvec( 0, 1 ) );
		}
		else{
			addPrecondition( name, accumulator_preds[ proc ], true, IntVec( 1, 1 ) );
			addPrecondition( name, false_no_act_preds[ proc ], false, incvec( 0, 1 ) );
		}
		addEffect( name, action_preds[ proc ], false, IntVec( 1, 1 ) );	
	}

	void createRepeatFSCAction( const String &name, int act, int proc, bool is_true, const StringVec &accumulator_preds, const StringVec &evaluating_preds, const StringVec &action_preds, const StringDVec &tacts, const StringDVec &facts  ){
			StringVec params = od->typeList( od->actions[ act ] );
			params.push_back( "STACKSTATE" );
			params.push_back( "STACKROW" );
			unsigned par_size = params.size();
			
			Action *repeat_act = createAction( name, params );
		
			copyPreconditions( od->actions[ act ]->name, name );
			addParameters( name, par_size - 2, 2 );

			addPrecondition( name, stack_top_pred, false, IntVec( 1, par_size - 1 ) );
			addPrecondition( name, stack_procedures[ proc ], false, IntVec( 1 , par_size - 1 ) );
			addPrecondition( name, stack_state_pred, false, incvec( par_size - 2 , par_size ) );
			addPrecondition( name, evaluating_preds[ proc ], false, IntVec( 1 , par_size - 1 ) );

			addPrecondition( name, action_preds[ proc ], true, IntVec( 1 , par_size - 1 ) );
			if( is_true ){
				addPrecondition( name, accumulator_preds[ proc ], false, IntVec( 1 , par_size - 1 ) );
				addPrecondition( name, tacts[ act ][ proc ], false, incvec( 0, par_size - 1 ) );
			}
			else{
				addPrecondition( name, accumulator_preds[ proc ], true, IntVec( 1 , par_size - 1 ) );
				addPrecondition( name, facts[ act ][ proc ], false, incvec( 0, par_size - 1 ) );
			}

			addStackRow( repeat_act->pre, par_size - 1 );

			copyEffects( od->actions[ act ]->name, name );
			addParameters( name, par_size - 2, 2, false );

			addEffect( name, action_preds[ proc ], false, IntVec( 1, par_size - 1 ) ) ;

			addStackRow( repeat_act->eff, par_size - 1 );	
	}

	void createRepeatFSCActions( unsigned procedures, const StringVec &accumulator_preds, const StringVec &evaluating_preds, const StringVec &action_preds, const StringDVec &tacts, const StringDVec &facts, const StringVec& true_no_act_preds, const StringVec & false_no_act_preds ){ 
		// Repeat no actions
		for( unsigned p = 0; p < procedures + 1; p++ ){
			SStream ss;
			ss << "REPEAT-TRUE-NO-ACT-" << p;
			createRepeatFSCNoAct( ss.str() , p, true, accumulator_preds, evaluating_preds, action_preds, true_no_act_preds, false_no_act_preds );
			ss.str( "" );
			ss << "REPEAT-FALSE-NO-ACT-" << p;
			createRepeatFSCNoAct( ss.str() , p, false, accumulator_preds, evaluating_preds, action_preds, true_no_act_preds, false_no_act_preds  );
		}
		// Repeat actions
		for ( unsigned act = 0; act < od->actions.size(); ++act ){
			for( unsigned p = 0; p < procedures + 1; p++ ){
				SStream ss;
				ss << "REPEAT-TRUE-" << od->actions[ act ]->name << "-" << p;
				createRepeatFSCAction( ss.str(), act, p, true, accumulator_preds, evaluating_preds, action_preds, tacts, facts );
				ss.str("");
				ss << "REPEAT-FALSE-" << od->actions[ act ]->name << "-" << p;
				createRepeatFSCAction( ss.str(), act, p, false, accumulator_preds, evaluating_preds, action_preds, tacts, facts );
			}
		}
	}

	/*void createCFGProgrammingActions( unsigned procedures, unsigned lines, const StringVec& empty_lines, const StringVec& allowed_lines, const StringTVec& instruction ){
		for( unsigned action_id = 0; action_id < od->actions.size(); action_id++ ){
			// Get original action data
			Action *oact = od->actions[ action_id ];
			String oname = oact->name;
			unsigned osize = oact->params.size();
			StringVec parameters = od->typeList( oact );
			parameters.push_back( "STACKROW" );

			// Prepare compiled sequential programming actions
			String prefix_name = "PROGRAM-" + oname; 
			createValuedActions( prefix_name, lines, 0, parameters ); 
			StringVec action_names = getNames( prefix_name );

			for( unsigned line = 0; line < action_names.size(); line++ ){
				String name = action_names[ line ];

				copyPreconditions( oname, name ); 
				addParameters( name, osize, 1 ); 
					
				addPrecondition( name, empty_lines[ line ] ); 
				addPrecondition( name, allowed_lines[ line ] );
				addPrecondition( name, stack_lines[ line ], false, IntVec( 1, osize ) ); 
				addPrecondition( name, "TOP-STACK", false, IntVec( 1, osize ) ); 
				addPrecondition( name, "STACK-MAIN", false, IntVec( 1, osize ) ); 
			
				addEffect( name, empty_lines[ line ], true );
				addEffect( name, instruction[ action_id ][ procedures ][ line ], false, incvec( 0, (INSTR_WITH_PARAMS?osize:0) ) );
				addEffect( name, allowed_lines[ line + 1 ] );
			
				addCost( name, 1001 );
			
				addActionToInstruction( name, instruction[ action_id ][ procedures ][ line ] );	
			}
		}
	}*/

};

#endif
