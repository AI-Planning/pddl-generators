#include "include.h"

class Predicates{
	public:
	Predicates(){}

	void createPredicate( Domain *d, const String& predicate, const StringVec& parameters = StringVec() ){
		d->createPredicate( predicate, parameters );
	}

	void createValuedPredicates(	Domain *d , const String& predicate_name, StringVec& predicate_names,
									unsigned bound, int offset = 0, const StringVec& parameters = StringVec() ){
		predicate_names.resize( bound );
		for( unsigned i = offset; i < bound; i++ ){
			SStream ss;
			ss << predicate_name << "-" << i;
			createPredicate( d, ss.str() , parameters );

			predicate_names[ i ] = ss.str();
		}
	}
	
	void createDValuedPredicates( 	Domain *d, const String& predicate_prefix, StringDVec& predicate_names, 
									unsigned bound1, unsigned bound2, unsigned offset1 = 0, unsigned offset2 = 0, 
									const StringVec& parameters = StringVec() ){
		predicate_names.resize( bound1 );
		for( unsigned i = offset1; i < bound1; i++ ){
			SStream ss;
			ss << predicate_prefix << "-" << i;
			createValuedPredicates( d, ss.str(), predicate_names[ i ], bound2, offset2, parameters );
		}
	}

	void createTValuedPredicates( 	Domain *od, Domain *cd, StringTVec& predicate_names, 
									unsigned bound1, unsigned bound2, unsigned offset1 = 0, unsigned offset2 = 0 ){
		predicate_names.resize( od->preds.size() );
		for( int effect_id : effs ){
			predicate_names.emplace_back( );
			String prefix = "COND-" + od->preds[ effect_id ]->name;
			StringVec parameters = od->typeList( od->preds[ effect_id ] );
			
			/*if( IS_HIGH_LEVEL ){
				for( unsigned j = 1; j < parameters.size(); j++ )
					parameters[ j ] = "CONDITION-VARIABLE";
			}*/

			createDValuedPredicates( cd, prefix, predicate_names[ effect_id ], bound1, bound2, offset1, offset2, parameters );
		}
	}

	void createTValuedPredicates( 	Domain *d, const String& predicate_prefix, StringTVec& predicate_names, 
									unsigned bound1, unsigned bound2, unsigned bound3,
									unsigned offset1 = 0, unsigned offset2 = 0, unsigned offset3 = 0,
									const StringVec& parameters = StringVec() ){
		predicate_names.resize( bound1 );
		for( unsigned i = offset1; i < bound1; i++ ){
			SStream ss;
			ss << predicate_prefix << "-" << i;
			createDValuedPredicates( d, ss.str(), predicate_names[ i ], bound2, bound3, offset2, offset3, parameters );
		}
	}


	void copyPredicates( Domain *od, Domain *cd ){
		for ( unsigned i = 0; i < od->preds.size(); ++i ){
			String predicate_name = od->preds[ i ]->name;
			StringVec parameter_types = od->typeList( od->preds[ i ] );
			// Check if the predicate must be stackable
			if( stack_predicates.find( predicate_name ) != stack_predicates.end() ){
				parameter_types.push_back( "STACKROW" );
			}
			createPredicate( cd, predicate_name, parameter_types );
		}
	}

	void createPredicates(  Domain *od, Domain *cd, unsigned total_procedures, unsigned total_lines, 
							unsigned total_instances, StringVec &tests, StringVec &empty_lines, 
							StringDVec &end_instructions, StringTVec &calls, StringTVec &instructions, StringTVec &choiceInstr ){
		
		createPredicate( cd , "EMPTY-STACK" );

		// Link the stack rows
		StringVec parameters( 2 , "STACKROW" );
		createPredicate( cd, "NEXT-STACK-ROW", parameters );

		// Add top stack predicate
		parameters.resize( 1 );
		parameters.shrink_to_fit();
		createPredicate( cd, "TOP-STACK", parameters );

		// Add evaluation predicates
		createPredicate( cd, "ACCUMULATOR" );
		createPredicate( cd, "DONE-EVALUATING" );

		if( EVAL_DERIVED ){
			createPredicate( cd, "TODO-DERIVED" );
			createPredicate( cd, "DONE-DERIVED" );
		}

		// Add goal predicate
		createPredicate( cd, "DONE-PROGRAMMING" );

		// Add procedure predicates
		createValuedPredicates( cd , "STACK-PROCEDURE", stack_procedures, total_procedures, 0, parameters );

		stack_procedures.emplace_back( "STACK-MAIN" );
		createPredicate( cd, "STACK-MAIN", parameters );

		// Add predicates for each pair line-row in the stack
		createValuedPredicates( cd, "STACK-LINE", stack_lines, total_lines + 1, 0, parameters );

		// Add TEST predicates for each training instance
		createValuedPredicates( cd, "TEST", tests, total_instances );
	
		// Add empty predicates for each pair main-line
		createValuedPredicates( cd, "EMPTY", empty_lines, total_lines + 1 );

		// Add end instructions predicates for each pair procedure-line and main-line
		createDValuedPredicates( cd , "INS-END", end_instructions, total_procedures, total_lines + 1, 0, 1 );
		end_instructions.resize( total_procedures + 1 );
		createValuedPredicates( cd, "INS-END", end_instructions[ total_procedures ], total_lines + 1, 1 );

		// Add CALL predicates for procedures
		StringVec stack_parameters( stack_variables.begin(), stack_variables.end() );
		
		calls.resize( total_procedures + 1 );
		for( unsigned p = 0; p < total_procedures + 1; p++ ){
			SStream ss;
			ss << "CALL-" << p;
			createDValuedPredicates( cd, ss.str(), calls[ p ], total_procedures + 1, total_lines, p, 0, stack_parameters );
		}

	
		// Add instruction predicates for each pair procedure-line and main-line
		instructions.resize( od->actions.size() );	
		for ( unsigned i = 0; i < od->actions.size(); ++i ){
			String action_name = "INS-" + od->actions[ i ]->name;
			StringVec parameters = StringVec();
			if( IS_HIGH_LEVEL || INSTR_WITH_PARAMS )
				parameters = od->typeList( od->actions[ i ] );
			createDValuedPredicates( cd, action_name, instructions[ i ], total_procedures, total_lines, 0, 0, parameters );
			instructions[ i ].resize( total_procedures + 1 );	
			createValuedPredicates( cd, action_name, instructions[ i ][ total_procedures ], total_lines, 0, parameters);		
		}

		// Add instructions predicates for choice-actions
		choiceInstr = StringTVec( od->preds.size(), StringDVec() );
		for ( int id : effs ){
			StringVec pars = od->typeList( od->preds[id] );
			bool b = pars.size() && od->types.index( od->types.get( pars[ 0 ] )->name + "-VARIABLE" ) >= 0;
			for ( unsigned i = 1; i < pars.size(); ++i )
				b &= pars[ i ] == pars[ 0 ];

			if ( b ){
				choiceInstr[ id ] = StringDVec( total_procedures + 1 );
			}
			
			for( unsigned procedure = 0; b && procedure < total_procedures + 1; procedure++ ){
				for( unsigned line = 0; line < total_lines; line++ ) {
					SStream ss;
					ss << "INS-CHOICE-" << od->preds[id]->name << "-" << line;
					choiceInstr[ id ][ procedure ].emplace_back( ss.str() );
					createPredicate( cd, choiceInstr[ id ][ procedure ][ line ], StringVec( 1, pars[ 0 ] + "-VARIABLE" ) );
				}
			}
		}

	}
	
};
