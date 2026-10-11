#ifndef CFG_ACT_H
#define CFG_ACT_H

class CFGActions : public Actions{
	public:
	CFGActions( Domain *_od, Domain *_cd ): Actions( _od, _cd ){}

	void createRepeatActions( unsigned procedures, unsigned lines, const StringDVec& allowed_lines, const StringVec& select_programs ){
		for( unsigned procedure = 0; procedure < procedures; procedure++ ){
			for( unsigned line = 1; line <= lines; line++ ){
				SStream ss;
				ss << "SELECT-PROGRAM-" << procedure << "-" << line;
			
				String name = ss.str();
				createAction( name, StringVec( 1, "STACKROW" ) );

				addPrecondition( name, "TOP-STACK", false, IntVec( 1, 0 ) );
				if( procedure + 1 == procedures )
					addPrecondition( name, "STACK-MAIN", false, IntVec( 1, 0 ) );
				else
					addPrecondition( name, stack_procedures[ procedure ], false, IntVec( 1, 0 ) );
				
				addPrecondition( name, "STACK-LINE-0", false, IntVec( 1, 0 ) );
				//addPrecondition( name, "INS-SELECT-PROGRAM" );
				addPrecondition( name, select_programs[ procedure ] );
				addPrecondition( name, allowed_lines[ procedure ][ line ] );			

				addEffect( name, "STACK-LINE-0", true, IntVec( 1, 0 ) );
				addEffect( name, stack_lines[ line ], false, IntVec( 1, 0 ) );
		
				//addActionToInstruction( name, "INS-SELECT-PROGRAM" );
				//addActionToInstruction( name, select_programs[ procedure ] );
			}		
		}
	}
};

#endif
