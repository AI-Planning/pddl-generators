#ifndef U_ACT_H
#define U_ACT_H

class UActions : public Actions{
	public:
	UActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}

	void createRepeatActions( unsigned lines, unsigned noclasses, StringVec &constant_programs ){
		String name = "SELECT-PROGRAM";
		StringVec parameters;
		parameters.emplace_back( "PROGRAM" );
		parameters.emplace_back( "STACKROW" );
		Action *act = createAction( name, parameters );

		addPrecondition( name, "TOP-STACK", false, IntVec( 1, 1 ) );
		addPrecondition( name, "STACK-MAIN", false, IntVec( 1, 1 ) );
		addPrecondition( name, "STACK-LINE-0", false, IntVec( 1, 1 ) );
		addPrecondition( name, "INS-SELECT-PROGRAM" );

		addEffect( name, "STACK-LINE-0", true, IntVec( 1, 1 ) );

		unsigned program_lines = lines / noclasses;
		unsigned program_line = 1;
		unsigned current_class = 0;
		while( program_line < lines ){
			int cpIdx = cd->constantIndex( constant_programs[ current_class ], "PROGRAM" );
			When *w = new When;
			w->pars = new And;
			IntVec v;
			v.emplace_back( 0 );
			v.emplace_back( cpIdx );
			( ( And * ) w->pars )->add( cd->ground( "CHOSEN-PROGRAM", v ) );
			w->cond = new And;
			( ( And * ) w->cond )->add( cd->ground( stack_lines[ program_line ] , IntVec( 1, 1 ) ) );
			( ( And * ) act->eff )->add( w );

			program_line += program_lines;
			current_class++;
		}

		addActionToInstruction( name, "INS-SELECT-PROGRAM" );
	}
};

#endif
