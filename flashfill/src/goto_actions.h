#ifndef GOTO_H
#define GOTO_H

#include "include.h"

class GotoActions : public Actions {
	public:
	GotoActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}
	
	bool unsupervisedCheck( unsigned line_from, unsigned line_to, unsigned lines, unsigned noclasses ){
		// Constraints for unsupervised gotos
		if( IS_UNSUPERVISED ){
			if( line_from == 0 || line_to == 0) return false;
			unsigned program_lines = lines / noclasses;
			if( line_from % program_lines == 0 ) return false;
			if( ( line_from - 1 ) / program_lines != ( line_to - 1 ) / program_lines ) return false;
		}
		return true;
	}

	void createProgrammingAction( const String &name, const StringVec &parameters, unsigned original_size, unsigned predicate_id, unsigned procedures, unsigned line_from, unsigned line_to, const StringVec &empty_lines, const StringTVec &gotos, const StringTVec &condition ){
		createAction( name, parameters );
		addPrecondition( name, "TOP-STACK", false, IntVec( 1, original_size ) );
		addPrecondition( name, "STACK-MAIN", false, IntVec( 1, original_size ) );
		addPrecondition( name, empty_lines[ line_from ] );
		addPrecondition( name, stack_lines[ line_from ], false, IntVec( 1, original_size ) );

		//[PERFORMANCE] Adding preconditions to avoid crossing conditionals
		/*unsigned lines = empty_lines.size() - 1;
		for( unsigned line_from_2 = 0; line_from_2 < lines; line_from_2++ ){
			for( unsigned line_to_2 = 0; line_to_2 < lines + 1; line_to_2++ ){
				if( line_from_2 == line_to_2 || ( line_from_2 + 1 ) == line_to_2 )
					continue;
				if( line_from == line_from_2 && line_to == line_to_2 ) continue;
				if( line_from < line_to ){
					if( line_from_2 < line_to_2 ){
						if( line_to_2 <= line_from ) continue;
						if( line_to <= line_from_2 ) continue;
						if( line_from < line_from_2 && line_to_2 <= line_to ) continue;
					}
					else{
						if( line_from_2 < line_from ) continue;
						if( line_to <= line_to_2 ) continue;
						if( line_from < line_to_2 && line_from_2 < line_to ) continue;
					}
				}
				else{
					if( line_from_2 < line_to_2 ){
						if( line_to_2 <= line_to ) continue;
						if( line_from < line_from_2 ) continue;
						if( line_to < line_from_2 && line_to_2 <= line_from ) continue;
					}
					else{
						if( line_from_2 < line_to ) continue;
						if( line_from < line_to_2 ) continue;
						if( line_to < line_to_2 && line_from_2 < line_from ) continue;
					}
				}
				addPrecondition( name, "NOT-"+gotos[ procedures ][ line_from_2 ][ line_to_2 ] );
			}
		}
		addEffect( name, "NOT-"+gotos[ procedures ][ line_from ][ line_to ], true );*/
		//[END-OF-PERFORMANCE]

		addEffect( name, empty_lines[ line_from ], true );
		addEffect( name, gotos[ procedures ][ line_from ][ line_to ] );
		addEffect( name, condition[ predicate_id ][ procedures ][ line_from ], false, incvec( 0, original_size ) );

		addCost( name, 1001 );

		addActionToInstruction( name, gotos[ procedures ][ line_from ][ line_to ] );
		addActionToInstruction( name, condition[ predicate_id ][ procedures ][ line_from ] );
	}

	void createHLProgrammingAction( const String& name , unsigned procedures, unsigned line_from, unsigned line_to, const StringVec &empty_lines, const StringTVec& gotos, const StringVec &empty_slots  ){
		Action *prog_goto = createAction( name, StringVec( 1, "STACKROW" ) );
		addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) );
		addPrecondition( name, "STACK-MAIN", false, IntVec( 1, 0 ) );
		addPrecondition( name, empty_lines[ line_from ] );
		addPrecondition( name, stack_lines[ line_from ], false, IntVec( 1, 0 ) );

		addEffect( name, empty_lines[ line_from ], true );
		addEffect( name, gotos[ procedures ][ line_from ][ line_to ] );

		Forall *f = new Forall;
		f->params = IntVec( 1, cd->types.index( "CONDITION-SLOT" ) );
		f->cond = cd->ground( empty_slots[ line_from ], IntVec( 1, 1 ) );
		( ( And * )prog_goto->eff )->add( f );

		addCost( name, 1001 );

		addActionToInstruction( name, gotos[ procedures ][ line_from ][ line_to ] );
	}

	void createProgrammingActions( unsigned procedures, unsigned lines, StringVec& empty_lines, StringTVec &condition, StringTVec& gotos, StringVec &empty_slots, unsigned noclasses = 0){
		String prefix_name = "PROGRAM-GOTO";

		for( unsigned line_from = 0; line_from < lines; line_from++ ){
			for( unsigned line_to = 0; line_to < lines + 1; line_to++ ){
				if( line_from == line_to || line_from + 1 == line_to ) continue;
				if( !unsupervisedCheck( line_from, line_to, lines, noclasses ) ) continue;
				if( IS_HIGH_LEVEL ){
					SStream ss;
					ss << prefix_name << "-" << line_from << "-" << line_to;
					String name = ss.str();
					createHLProgrammingAction( name, procedures, line_from, line_to, empty_lines, gotos, empty_slots );
				}
				else{
					for( int predicate_id : effs ){
						StringVec parameters = od->typeList( od->preds[ predicate_id ] );
						parameters.emplace_back( "STACKROW" );
						unsigned osize = od->preds[ predicate_id ]->params.size();

						SStream ss;
						ss << prefix_name << "-" << cd->preds[ predicate_id ]->name << "-" << line_from << "-" << line_to;
						String name = ss.str();
						createProgrammingAction( name, parameters, osize, predicate_id, procedures, line_from, line_to, empty_lines, gotos, condition);
					}
				}
			}
		}
	}

	void createDoGotoActions( bool donot, unsigned procedures, unsigned lines, StringTVec& gotos, unsigned noclasses = 0 ){
		String prefix_name = "DO-GOTO";
		if( donot ) prefix_name = "DONT-GOTO";

		for( unsigned procedure = 0; procedure < procedures + 1; procedure++ ){
			for( unsigned line_from = 0; line_from < lines; line_from++ ){
				for( unsigned line_to = 0; line_to < lines + 1; line_to++ ){
					if( line_from == line_to || line_from + 1 == line_to ) continue;
					if( !unsupervisedCheck( line_from, line_to, lines, noclasses ) ) continue;
					SStream ss;
					ss << prefix_name << "-" << procedure << "-" << line_from << "-" << line_to; // Procedure P (last one is the main)
					String name = ss.str();
					createAction( name, StringVec( 1, "STACKROW" ) );
					addPrecondition( name, gotos[ procedure ][ line_from ][ line_to ] );
					addPrecondition( name, eval_pred );

					if( donot ){
						addPrecondition( name, accu_pred );
					}
					else{
						addPrecondition( name, accu_pred, true );
					}
					addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) );
					addPrecondition( name, stack_procedures[ procedure ], false, IntVec( 1, 0 ) );
					addPrecondition( name, stack_lines[ line_from ], false, IntVec( 1, 0 ) );

					addEffect( name, eval_pred, true );
					addEffect( name, stack_lines[ line_from ], true, IntVec( 1, 0 ) );
					if( donot ){
						addEffect( name, accu_pred, true );
						addEffect( name, stack_lines[ line_from + 1 ], false, IntVec( 1, 0 ) );
					}
					else{
						addEffect( name, stack_lines[ line_to ], false, IntVec( 1, 0 ) );
					}

					addCost( name, 1 ) ;
				}
			}
		}
	}

	void createEvalActions( unsigned procedures, unsigned lines, StringTVec &conds, unsigned noclasses = 0 ){
		for( int effect_id : effs ){
			String prefix_name = od->preds[ effect_id ]->name;
			unsigned osize = od->preds[ effect_id ]->params.size();
			StringVec parameters = od->typeList( od->preds[ effect_id ] );
			parameters.emplace_back( "STACKROW" );

			for( unsigned procedure = 0; procedure < procedures + 1; procedure++ ){
				for( unsigned line = 0; line < lines; line++ ){
					if( !unsupervisedCheck( line, line, lines, noclasses ) ) continue;
					SStream ss;
					ss << "EVAL-COND-" << prefix_name << "-" << procedure << "-" << line;
					String name = ss.str();
				
					Action *eval = createAction( name, parameters );
					addPrecondition( name, conds[ effect_id ][ procedure ][ line ], false, incvec( 0, osize ) );
					addPrecondition( name, "TOP-STACK", false, IntVec( 1, osize ) );
					addPrecondition( name, stack_procedures[ procedure ], false, IntVec( 1, osize ) );
					addPrecondition( name, stack_lines[ line ], false, IntVec( 1, osize ) );

					addEffect( name, eval_pred );

					if( EVAL_DERIVED && !IS_HIGH_LEVEL ){
						addPrecondition( name, "DONE-DERIVED" );

						addEffect( name, "DONE-DERIVED", true );
						addEffect( name, "TODO-DERIVED" );
					}

					When *w = new When;
					w->cond = new And;
					( (And *) w->cond )->add( cd->ground( accu_pred ) );
				
					if( unclean_effs.find( effect_id ) == unclean_effs.end() ){
						// It is not a derived predicate
						w->pars = cd->ground( prefix_name, incvec( 0, osize ) );
						addStackRow( w , osize );
						( ( And * )eval->eff )->add( w );
					}
					else{
						// Derived predicates as effects of eval actions
						for ( unsigned derived_id = 0; derived_id < od->derived.size(); derived_id++ ){
							if( od->derived[ derived_id ]->name != prefix_name ) continue;
							unsigned dsize = od->derived[ derived_id ]->params.size();
							IntVec derived_params = incvec( 0 , dsize );

							( ( And * )eval->eff )->add( new Not( cd->ground( prefix_name, derived_params ) ) );

							( ( And *) w->cond)->add( cd->ground( prefix_name, derived_params ) );
						
							Exists * exists = dynamic_cast< Exists * >( od->derived[ derived_id ]->cond );

							if ( exists ) {
								w->pars = exists->cond->copy( *cd );
								w->addParams( dsize, 1 );

								Forall * f = new Forall;
								f->params = exists->params;
								f->cond = w;

								addStackRow( f, osize );

								( ( And * )eval->eff )->add( f );
							}
							else {
								w->pars = od->derived[ derived_id ]->cond->copy( *cd );
								w->addParams( dsize, 1 );

								addStackRow( w, osize );

								( ( And * )eval->eff )->add( w );
							}
							break;
						}
					}

					addCost( name, 1 );
				}
			}
		}
	}

	void createRepeatActions( unsigned procedures, unsigned lines, StringTVec& gotos, StringTVec& conds, unsigned noclasses = 0 ){
		createDoGotoActions( false, procedures, lines, gotos, noclasses );
		createDoGotoActions( true,  procedures, lines, gotos, noclasses );
		if( !IS_HIGH_LEVEL )
			createEvalActions( procedures, lines, conds, noclasses );
	}

	void createFSCProgrammingActions( unsigned procedures, const StringVec& evaluating_preds, const StringVec &accumulator_preds, const StringVec &action_preds, const StringVec &nconds, const StringDVec &conds, const StringVec &tgotos, const StringVec &fgotos ){ // Goto actions
		// Program NO-CONDITION
		SStream ss;
		ss << "PROGRAM-NOCOND-" << procedures;
		String name = ss.str();
		
		createAction( name , StringVec() = { "STACKSTATE", "STACKROW" } );

		addPrecondition( name, stack_top_pred, false, IntVec( 1 , 1 ) );
		addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 1 ) );
		addPrecondition( name, stack_state_pred, false, incvec( 0 , 2 ) );
		addPrecondition( name, stack_empty_cond_pred, false, IntVec( 1 , 0 ) );
	
		addEffect( name, stack_empty_cond_pred, true, IntVec( 1 , 0 ) );
		addEffect( name, nconds[ procedures ], false, IntVec( 1 , 0 ) );
	
		// Add instruction
		addActionToInstruction( name, nconds[ procedures ] );

		for ( IntSet::iterator eff = effs.begin(); eff != effs.end(); ++eff ){
			// Program condition
			ss.str("");
			ss << "PROGRAM-COND-" << od->preds[ *eff ]->name << "-" << procedures;
			name = ss.str();
			StringVec params = od->typeList( od->preds[ *eff ] );
			params.push_back( "STACKSTATE" );
			params.push_back( "STACKROW" );
			unsigned par_size = params.size();
			createAction( name, params );

			addPrecondition( name, stack_top_pred, false, IntVec( 1 , par_size - 1 ) );
			addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, par_size - 1 ) );
			addPrecondition( name, stack_state_pred, false, incvec( par_size - 2 , par_size ) );
			addPrecondition( name, stack_empty_cond_pred, false, IntVec( 1 , par_size - 2 ) );

			addEffect( name, stack_empty_cond_pred, true, IntVec( 1 , par_size - 2 ) );
			addEffect( name, conds[ *eff ][ procedures ], false, incvec( 0, par_size - 1 ) );

			// Add instruction
			addActionToInstruction( name, conds[ *eff ][ procedures ] );
		}

		// Program TRUE GOTO
		ss.str("");
		ss << "PROGRAM-TRUE-GOTO-" << procedures;
		name = ss.str();

		Action *prog_tgoto = createAction( name , StringVec() = { "STACKSTATE" , "STACKSTATE", "STACKROW" } );

		addPrecondition( name, evaluating_preds[ procedures ], false, IntVec( 1 , 2 ) );
		addPrecondition( name, accumulator_preds[ procedures ] , false, IntVec( 1 , 2 ) );
		addPrecondition( name, action_preds[ procedures ], false, IntVec( 1 , 2 ) );
		addPrecondition( name, stack_top_pred, false, IntVec( 1 , 2 ) );
		addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 2 ) );

		addPrecondition( name, stack_state_pred, false, IntVec() = { 0, 2 } );
		addPrecondition( name, stack_empty_tgoto_pred, false, IntVec( 1 , 0 ) );
		addPrecondition( name, available_pred, false, IntVec( 1, 1 ) );

		addEffect( name, stack_empty_tgoto_pred, true, IntVec( 1 , 0 ) );
		addEffect( name, tgotos[ procedures ], false, incvec( 0 , 2 ) );

		When *wtgoto = new When;
		wtgoto->pars = cd->ground( next_state_pred , IntVec() = { 1, 3 } );
		wtgoto->cond = cd->ground( available_pred , IntVec() = { 3 } );

		Forall *ftgoto = new Forall;
		ftgoto->params = cd->preds.get( available_pred )->params;
		ftgoto->cond = wtgoto;
	
		( ( And * ) prog_tgoto->eff )->add( ftgoto );

		// Add instruction
		addActionToInstruction( name, tgotos[ procedures ] );

		// Program FALSE GOTO
		ss.str( "" );
		ss << "PROGRAM-FALSE-GOTO-" << procedures;
		name = ss.str();

		Action *prog_fgoto = createAction( name , StringVec() = { "STACKSTATE" , "STACKSTATE", "STACKROW" } );

		addPrecondition( name, evaluating_preds[ procedures ], false, IntVec( 1 , 2 ) );
		addPrecondition( name, accumulator_preds[ procedures ], true, IntVec( 1 , 2 ) );
		addPrecondition( name, action_preds[ procedures ] , false, IntVec( 1 , 2 ) );
		addPrecondition( name, stack_top_pred, false, IntVec( 1 , 2 ) );
		addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 2 ) );
		addPrecondition( name, stack_state_pred, false, IntVec() = { 0, 2 } );
		addPrecondition( name, stack_empty_fgoto_pred, false, IntVec( 1 , 0 ) );
		addPrecondition( name, available_pred, false, IntVec( 1, 1 ) );

		addEffect( name, stack_empty_fgoto_pred, true, IntVec( 1 , 0 ) );
		addEffect( name, fgotos[ procedures ], false, incvec( 0 , 2 ) );

		When *wfgoto = new When;
		wfgoto->pars = cd->ground( next_state_pred , IntVec() = { 1, 3 } );
		wfgoto->cond = cd->ground( available_pred , IntVec() = { 3 } );

		Forall *ffgoto = new Forall;
		ffgoto->params = cd->preds.get( available_pred )->params;
		ffgoto->cond = wfgoto;
	
		( ( And * ) prog_fgoto->eff )->add( ffgoto );

		// Add instruction
		addActionToInstruction( name, fgotos[ procedures ] );
	}

	void createFSCTrueGotoActions( unsigned procedures, const StringVec& evaluating_preds, const StringVec &accumulator_preds, const StringVec &action_preds, const StringVec &tgotos ){
		for( unsigned p = 0; p < procedures + 1; p++){
			// Repeat TRUE GOTO
			SStream ss;
			ss << "REPEAT-TRUE-GOTO-" << p ;
			String name = ss.str();
			createAction( name , StringVec() = { "STACKSTATE", "STACKSTATE", "STACKROW" } );

			addPrecondition( name, stack_top_pred, false,IntVec( 1, 2 ) );
			addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, 2 ) );
			addPrecondition( name, tgotos[ p ], false, incvec( 0 , 2 ) );
			addPrecondition( name, stack_state_pred, false, IntVec() = { 0, 2 } );
			addPrecondition( name, evaluating_preds[ p ], false,  IntVec( 1, 2 )  );
			addPrecondition( name, accumulator_preds[ p ], false,  IntVec( 1, 2 ) );
			addPrecondition( name, action_preds[ p ], false, IntVec( 1, 2 ) );
			
			addEffect( name, evaluating_preds[ p ], true,  IntVec( 1, 2 ) );
			addEffect( name, action_preds[ p ], true, IntVec( 1, 2 ) );
			addEffect( name, accumulator_preds[ p ], true, IntVec( 1, 2 ) );
			addEffect( name, stack_state_pred, true, IntVec() = { 0, 2 }  );
			addEffect( name, stack_state_pred, false, IntVec() = { 1, 2 }  );
		}
	}

	void createFSCFalseGotoActions(unsigned procedures, const StringVec& evaluating_preds, const StringVec &accumulator_preds, const StringVec &action_preds, const StringVec &fgotos ){
		for( unsigned p = 0; p < procedures + 1; p++){
			// Repeat FALSE GOTO
			SStream ss;
			ss << "REPEAT-FALSE-GOTO-" << p ;
			String name = ss.str();
			createAction( name , StringVec() = { "STACKSTATE", "STACKSTATE", "STACKROW" } );

			addPrecondition( name, stack_top_pred, false,IntVec( 1, 2 ) );
			addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, 2 ) );
			addPrecondition( name, fgotos[ p ], false, incvec( 0 , 2 ) );
			addPrecondition( name, stack_state_pred, false, IntVec() = { 0, 2 } );
			addPrecondition( name, evaluating_preds[ p ], false,  IntVec( 1, 2 )  );
			addPrecondition( name, accumulator_preds[ p ], true,  IntVec( 1, 2 ) );
			addPrecondition( name, action_preds[ p ], false, IntVec( 1, 2 ) );

			addEffect( name, evaluating_preds[ p ], true,  IntVec( 1, 2 ) );
			addEffect( name, action_preds[ p ], true, IntVec( 1, 2 ) );
			addEffect( name, stack_state_pred, true, IntVec() = { 0, 2 }  );
			addEffect( name, stack_state_pred, false, IntVec() = { 1, 2 }  );
		}
	}

	void createFSCEvalConditions( unsigned procedures, const StringVec& evaluating_preds, const StringVec &accumulator_preds, const StringVec &action_preds, const StringVec &nconds, const StringDVec &conds  ){
		for( unsigned p = 0; p < procedures + 1; p++ ){
			// Evaluate NO CONDITION
			SStream ss; 
			ss << "EVAL-NOCOND-" << p;
			String name = ss.str();
			createAction( name , StringVec() = { "STACKSTATE", "STACKROW" } );

			addPrecondition( name, stack_top_pred, false, IntVec( 1 , 1 ) );
			addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, 1 ) );
			addPrecondition( name, stack_state_pred, false, incvec( 0 , 2 ) );
			addPrecondition( name, nconds[ p ], false, IntVec( 1 , 0 ) );
			addPrecondition( name, evaluating_preds[ p ], true, IntVec( 1 , 1 ) );
					
			addEffect( name, evaluating_preds[ p ], false, IntVec( 1 , 1 ) );
			addEffect( name, accumulator_preds[ p ], false, IntVec( 1 , 1 ) );
		}

		// Evaluate conditions on each original action
		for ( IntSet::iterator eff = effs.begin(); eff != effs.end(); ++eff ){
			for( unsigned p = 0; p < procedures + 1; p++ ){
				SStream ss;
				ss << "EVAL-COND-" << od->preds[ *eff ]->name << "-" << p;
				String name = ss.str();
				StringVec params_aux = od->typeList( od->preds[ *eff ] );
				params_aux.push_back( "STACKSTATE" );
				params_aux.push_back( "STACKROW" );
				unsigned par_size = params_aux.size();

				Action * eval = createAction( name, params_aux );

				addPrecondition( name, stack_top_pred, false, IntVec( 1 , par_size - 1 ) );
				addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, par_size - 1 ) );
				addPrecondition( name, stack_state_pred, false, incvec( par_size - 2 , par_size ) );
				addPrecondition( name, conds[ *eff ][ p ], false, incvec( 0, par_size - 1) );
				addPrecondition( name, evaluating_preds[ p ], true, IntVec( 1 , par_size - 1 ) );
		
				addEffect( name, evaluating_preds[ p ], false, IntVec( 1 , par_size - 1 ) );

				When * w = new When;
				w->cond = new And;
				if ( unclean_effs.find( *eff ) == unclean_effs.end() ) {
					( (And *) w->cond)->add( cd->ground( accumulator_preds[ p ] , IntVec( 1 , par_size - 1 ) ) );
					w->pars = cd->ground( od->preds[ *eff ]->name, incvec( 0, par_size - 2 ) );
					addStackRow( w , par_size - 1 );
					( ( And * )eval->eff )->add( w );
				}
				else {
					for ( unsigned j = 0; j < od->derived.size(); ++j ){
						if ( od->derived[ j ]->name == od->preds[ *eff ]->name ) {
							// To allow jumps over derived predicates
							Exists * exists = dynamic_cast< Exists * >( od->derived[ j ]->cond );
							if ( exists ) {
								w->pars = exists->cond->copy( *cd );
								w->addParams( od->derived[j]->params.size(), 2 );
								( (And *) w->cond )->add( cd->ground( accumulator_preds[ p ] , IntVec( 1 , par_size - 1 ) ) );
								Forall * f = new Forall;
								f->params = exists->params;
								f->cond = w;
								addStackRow( f, par_size - 1 );
								( ( And * )eval->eff )->add( f );
							}
							else {
								w->pars = od->derived[ j ]->cond->copy( *cd );
								w->addParams( od->derived[j]->params.size(), 2 );
								( (And *) w->cond)->add( cd->ground( accumulator_preds[ p ] , IntVec( 1 , par_size - 1 ) ) );
								addStackRow( w, par_size - 1 );
								( ( And * ) eval->eff )->add( w );
							}
							break;
						}
					}
				}
			}
		}
	}

};


#endif
