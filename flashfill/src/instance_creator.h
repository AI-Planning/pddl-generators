#include "include.h"

class InstanceCreator{
	private:
	Domain *od;
	Domain *cd;
	Actions *actions;

	public:
	InstanceCreator( ){}

	static void copyInitialState( Domain *d, Instance *oins, Instance *cins ){
		for( unsigned i = 0; i < oins->init.size(); i++ ){
			Ground *g = oins->init[ i ];
			Type *t = d->getType( g->name );
			int predicate_idx = d->preds.index( t->name );
			if( predicate_idx >= 0 ){
				StringVec parameters = d->objectList( g );
				// Add first row to stackable predicates
				if( stack_predicates.find( t->name ) != stack_predicates.end() ){
					stack_constants.emplace_back( StringVec() );
					for( unsigned j = 0; j < parameters.size(); j++ ){
						// Add the name of the base constants for local predicates
						int parameter_type_id = g->lifted->params[ j ];
						String parameter_type_name = d->types[ parameter_type_id ]->name;
						if( stack_variables.find( parameter_type_name ) != stack_variables.end() ){
							stack_constants[ stack_constants.size() - 1 ].emplace_back( parameters[ j ] );
						}
					}
					parameters.emplace_back( "ROW-0" );
				}
				cins->addInit( t->name, parameters );
			}	
		}
	}
	
	static Instance* create( Domain *od, Domain *cd, Instance *oins, const StringVec& rows, const StringVec& empty_lines, unsigned procedures, bool to_program = true, bool instance_metric = true, const StringVec& slots = StringVec(), unsigned noclasses = 0 ){

		Instance *cins = new Instance( *cd );
		cins->name = oins->name;
		cins->metric = instance_metric;

		copyInitialState( od, oins, cins );

		// Add ToDo derived to evaluate derived as first step	
		if( EVAL_DERIVED && !IS_HIGH_LEVEL )
			cins->addInit( "TODO-DERIVED" );

		// Add initial test
		cins->addInit( "TEST-0" );
	
		// Add stack rows connections
		for( unsigned i = 1; i < rows.size(); i++ ){
			StringVec parameters = { rows[ i - 1 ], rows[ i ] };
			//current_rows.push_back( rows[i-1] );
			//current_rows.push_back( rows[i] );
			cins->addInit( "NEXT-STACK-ROW" , parameters );
		}

		// Start programming the main procedure
		cins->addInit( "STACK-MAIN", StringVec( 1 , "ROW-0" ) );

		// Start in the first line
		cins->addInit( "STACK-LINE-0", StringVec( 1 , "ROW-0" ) );					   

		// Set the top row of the stack
		cins->addInit( "TOP-STACK", StringVec( 1 , "ROW-0" ) );

		if( to_program ){
			if( !IS_UNSUPERVISED ){
				// Instructions start empty
				for ( unsigned k = 0; k < empty_lines.size(); ++k ) {
					cins->addInit( empty_lines[ k ] );
				}
				//[PERFORMANCE] Adding no-gotos
				/*for( unsigned line_from = 0; line_from + 1 < empty_lines.size(); line_from++ ){
					for( unsigned line_to = 0; line_to < empty_lines.size(); line_to++ ){
						if( line_from == line_to || (line_from+1)==line_to ) continue;
						std::string notgoto = "NOT-GOTO-"+ std::to_string( procedures ) + "-" + std::to_string( line_from ) + "-" + std::to_string( line_to );
						cins->addInit( notgoto );
					}
				}*/
				//[END-OF-PERFORMANCE]
			}
			else{ // UNSUPERVISED
				for( unsigned k = 0; k < noclasses; k++ ){
					SStream ss;
					ss << "PROG-" << k;
					cins->addInit( "CHOSEN-PROGRAM", StringVec( 2, ss.str() ) );
				}

				unsigned program_lines = ((empty_lines.size()) / noclasses);
				// Initial instruction is a choice of a program (classifier)
				cins->addInit( "INS-SELECT-PROGRAM" );
				for( unsigned k = 1; k < empty_lines.size(); ++k ){
					// the last line of  classified program is an end
					if( (k % program_lines) == 0 ){
						SStream end_prog;
						end_prog << "INS-END-" << k;
						cins->addInit( end_prog.str() );
					}
					else{
						cins->addInit( empty_lines[ k ] );
					}
				}
			}
		}

		if( IS_HIGH_LEVEL ){			
			cins->addInit( "CURRENT-SLOT", StringVec( 1, slots[ 0 ] ) );
		}

		// We are done programming
		cins->addGoal( "DONE-PROGRAMMING" );
		
		return cins;
	}

	static Instance* createFSCInstance(Domain *od, Domain *cd, Instance *oins, unsigned procedures, const StringVec& constant_states, const StringVec& rows, unsigned nstates, bool to_program = true, bool instance_metric = true ){
		Instance *cins = new Instance( *cd );
		cins->name = oins->name;
		cins->metric = instance_metric;
		
		copyInitialState( od, oins, cins );

		// add initial test
		cins->addInit( "TEST-0" );

		// start of first line
		cins->addInit( stack_state_pred, StringVec() = { "STATE-0", "ROW-0" } );

		// start programming with main
		cins->addInit( stack_procedures[ procedures ], StringVec( 1 , "ROW-0" ) );


		// set the top row of the stack
		cins->addInit( stack_top_pred, StringVec( 1 , "ROW-0" ) );

		// instructions are empty
		if( to_program ){
			for ( unsigned s = 0; s < nstates; ++s ){
				cins->addInit( stack_empty_cond_pred , StringVec( 1 , constant_states[ s ] ) );
				cins->addInit( stack_empty_tgoto_pred , StringVec( 1 , constant_states[ s ] ) );
				cins->addInit( stack_empty_fgoto_pred , StringVec( 1 , constant_states[ s ] ) );
				cins->addInit( stack_empty_tact_pred , StringVec( 1 , constant_states[ s ] ) );
				cins->addInit( stack_empty_fact_pred , StringVec( 1 , constant_states[ s ] ) );
			}
			cins->addInit( stack_empty_cond_pred, StringVec( 1, constant_states[ nstates ] ) ); // Allows to program end in last state
		}

		// add stack rows connections
		for( unsigned i = 1; i < stack_size; i++){
			cins->addInit( stack_row_pred , StringVec() = { rows[ i - 1 ], rows[ i ] });
		}

		// Availability of the states
		cins->addInit( available_pred , StringVec( 1 , constant_states[ 0 ] ) );
		cins->addInit( available_pred , StringVec( 1 , constant_states[ 1 ] ) );
		unsigned const_size = constant_states.size();
		//if( const_size > size_t( 2 ) )
		//	cins->addInit( available_pred , StringVec( 1 , constant_states[ int( const_size ) - 1 ] ) );
	
		// Add the states relation
		for( unsigned i = 0; i + 1 < const_size; i++ ){
			cins->addInit( next_state_pred , StringVec() = { constant_states[ i ], constant_states[ i + 1 ] } );
		}

		// we are done programming (only for mains)
		cins->addGoal( goal_pred );
	
		return cins;
	}

	static Instance* createCFGInstance( Domain *od, Domain *cd, Instance *oins, const StringVec& rows, const StringVec& empty_lines, bool to_program = true, bool instance_metric = true, const StringVec& select_programs = StringVec(), unsigned bound = 1){

		Instance *cins = new Instance( *cd );
		cins->name = oins->name;
		cins->metric = instance_metric;

		copyInitialState( od, oins, cins );

		// Add initial test
		cins->addInit( "TEST-0" );
	
		// Add stack rows connections
		for( unsigned i = 1; i < rows.size(); i++ ){
			StringVec parameters = { rows[ i - 1 ], rows[ i ] };
			cins->addInit( "NEXT-STACK-ROW" , parameters );
		}

		// Start programming the main procedure
		cins->addInit( "STACK-MAIN", StringVec( 1 , "ROW-0" ) );

		// Start in the first line
		cins->addInit( "STACK-LINE-0", StringVec( 1 , "ROW-0" ) );					   

		// Set the top row of the stack
		cins->addInit( "TOP-STACK", StringVec( 1 , "ROW-0" ) );

		// Initial instruction is a choice of a program (classifier)
		for( unsigned proc = 0; proc < select_programs.size(); proc++ )
			cins->addInit( select_programs[ proc ] );

		if( to_program ){
			// Last case is already programmed and its an END instruction
			for( unsigned line = 1; line < bound; line++ ){
				cins->addInit( empty_lines[ line ] );
			}
		}

		// Initial allowed programs and lines
		//cins->addInit( "ALLOWED-LINE-1" );
		SStream ss;
		//ss << "ALLOWED-LINE-" << empty_lines.size() - 1;
		ss << "ALLOWED-LINE-" << (select_programs.size() - 1u) << "-1";
		cins->addInit( ss.str() );
		ss.str("");
		ss << "ALLOWED-LINE-" << (select_programs.size() - 1u) << "-" << bound;
		cins->addInit( ss.str() );
		ss.str("");
		ss << "INS-END-" << bound;
		cins->addInit( ss.str() );
			
		// We are done programming
		cins->addGoal( "DONE-PROGRAMMING" );
		
		return cins;
	}

};
