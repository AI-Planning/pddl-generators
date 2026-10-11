#ifndef DERIVED_H
#define DERIVED_H

#include "actions.h"

class DerivedActions : public Actions{
	public:
	DerivedActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}

	void createEvalActions( const String &name ){
		//String name = "DERIVED-EVALUATION";
		Action *act = createAction( name , StringVec( 1, "STACKROW" ) );
		addPrecondition( name, "TODO-DERIVED" );
		addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) );

		addEffect( name, "DONE-DERIVED" );
		addEffect( name, "TODO-DERIVED", true );

		// Derived predicates as conditional effects of derived evaluation
		for ( unsigned derived_id = 0; derived_id < od->derived.size(); derived_id++ ){
			//if( d->derived[ derived_id ]->name != prefix_name ) continue;
			String derived_name = od->derived[ derived_id ]->name;
			unsigned dsize = od->derived[ derived_id ]->params.size();

			if( dsize == 0 ){
				( ( And * )act->eff )->add( new Not( cd->ground( derived_name, IntVec() ) ) );
			}
			else{
				Forall *f = new Forall;
				f->params = od->derived[ derived_id ]->params;
				//f->addParams( 0, 1 );
				f->cond = new And;
				( ( And * )f->cond )->add( new Not( cd->ground( derived_name, incvec( 1 , dsize + 1 ) ) ) );
				( ( And * ) act->eff)->add( f );
			}

						
			Exists * exists = dynamic_cast< Exists * >( od->derived[ derived_id ]->cond );
			//Forall * forall = dynamic_cast< Forall * >( od->derived[ derived_id ]->cond );

			if ( exists ) { // Originally is an Exists
				When *w = new When;
				w->pars = exists->cond->copy( *cd );
				w->cond = new And;
				( ( And *) w->cond)->add( cd->ground( derived_name, incvec( 0 , dsize ) ) );
				//w->addParams( dsize, 1 );
				w->addParams( 0, 1 );

				Forall * f = new Forall;
				//f->params = exists->params;
				for( int i : od->derived[ derived_id ]->params )
					f->params.push_back( i );
				for( int i : exists->params )
					f->params.push_back( i );
				f->cond = w;

				addStackRow( f, 0 );

				( ( And * )act->eff )->add( f );
			}
			/*else if( forall ){ // Originally is a Forall (Not tested)
				When *w = new When;
				//w->pars = forall->cond->copy( *cd );
				w->pars = forall->cond;
				w->cond = new And;
				( ( And *) w->cond)->add( cd->ground( derived_name, incvec( 0 , dsize ) ) );

				Forall * f = new Forall;
				f->params = od->derived[ derived_id ]->params;
				for( int i : forall->params )
					f->params.push_back( i );
				f->cond = w;
				f->addParams( 0, 1 );

				addStackRow( f, 0 );

				( ( And * )act->eff )->add( f );
			
			}*/
			else { // Originally is a When
				When *w = new When;
				w->pars = od->derived[ derived_id ]->cond->copy( *cd );
				w->cond = new And;
				( ( And *) w->cond)->add( cd->ground( derived_name, incvec( 0 , dsize ) ) );
				w->addParams( 0, 1 );

				addStackRow( w, 0 );

				( ( And * )act->eff )->add( w );
			}
		}
	}
	
};

#endif
