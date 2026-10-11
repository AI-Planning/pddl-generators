#include "include.h"

class Types{
	public:

	Types( ){ }

	/****
	** Copy types from the original domain to the compiled domain
	****/
	void copyTypes( Domain *od, Domain *cd ){ 
		cd->setTypes( od->copyTypes() );
	}

	void createType( Domain *cd , const String& type_name, const String& parent_type_name = "OBJECT" ){
		cd->createType( type_name, parent_type_name );
	}

	void moveObjectsToConstants( Domain *d ){
		/*for ( unsigned i = 0; i < d->types.size(); i++ ) {
			Type *t = d->types[ i ];
			for( unsigned j = 0; j < t->objects.size(); j++ ){
				t->constants.insert( t->objects[ j ] );
			}
		}*/
		// Avoid duplicate objects in the universal pddl parser (workaround in the compiler)
		for( unsigned i = 0; i < d->types.size(); i++ ){
			StringSet scons;
			StringSet sobj;

			for ( unsigned j = 0; j < d->types[ i ]->constants.size(); ++j){
				scons.insert( d->types[ i ]->constants[ j ] ) ;
			}

			for ( unsigned j = 0; j < d->types[ i ]->objects.size(); ++j ){
				String obj = d->types[ i ]->objects[ j ];
				if ( scons.find( obj ) == scons.end() ) 
					sobj.insert( obj );
			}

			//Insert
			for( StringSet::iterator it = sobj.begin(); it != sobj.end(); it++){
				d->types[ i ]->constants.insert( *it );
			}
		}
	}
	
	void clearObjects( Domain *d ){
		for( unsigned i = 0; i < d->types.size(); ++i ){
			Type *t = d->types[ i ];
			t->objects.tokens.clear();
			t->objects.tokenMap.clear();
			t->objects.types.clear();
		}
	}

	void addValuedConstants( Domain *d, const String &type_name, const String &constant_name, 
							 StringVec& constant_names, unsigned bound, unsigned offset = 0){
		Type *t = d->types.get( type_name );
		for( unsigned i = offset; i < bound; i++ ){
			SStream ss;
			ss << constant_name << "-" << i;
			t->constants.insert( ss.str() );
			constant_names.emplace_back( ss.str() );
		}
	}
};
