#ifndef TERMINAL_H
#define TERMINA_H

#include "actions.h"

class TerminalActions : public Actions{

	public:

	TerminalActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}
	
	void createProgrammingActions( unsigned procedures, unsigned lines, StringVec& empty_lines, StringDVec& endinstr, const StringDVec& allowed_lines = StringDVec() ){
		unsigned offset = 1;

		createValuedActions( "PROGRAM-END", lines + 1, offset, StringVec( 1, "STACKROW" ) );

		StringVec action_names = getNames( "PROGRAM-END" );

		for( unsigned idx = 0; idx < action_names.size(); idx++ ){
			String name = action_names[ idx ];
			unsigned line = idx + offset;

			addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) ); // top
			addPrecondition( name, "STACK-MAIN", false, IntVec( 1, 0 ) ); // main
			addPrecondition( name, empty_lines[ line ] ); // assign corresponding empty
			addPrecondition( name, stack_lines[ line ], false, IntVec( 1, 0 ) ); // stack line

			addEffect( name, empty_lines[ line ], true );
			addEffect( name, endinstr[ procedures ][ line ], false );

			if( compiler_type == "CFG" ){
				if( line < lines )
					addEffect( name, allowed_lines[ procedures ][ line + 1 ] );				
			}
			else{
				addPrecondition( name, "TEST-0" ); // first test
			}

			addCost( name, 1 );
		
			// Action to instruction
			addActionToInstruction( name, endinstr[ procedures ][ line ] );
		}
	}

	void createFSCProgrammingActions( unsigned procedures, unsigned nstates, const StringDVec& endinstr, const StringVec& constant_states ){
		//unsigned offset = 1; // Allows to program from first state
		unsigned offset = nstates; // Forces to program end in last state
		createValuedActions( "PROGRAM-END", nstates + 1, offset, StringVec() = { "STACKROW" } );

		StringVec action_names = getNames( "PROGRAM-END" );

		for( unsigned idx = 0; idx < action_names.size(); idx++ ){
			String name = action_names[ idx ];
			unsigned state = idx + offset;			
			int stateIdx = cd->constantIndex( constant_states[ state ] , "STACKSTATE" );

			addPrecondition( name, stack_top_pred, false, IntVec( 1 , 0 ) );
			addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, 0 ) );			
			addPrecondition( name, stack_state_pred, false, IntVec() = { stateIdx, 0 } );
			addPrecondition( name, stack_empty_cond_pred, false, IntVec( 1 , stateIdx ) );

			addEffect( name, stack_empty_cond_pred, true, IntVec( 1 , stateIdx ) );
			addEffect( name, endinstr[ procedures ][ state ] );

			addCost( name, 1 );
		
			// Action to instruction
			addActionToInstruction( name, endinstr[ procedures ][ state ] );
		}
	}

	void createRepeatTerminalAction( unsigned procedures, unsigned total_instances, unsigned lines, StringDVec& endinstr, StringVec& tests ){
		// Repeat ends for other procedures or the main recursion
		for( unsigned p = 0; p < procedures + 1 ; p++ ){
			for( unsigned t = 0; t < total_instances; t++){
				for ( unsigned j = 1; j < lines + 1; j++ ) {
					SStream ss;
					ss << "REPEAT-END-" << p << "-" << t << "-" << j;
					String name = ss.str();
				
					Action* endact = createAction( name, StringVec( 2, "STACKROW" ) );
					addPrecondition( name, endinstr[ p ][ j ] );
					addPrecondition( name, tests[ t ] );
					addPrecondition( name, "NEXT-STACK-ROW", 	   false, incvec( 0, 2 ) );
					addPrecondition( name, "TOP-STACK", 		   false, IntVec( 1, 1 ) );
					addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, 1 ) );
					addPrecondition( name, stack_lines[ j ], 	   false, IntVec( 1, 1 ) );

					addEffect( name, "TOP-STACK", 			 true,  IntVec( 1, 1 ) );
					addEffect( name, "TOP-STACK", 			 false, IntVec( 1, 0 ) );
					addEffect( name, stack_procedures[ p ], true,  IntVec( 1, 1 ) );
					addEffect( name, stack_lines[ j ], 	 true,  IntVec( 1, 1 ) );			

					// Delete all the stackable fluents of the current level
					for( String pred : stack_predicates ){
						Forall * f = new Forall;
						Lifted *l = cd->preds.get( pred );
						f->params = l->params;
						(f->params).resize( (l->params).size() - 1 );
						IntVec parameters;
						for(size_t i = 0; i < f->params.size(); i++)
							parameters.push_back( i + 2 );
						parameters.push_back( 1 );
						f->cond = new And;	
						( ( And * )f->cond )->add( new Not( cd->ground( pred, parameters ) ) );
						( ( And * )endact->eff )->add( f );
					}

					addCost( name, 1 );
				}
			}
		}
	}
	
	void createRepeatTerminalMain(	InstVec &ins, unsigned procedures, unsigned total_instances, 
									unsigned lines, StringDVec& endinstr, StringVec& tests ){
		// Action for the Main in the first stack level
		for( unsigned t = 0; t < total_instances; t++ ){
			for( unsigned j = 1; j < lines + 1; j++ ){
				SStream ss;
				ss << "REPEAT-END-MAIN-" << t << "-" << j;
				String name = ss.str();
			
				Action *endact = createAction( name, StringVec( 1, "STACKROW" ) );
				
				addPrecondition( name, "TOP-STACK",  false, IntVec( 1, 0 ) );
				addPrecondition( name, "TOP-STACK",  false, IntVec( 1, cd->constantIndex( "ROW-0", "STACKROW" ) ) );
				addPrecondition( name, "STACK-MAIN", false, IntVec( 1, cd->constantIndex( "ROW-0", "STACKROW" ) ) );
				addPrecondition( name, stack_lines[ j ], false, IntVec( 1, cd->constantIndex( "ROW-0", "STACKROW" ) ) );
				addPrecondition( name, endinstr[ procedures ][ j ] );
				addPrecondition( name, tests[ t ] );

				for ( unsigned i = 0; i < ins[ t ]->goal.size(); ++i ) {
					Ground* goal = ins[ t ]->goal[ i ];
					Type *t = od->getType( goal->name );
					StringVec objects = od->objectList( goal );
					Lifted * l = od->preds.get( goal->name );
					IntVec constant_idxs;
					for ( unsigned k = 0; k < objects.size(); k++ ){
						int constant_idx = cd->constantIndex( objects[ k ], od->types[ l->params[ k ] ]->name );
						constant_idxs.emplace_back( constant_idx );
					}

					if( stack_predicates.find( t->name ) != stack_predicates.end() ){ // Add stack row
						constant_idxs.emplace_back( cd->constantIndex( "ROW-0", "STACKROW" ) );
					}
					
					addPrecondition( name, goal->name, false, constant_idxs );
				}

				if( t + 1 == total_instances ){
					addEffect( name, goal_pred );
				}
				else{
					addEffect( name, tests[ t ], true );
					addEffect( name, tests[ t + 1 ] );
					addEffect( name, stack_lines[ j ], true, IntVec( 1, 0 ) );
					addEffect( name, stack_lines[ 0 ], false, IntVec( 1, 0 ) );
				
					std::vector< std::pair< String , IntVec > > add_eff;
				
					// Add effects of the next instance
					unsigned ins_size = ins[ t + 1 ]->init.size();
					for ( unsigned i = 0; i < ins_size; i++ ){ 
						Ground* init = ins[ t + 1 ]->init[ i ]; 

						Type *t = od->getType( init->name );

						StringVec objects = od->objectList( init );

						Lifted *l = od->preds.get( init->name );

						IntVec constant_idxs;
						for( unsigned k = 0; k < objects.size(); k++ ){
							int constant_idx = cd->constantIndex( objects[ k ], od->types[ l->params[ k ] ]->name );
							constant_idxs.emplace_back( constant_idx );
						}

						if( stack_predicates.find( t->name ) != stack_predicates.end() ){ // Add stack row
							constant_idxs.emplace_back( cd->constantIndex( "ROW-0", "STACKROW" ) );
						}

						addEffect( name, init->name, false, constant_idxs );

						add_eff.emplace_back( std::make_pair( init->name, constant_idxs ) );
					}
				
					// Delete fluents of the current instance
					for ( unsigned i = 0; i < od->preds.size(); i++ ) {
						//if( unclean_effs.find( i ) != unclean_effs.end() ) continue;
						if( cd->preds[ i ]->params.size() ){
							Forall * f = new Forall;
							f->params = cd->preds[ i ]->params;

							When *w = new When();
							w->pars = new And();
							( ( And * ) w->pars )->add( cd->ground( od->preds[ i ]->name, incvec( 1, f->params.size() + 1 ) ) );
							/**for( size_t k = 0; k < add_eff.size(); k++ ){
								if( add_eff[ k ].first == od->preds[ i ]->name ){
									( ( And * ) w->pars )->add( new Not( cd->ground( od->preds[ i ]->name, add_eff[ k ].second ) ) );
								}
							}*/

							w->cond = new Not( cd->ground( od->preds[ i ]->name, incvec( 1, f->params.size() + 1 ) ) );
							f->cond = w;
							( ( And * )endact->eff )->add( f );
						}
						else
							cd->addEff( 1, name, od->preds[ i ]->name );
					}
				}			
			}	
		}
	}

	void createRepeatActions(	InstVec &ins, unsigned procedures, unsigned total_instances, 
								unsigned lines, StringDVec& endinstr, StringVec& tests ){

		createRepeatTerminalMain( ins, procedures, total_instances, lines, endinstr, tests );
		createRepeatTerminalAction( procedures, total_instances, lines, endinstr, tests );

	}

	void createFSCRepeatActions( InstVec &ins, unsigned procedures, unsigned total_instances, unsigned bound, unsigned max_bound, const StringVec &constant_states, const StringVec &tests, const StringDVec &endinstr ){

		int ci_row0 = cd->constantIndex( "ROW-0" , "STACKROW" ); 
		//int ci_endstN = cd->constantIndex( constant_states[ bound ] , "STACKSTATE" ); 
		int ci_endst0 = cd->constantIndex( constant_states[ 0 ] , "STACKSTATE" ); 
		//IntVec iv_pre = { ci_endstN, ci_row0 }; 
		IntVec iv_eff = { ci_endst0, ci_row0 };

		for( unsigned t = 0; t < total_instances; t++){
			for( unsigned state = 1; state < bound + 1; state++ ){
				IntVec iv_pre = { cd->constantIndex( constant_states[ state ], "STACKSTATE" ), ci_row0 }; 
				SStream ss;
				ss << "REPEAT-END-MAIN-" << t << "-" << procedures << "-" << state;
				String name = ss.str();

				Action * endact = createAction( name );

				addPrecondition( name, tests[ t ] );
				addPrecondition( name, stack_top_pred, false, IntVec( 1, ci_row0 ) );
				addPrecondition( name, stack_state_pred, false, iv_pre );
				addPrecondition( name, endinstr[ procedures ][ state ] );

				for ( unsigned g = 0; g < ins[ t ]->goal.size(); ++g ) {
					String goal_name = ins[ t ]->goal[ g ]->name;
					Lifted * l = od->preds.get( goal_name );
					StringVec params = od->objectList( ins[ t ]->goal[ g ] );
					IntVec iv_parameters;
					for ( unsigned j = 0; j < params.size(); ++j )
						iv_parameters.push_back( cd->constantIndex( params[ j ], od->types[ l->params[ j ] ]->name ) );
					addPrecondition( name, goal_name , false, iv_parameters );
				}

				if( t + 1 == total_instances ){
					addEffect( name, goal_pred );
				}
				else{
					addEffect( name, tests[ t ], true );
					addEffect( name, tests[ t + 1 ] );
					addEffect( name, stack_state_pred, true, iv_pre );
					addEffect( name, stack_state_pred, false, iv_eff );

					for ( unsigned p = 0; p < od->preds.size(); ++p ) {
						if( unclean_effs.find( p ) != unclean_effs.end() ) continue;
						//if( effs.find(i) == effs.end() ) continue; //[PERFORMANCE] static fluents
						if( cd->preds[ p ]->params.size() ){
							Forall * f = new Forall;
							f->params = cd->preds[ p ]->params;
							f->cond = new Not( cd->ground( od->preds[ p ]->name, incvec( 0, f->params.size() ) ) );
							( ( And * )endact->eff )->add( f );
						}
						else
							addEffect( name, od->preds[ p ]->name, true );
					}
			
					for ( unsigned i = 0; i < ins[ t + 1 ]->init.size(); ++i ){ 
						String init_name = ins[ t + 1 ]->init[ i ]->name;
						Lifted *l = od->preds.get( init_name ); 
						StringVec params = od->objectList( ins[ t + 1 ]->init[ i ] ); 
						IntVec iv_parameters;
						for( unsigned j = 0; j < params.size(); ++j){ 
							iv_parameters.push_back( cd->constantIndex( params[ j ], od->types[ l->params[ j ] ]->name ) );
						}
						// Add rows[ 0 ] to this stackable predicate
						if( stack_predicates.find( init_name ) != stack_predicates.end() ){
							iv_parameters.push_back( ci_row0 );
						}
						addEffect( name, init_name, false, iv_parameters );	
					}
				}
			}
		}

		for( unsigned t = 0; t < total_instances; t++ ){
			for( unsigned p = 0; p < procedures + 1; p++){
				for( unsigned state = 1; state < max_bound + 1; state++ ){


					SStream ss;
					ss << "REPEAT-END-" << t << "-" << p << "-" << state;
					String name = ss.str();
		
					Action * endact = createAction( name, StringVec( 2, "STACKROW" ) );

					//IntVec stack_state_iv = { ci_endstN, 1 };

					IntVec stack_state_iv = { cd->constantIndex( constant_states[ state ] , "STACKSTATE" ), 1 }; 

					addPrecondition( name, tests[ t ] );
					addPrecondition( name, stack_row_pred, false, incvec( 0, 2 ) );					
					addPrecondition( name, stack_top_pred, false, IntVec( 1, 1 ) );
					addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, 1 ) );
					addPrecondition( name, stack_state_pred, false, stack_state_iv );
					addPrecondition( name, endinstr[ p ][ state ] );

					addEffect( name, stack_top_pred, true, IntVec( 1, 1 ) );
					addEffect( name, stack_top_pred, false, IntVec( 1, 0 ) );
					addEffect( name, stack_state_pred, true, stack_state_iv );
					addEffect( name, stack_procedures[ p ], true, IntVec( 1, 1 ) );

					//[BUG] Add Forall delete ASSIGNMENT from this stackrow
					for( StringSet::iterator it = stack_predicates.begin(); it != stack_predicates.end(); it++ ){
						Forall * f = new Forall;
						Lifted *l = od->preds.get( *it );
						f->params = l->params;
						IntVec iv_parameters;
						for( size_t i = 0; i < f->params.size(); i++ )
							iv_parameters.push_back( i + 2 );
						iv_parameters.push_back( 1 );
						f->cond = new And;	
						( ( And * )f->cond )->add( new Not( cd->ground( *it, iv_parameters ) ) );
						( ( And * )endact->eff )->add( f );
					}
				}

			}
		}
	}
};

#endif
