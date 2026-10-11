#include "actions.h"

class CallActions : public Actions{
	public:
	CallActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}
	
	void createProgrammingActions(	unsigned procedures, unsigned lines, StringVec& empty_lines, StringTVec& calls ){		
		StringVec parameters( 1, "STACKROW" );
		for( String var : stack_variables ){
			parameters.emplace_back( var );
		}
		unsigned psize = parameters.size();
		
		for( unsigned to_procedure = 0; to_procedure < procedures + 1; to_procedure++ ){
			for( unsigned line = 0; line < lines; line++ ){
				SStream ss;
				ss << "PROGRAM-CALL-" << to_procedure << "-" << procedures << "-" << line;
				String name = ss.str();
				createAction( name, parameters );

				addPrecondition( name, empty_lines[ line ] );
				addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) );
				addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 0 ) );
				addPrecondition( name, stack_lines[ line ], false, IntVec( 1, 0 ) );
			
				addEffect( name, empty_lines[ line ], true );

				// Include this fluents to allow repetitions
				addEffect( name, calls[ to_procedure ][ procedures ][ line ], false, incvec( 1, psize ) );

				addCost( name, 1001 );
			
				addActionToInstruction( name, calls[ to_procedure ][ procedures ][ line ] );
			}
		}
	}

	void createRepeatActions(	unsigned procedures, unsigned lines, StringTVec& calls ){
		StringVec parameters( 2, "STACKROW" );
		for( String var : stack_variables ){
			parameters.emplace_back( var );
		}

		for( unsigned to_procedure = 0; to_procedure < procedures + 1; to_procedure++ ){
			SStream ss;
			ss << "REPEAT-CALL-" << to_procedure;

			createDValuedActions( ss.str(), procedures + 1, lines, to_procedure, 0, parameters );
		
			StringVec action_prefixes = getNames( ss.str() );
		
			for( unsigned idx1 = 0; idx1 < action_prefixes.size(); idx1++ ){				
				StringVec names = getNames( action_prefixes[ idx1 ] );
				unsigned from_procedure = idx1 + to_procedure;

				for( unsigned line = 0; line < lines; line++ ){
					String name = names[ line ];

					addPrecondition( name, calls[ to_procedure ][ from_procedure ][ line ], false, incvec( 2 , parameters.size() ) );
					addPrecondition( name, "NEXT-STACK-ROW", false, incvec( 0, 2 ) ) ;
					addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) );
					addPrecondition( name, stack_procedures[ from_procedure ], false, IntVec( 1, 0 ) );
					addPrecondition( name, stack_lines[ line ], false, IntVec( 1, 0 ) );
				
					addEffect( name, "TOP-STACK", true, IntVec( 1, 0 ) );
					addEffect( name, "TOP-STACK", false, IntVec( 1, 1 ) );
					addEffect( name, stack_procedures[ to_procedure ], false, IntVec( 1, 1 ) );
					addEffect( name, stack_lines[ 0 ], false, IntVec( 1, 1 ) );
					addEffect( name, stack_lines[ line ], true, IntVec( 1, 0 ) );
					addEffect( name, stack_lines[ line + 1 ], false, IntVec( 1, 0 ) );

					if( EVAL_DERIVED && !IS_HIGH_LEVEL ){
						addPrecondition( name, "DONE-DERIVED" );

						addEffect( name, "DONE-DERIVED", true );
						addEffect( name, "TODO-DERIVED" );
					}

					Action *repcall = getAction( cd, name );

					int predicate_idx = 0;
					for( String predicate : stack_predicates ){
						Lifted *l = cd->preds.get( predicate );
						if( l->params.size() <= 1 ){
							When *w = new When;
							( ( And * ) repcall->eff )->add( w );
							predicate_idx++;
							continue;
						}
					
						Forall *f = new Forall;

						//Add the correct params to Forall
						for( auto lparam : (l->params) ){
							bool ok = true;
							for( auto aparam : (repcall->params) ) {
								if( lparam == aparam ){
									ok = false;
									break;
								}
							}
							if( ok ) 
								(f->params).emplace_back( lparam );
						}

						When *w = new When;
						IntVec when_parameters_1;
						IntVec when_parameters_2;

						for( unsigned parameter = 2; parameter < parameters.size(); parameter++ ){
							Type *t = cd->getType( parameters[ parameter ] );
							int parameter2 = ( t->parseConstant( stack_constants[ predicate_idx ][ parameter - 2 ] ) ).second;
							when_parameters_1.emplace_back( parameter );
							when_parameters_2.emplace_back( parameter2 );
						}

						for( size_t par = 0; par < (f->params).size(); par++){
							when_parameters_1.emplace_back( par + (repcall->params).size());
							when_parameters_2.emplace_back( par + (repcall->params).size());
						}

						when_parameters_1.push_back( 0 );
						when_parameters_2.push_back( 1 );

						w->pars = cd->ground( l->name, when_parameters_1 );
						w->cond = cd->ground( l->name, when_parameters_2 );
						f->cond = w;
						( ( And * ) repcall->eff )->add( f );
					
						predicate_idx++;
					}

					addCost( name, 1 );
				}
			}
		}
	}

	void createFSCProgrammingActions( unsigned procedures, const StringVec &evaluating_preds, const StringVec &accumulator_preds, const StringDVec &true_calls, const StringDVec &false_calls ){
		for(unsigned p_to = 0; p_to < procedures + 1; p_to++ ){
			for( unsigned p_from = p_to; p_from < procedures + 1; p_from++){
				SStream ss;
				ss << "PROGRAM-TRUE-CALL-" << p_to << "-" << p_from; // to p from p2

				String name = ss.str();
	
				StringVec params( stack_variables.begin() , stack_variables.end() );
				params.push_back( "STACKSTATE" );
				params.push_back( "STACKROW" );
				unsigned par_size = params.size();

				createAction( name, params );
				addPrecondition( name, stack_top_pred, false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, stack_procedures[ p_from ], false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, stack_state_pred, false, incvec( par_size - 2, par_size ) );
				addPrecondition( name, stack_empty_tact_pred, false, IntVec( 1 , par_size - 2 ) );
				addPrecondition( name, evaluating_preds[ p_from ], false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, accumulator_preds[ p_from ], false, IntVec( 1, par_size - 1 ) );

				addEffect( name, stack_empty_tact_pred, true, IntVec( 1 , par_size - 2 ) );
				// include this fluent to allow repetitions
				addEffect( name, true_calls[ p_to ][ p_from ], false, incvec( 0 , par_size - 1 ) ); 

				// Add instruction
				addActionToInstruction( name, true_calls[ p_to ][ p_from ] );

				ss.str( "" );
				ss << "PROGRAM-FALSE-CALL-" << p_to << "-" << p_from; // to p from p2
				name = ss.str();
				createAction( name, params );
				addPrecondition( name, stack_top_pred, false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, stack_procedures[ p_from ], false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, stack_state_pred, false, incvec( par_size - 2, par_size ) );
				addPrecondition( name, stack_empty_fact_pred, false, IntVec( 1 , par_size - 2 ) );
				addPrecondition( name, evaluating_preds[ p_from ], false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, accumulator_preds[ p_from ] , true, IntVec( 1, par_size - 1 ) );

				addEffect( name, stack_empty_fact_pred, true, IntVec( 1 , par_size - 2 ) );
				// include this fluent to allow repetitions
				addEffect( name, false_calls[ p_to ][ p_from ], false, incvec( 0 , par_size - 1 ) ); 

				// Add instruction
				addActionToInstruction( name, false_calls[ p_to ][ p_from ] );
			}
		}
	}

	void createFSCRepeatAction( const String &name , unsigned p_to, unsigned p_from , bool is_true, const StringVec& constant_states, const StringVec &accumulator_pred, const StringVec &evaluating_preds, const StringVec &action_preds, const StringDVec &true_calls, const StringDVec &false_calls  ){
		int ci_st0 = cd->constantIndex( constant_states[ 0 ] , "STACKSTATE" );

		StringVec params( stack_variables.begin(), stack_variables.end() );
		unsigned stack_size = stack_variables.size();
		params.push_back( "STACKSTATE" );
		params.push_back( "STACKROW" );
		params.push_back( "STACKROW" );
		int par_size = (int)params.size();

		Action *repcall = createAction( name, params );
	
		if( is_true ){
			addPrecondition( name, accumulator_pred[ p_from ], false, IntVec( 1 , par_size - 2 ) );
			addPrecondition( name, true_calls[ p_to ][ p_from ], false, incvec( 0 , par_size - 2 ) );
		}
		else{
			addPrecondition( name, accumulator_pred[ p_from ], true, IntVec( 1 , par_size - 2 ) );
			addPrecondition( name, false_calls[ p_to ][ p_from ], false, incvec( 0, par_size - 2 ) );
		}
		addPrecondition( name, stack_row_pred, false, incvec( par_size - 2, par_size ) );
		addPrecondition( name, stack_top_pred, false, IntVec( 1 , par_size - 2 ) );
		addPrecondition( name, stack_procedures[ p_from ], false, IntVec( 1, par_size - 2 ) );
		addPrecondition( name, stack_state_pred, false, incvec( par_size - 3 , par_size - 1 ) );
		addPrecondition( name, evaluating_preds[ p_from ], false, IntVec( 1 , par_size - 2 ) );
		addPrecondition( name, action_preds[ p_from ], true, IntVec( 1 , par_size - 2 ) );

		addEffect( name, stack_top_pred, true, IntVec( 1, par_size - 2 ) );
		addEffect( name, stack_top_pred, false, IntVec( 1, par_size - 1 ) );
		addEffect( name, stack_procedures[ p_to ], false, IntVec( 1, par_size - 1 ) );
		addEffect( name, stack_state_pred, false, IntVec() = { ci_st0, par_size - 1 } );
		addEffect( name, action_preds[ p_from ], false, IntVec( 1 , par_size - 2 ) );


		//[BUG][ToDo] This few lines have big chances of being future bugs
		int predi = 0;
		for( StringSet::iterator it = stack_predicates.begin(); it != stack_predicates.end(); it++, predi++){
			Lifted *l = cd->preds.get( *it );
			if( l->params.size() > 1 ){
				Forall *f = new Forall;
				When *w = new When;
				IntVec wp1, wp2;
				wp1 = incvec( 0 , stack_size );					
				wp2 = incvec( 0 , stack_size );
				//Add the correct params to Forall
				for(size_t par = 0, fpars = par_size; par < (l->params).size(); par++){
					bool ok = true;
					for(size_t par2 = 0; par2 < (repcall->params).size(); par2++){
						if( (l->params)[par] == (repcall->params)[par2] ){
							ok = false; 
							break;
						}
					}
					if( ok ){
						(f->params).push_back( (l->params)[par] );
						wp1.push_back( fpars );
						wp2.push_back( fpars );
						fpars++;
					}
				}
				wp1.push_back( par_size - 2 );
				wp2.push_back( par_size - 1 );
				// [BUG] Automatize this assignment
				wp2[ 0 ] = cd->constantIndex( stack_constants[ 0 ][ 0 ] , "VARIABLE" );
				w->pars = cd->ground( l->name, wp1 );
				w->cond = cd->ground( l->name, wp2 );
				f->cond = w;
				( ( And * ) repcall->eff )->add( f );
			}
		}
	}

	void createFSCRepeatActions( unsigned procedures, const StringVec& constant_states, const StringVec &accumulator_pred, const StringVec &evaluating_preds, const StringVec &action_pred, const StringDVec &true_calls, const StringDVec &false_calls ){	

		// CALL P from P2
		for(unsigned p_to = 0; p_to < procedures + 1; p_to++ ){
			for( unsigned p_from = p_to; p_from < procedures + 1; p_from++){
				SStream ss;
				ss << "REPEAT-TRUE-CALL-" << p_to << "-" << p_from;
				String name = ss.str();			
				createFSCRepeatAction( name, p_to, p_from, true, constant_states, accumulator_pred, evaluating_preds, action_pred, true_calls, false_calls );

				ss.str( "" );
				ss << "REPEAT-FALSE-CALL-" << p_to << "-" << p_from;
				name = ss.str();
				createFSCRepeatAction( name, p_to, p_from, false, constant_states, accumulator_pred, evaluating_preds, action_pred, true_calls, false_calls );			
			}
		}
	}

};
