#ifndef CHOICE_H
#define CHOICE_H

#include "include.h"

class ChoiceActions : public Actions {
	public:
	ChoiceActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ) {}

	void createProgrammingActions( StringTVec& choiceInstr, unsigned procedures, unsigned lines, StringVec& empty_lines ){		
		String prefix_name = "PROGRAM-CHOICE-";

		for ( int id : effs ) {
			StringVec pars = od->typeList( od->preds[id] );
			bool b = pars.size() && od->types.index( od->types.get( pars[0] )->name + "-VARIABLE" ) >= 0;
			for ( unsigned i = 1; i < pars.size(); ++i )
				b &= pars[ i ] == pars[0];

			for( unsigned procedure = 0; b && procedure < procedures + 1; procedure++ )
				for( unsigned line = 0; line < lines; line++ ) {
					SStream ss;
					// what to put for procedures?
					ss << od->preds[id]->name << "-" << line;
							
					StringVec parameters( 1, pars[ 0 ] + "-VARIABLE" );
					parameters.emplace_back( "STACKROW" );

					String name = prefix_name + ss.str();
					createAction( name, parameters );

					addPrecondition( name, "TOP-STACK", false, IntVec( 1, 1 ) );
					addPrecondition( name, "STACK-MAIN", false, IntVec( 1, 1 ) );
					addPrecondition( name, stack_lines[ line ], false, IntVec( 1, 1 ) );
					addPrecondition( name, empty_lines[ line ], false );

					addEffect( name, empty_lines[ line ], true );
					addEffect( name, choiceInstr[ id ][ procedure ][ line ], false, IntVec( 1, 0 ) );

					addCost( name, 1001 );

					addActionToInstruction( name, choiceInstr[ id ][ procedure ][ line ] );
				}
		}
	}

	void createRepeatActions( StringTVec& choiceInstr, unsigned procedures, unsigned lines ){
		String prefix_name = "REPEAT-CHOICE-";
		for ( int id : effs ) {
			StringVec pars = od->typeList( od->preds[ id ] );
			for( unsigned procedure = 0; choiceInstr[ id ].size() && procedure < procedures + 1; procedure++ )
				for( unsigned line = 0; line < lines; line++ ) {
					SStream ss;
					// what to put for procedures?
					ss << od->preds[id]->name << "-" << procedure << "-" << line;
					
					StringVec parameters( 1, pars[0] + "-VARIABLE" );
					parameters.emplace_back( pars[0] );
					parameters.emplace_back( "STACKROW" );

					String name = prefix_name + ss.str();
					Action *choice = createAction( name, parameters );

					addPrecondition( name, "TOP-STACK", false, IntVec( 1, 2 ) );
					addPrecondition( name, "STACK-MAIN", false, IntVec( 1, 2 ) );
					addPrecondition( name, stack_lines[ line ], false, IntVec( 1, 2 ) );
					addPrecondition( name, choiceInstr[ id ][ procedure ][ line ], false, IntVec( 1, 0 ) );
					addPrecondition( name, od->preds[ id ]->name, false, IntVec( od->preds[ id ]->params.size(), 1 ) );

					addEffect( name, stack_lines[line], true, IntVec( 1, 2 ) );
					addEffect( name, stack_lines[line+1], false, IntVec( 1, 2 ) );
					addEffect( name, pars[ 0 ] + "-ASSIGNMENT", false, incvec( 0, parameters.size() ) );
					Forall * f = new Forall;
					f->params.push_back( cd->types.index( pars[0] ) );
					IntVec vec( 1, 0 ); // variable
					vec.emplace_back( parameters.size() ); // Forall parameter
					vec.emplace_back( parameters.size() - 1 ); // STACKROW
					f->cond = new Not( cd->ground( pars[ 0 ] + "-ASSIGNMENT", vec ) );
					( ( And * )choice->eff )->conds.emplace_back( f );
				}
		}
	}
};


#endif
