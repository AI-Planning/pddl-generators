#ifndef HL_COND_H
#define HL_COND_H

class HLCondActions : public Actions{
	public:
	HLCondActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}

	void recursive_step_conditions( unsigned k, UIntVec & apars, const StringVec& constant_slots, const StringVec &constant_vars, unsigned novariables, const StringQVec& hl_conds ){
		if ( k == 4 + od->preds[ apars[ 3 ] ]->params.size() ) {
			// Step condition
			SStream ss;
			ss << "STEP-COND-" << od->preds[ apars[ 3 ] ]->name << "-" << apars[ 1 ] << "-" << apars[ 2 ] << "-" << apars[ 4 ];
			for ( unsigned j = 5; j < 4 + od->preds[ apars[ 3 ] ]->params.size(); ++j )
				ss << "-" << apars[ j ];
			String name = ss.str();

			StringVec pars = od->typeList( od->preds[ apars[ 3 ] ] );
			pars.resize( 1 );
			pars.insert( pars.begin(), "STACKROW" );
		
			Action * stepcond = createAction( name, pars );

			addPrecondition( name, stack_top_pred, false, IntVec( 1, 0 ) );
			addPrecondition( name, stack_procedures[ apars[ 1 ] ], false, IntVec( 1, 0 ) );
			addPrecondition( name, stack_lines[ apars[ 2 ] ], false, IntVec( 1, 0 ) );

			IntVec mypars( 1, 1 );
			for ( unsigned j = 5; j < 4 + od->preds[ apars[ 3 ] ]->params.size(); ++j ){
				mypars.push_back( cd->types.get( "CONDITION-VARIABLE" )->parseConstant( constant_vars[ apars[ j ] ] ).second );
			}
			
			addPrecondition( name, hl_conds[ apars[ 3 ] ][ apars[ 1 ] ][ apars[ 2 ] ][ apars[ 4 ] ], false, mypars );
			addPrecondition( name, "CURRENT-SLOT", false, IntVec( 1, cd->constantIndex( constant_slots[ apars[ 4 ] - 1], "CONDITION-SLOT" ) ) );
			if ( apars[ 4 ] == 1 ) 
				addPrecondition( name, eval_pred, true );

			addEffect( name, "CURRENT-SLOT", true, IntVec( 1, cd->constantIndex( constant_slots[ apars[ 4 ] - 1 ], "CONDITION-SLOT" ) ) );
			addEffect( name, "CURRENT-SLOT", false, IntVec( 1, cd->constantIndex( constant_slots[ apars[ 4 ] ], "CONDITION-SLOT" ) ) );

			Forall * f = new Forall;
			f->params.insert( f->params.end(), novariables, cd->types.index( "VALUE" ) );
			When * w = new When;
		
			w->pars = new And;
			if ( 1 < apars[ 4 ] ) {
				IntVec ppars = incvec( 0, novariables + 1 );
				ppars[ 0 ] = cd->constantIndex( constant_slots[ apars[ 4 ] - 1 ], "CONDITION-SLOT" );
				( ( And * )w->pars )->add( cd->ground( "POSSIBLE-VALUE", ppars ) );
			}
			IntVec cpars( 1, 0 );
			for ( unsigned j = 5; j < 4 + od->preds[ apars[ 3 ] ]->params.size(); ++j )
				cpars.push_back( apars[ j ] );
			( ( And * )w->pars )->add( cd->ground( od->preds[ apars[ 3 ] ]->name, cpars ) );

			w->cond = new And;
			IntVec ppars = incvec( 0, novariables + 1 );
			ppars[ 0 ] = cd->constantIndex( constant_slots[ apars[ 4 ] ], "CONDITION-SLOT" );
			( ( And * )w->cond )->add( cd->ground( "POSSIBLE-VALUE", ppars ) );
			// Use offset param in forall because of STACKROW param
			w->addParams( 0 , 1 );

			f->cond = w;
			( ( And * )stepcond->eff )->add( f );

			addCost( name, 1 );
		}
		else for ( apars[ k ] = 1; apars[ k ] < novariables + 1; ++apars[ k ] )
			recursive_step_conditions( k + 1, apars, constant_slots, constant_vars, novariables, hl_conds );
	}

	void createDoStepsConditions( unsigned procedures, unsigned lines, unsigned no_slots, StringVec& constant_slots, StringVec& constant_vars, unsigned novariables, StringQVec& hl_conds ){
		UIntVec apars( 20, 0 ); // ( current_pos, p, k, *i, s, v1, v2, ... )
		for( int eff : effs ){	
			apars[ 3 ] = eff; 
			for ( apars[ 1 ] = 0; apars[ 1 ] < procedures + 1; ++apars[ 1 ] ){
				for ( apars[ 2 ] = 0; apars[ 2 ] < lines; ++apars[ 2 ] ){		
					for ( apars[ 4 ] = 1; apars[ 4 ] < no_slots + 1; ++apars[ 4 ] ){
						recursive_step_conditions( 5, apars, constant_slots, constant_vars, novariables, hl_conds );
					}
				}
			}
		}
	}

	void createProgrammingActions( unsigned procedures, unsigned lines, unsigned no_slots, const StringVec &empty_slots, const StringQVec &hl_conds, const StringVec &constant_slots ){
		for ( int eff : effs ){
			for ( unsigned j = 0; j < lines; ++j ){
				for ( unsigned s = 1; s < no_slots + 1; ++s ) {
					// Program cond
					SStream ss;
					ss << "PROGRAM-COND-" << od->preds[ eff ]->name << "-" << j << "-" << s;
					String name = ss.str();

					StringVec parameters = od->typeList( od->preds[ eff ] );
					for ( unsigned k = 1; k < parameters.size(); ++k )
						parameters[ k ] = "CONDITION-VARIABLE";
					parameters.push_back( "STACKROW" );

					createAction( name, parameters );

					addPrecondition( name, empty_slots[ j ], false, IntVec( 1, cd->constantIndex( constant_slots[ s ], "CONDITION-SLOT" ) ) );
					addPrecondition( name, "CURRENT-SLOT", false, IntVec( 1, cd->constantIndex( constant_slots[ s - 1 ], "CONDITION-SLOT" ) ) );
					addPrecondition( name, stack_top_pred, false, IntVec( 1, int( parameters.size() ) - 1 ) );
					addPrecondition( name, stack_procedures[ procedures ], false, IntVec( 1, int( parameters.size() ) - 1 )  );
					addPrecondition( name, stack_lines[ j ], false, IntVec( 1, int( parameters.size() ) - 1 )  );

					// Add arity (possible bug)
					if( name.find( "ARY-" ) != String::npos ){
						SStream ordinal;
						ordinal << "ORDINAL-" << parameters.size() - 2 << "ARY";
						addPrecondition( name, ordinal.str(), false, IntVec( 1, 0 ) );
					}

					addEffect( name, empty_slots[ j ], true, IntVec( 1, cd->constantIndex( constant_slots[ s ], "CONDITION-SLOT" ) ) );

					addEffect( name, hl_conds[ eff ][ procedures ][ j ][ s ], false, incvec( 0, int( parameters.size() ) - 1 ) );

					addCost( name, 1001 );

					addActionToInstruction( name, hl_conds[ eff ][ procedures ][ j ][ s ] );

				}
			}
		}
	}

	void evalHighLevelActions( unsigned procedures, unsigned lines, StringVec& constant_slots, unsigned no_slots, unsigned no_variables ){
		for ( unsigned p = 0; p < procedures + 1; p++ ){
			for ( unsigned k = 0; k < lines; ++k ) {
				// Evaluate true condition
				SStream ss;
				ss << "EVAL-COND-" << p << "-" << k;
				String name = ss.str();
				Action * evalcond = createAction( name , StringVec( 1, "STACKROW" ) );
			
				addPrecondition( name, stack_top_pred, false, IntVec( 1, 0) );
				addPrecondition( name, stack_procedures[ p ], false, IntVec( 1, 0 ) );
				addPrecondition( name, stack_lines[ k ], false, IntVec( 1, 0 ) );
				addPrecondition( name, "CURRENT-SLOT", false, IntVec( 1, cd->constantIndex( constant_slots[ no_slots ], "CONDITION-SLOT" ) ) );

				addEffect( name, eval_pred );

				Forall * f = new Forall;
				f->params.insert( f->params.end(), no_variables, cd->types.index( "VALUE" ) );
				When * w = new When;
				IntVec ppars = incvec( 1, no_variables  + 1);
				ppars.insert( ppars.begin(), cd->constantIndex( constant_slots[ no_slots ], "CONDITION-SLOT" ) );
				w->pars = cd->ground( "POSSIBLE-VALUE", ppars );
				w->cond = new And;
				( ( And * )w->cond )->add( cd->ground( accu_pred ) );
				( ( And * )w->cond )->add( new Not( cd->ground( "POSSIBLE-VALUE", ppars ) ) );

				f->cond = w;
				( ( And * )evalcond->eff )->add( f );

				// delete everything about slots !!!
				addEffect( name, "CURRENT-SLOT", true, IntVec( 1, cd->constantIndex( constant_slots[ no_slots ], "CONDITION-SLOT" ) ) );
				addEffect( name, "CURRENT-SLOT", false, IntVec( 1, cd->constantIndex( constant_slots[ 0 ], "CONDITION-SLOT" ) ) );

				f = new Forall;
				f->params.insert( f->params.end(), no_variables, cd->types.index( "VALUE" ) );
				f->cond = new And;
				for ( unsigned s = 1; s < no_slots; ++s ) {
					ppars[ 0 ] = cd->constantIndex( constant_slots[ s ], "CONDITION-SLOT" );
					( ( And * )f->cond )->add( new Not( cd->ground( "POSSIBLE-VALUE", ppars ) ) );
				}
				( ( And * )evalcond->eff )->add( f );

				addCost( name, 1 );
			}
		}
	}
};

#endif
