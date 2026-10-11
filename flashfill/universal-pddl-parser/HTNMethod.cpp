 
#include "Domain.h"

void HTNMethod::SHOPPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, Domain & d ) {
	s << "( :METHOD (" << name << " ";

    //params
	TokenStruct< std::string > astruct;
	std::stringstream ss;
	

	printParams( 0, s, astruct, d );

	s<<")\n";
	//preconds
	if ( pre ) pre->PDDLPrint( s, 1, astruct, d );
	else s << "\t()";
	s << "\n";

	//tasks
	if ( tasks.size() > 0 ){ 
		s << "\t( ";
		for(unsigned i=0;i<tasks.size();++i)
			tasks[i]->SHOPPrint( s, 1, astruct, d );
		s << " )";
	}
	else s << "\t()";
	s << "\n";
	s << ")\n";
}

void HTNMethod::SHOPparse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	TokenStruct< std::string > astruct = f.parseTypedList( false );
	params = d.convertTypes( astruct.types );
	parseSHOPConditions( f, astruct, d );
	parseTaskList( f, astruct, d );

}

void HTNMethod::parseSHOPConditions( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert( "(" );
	if ( f.getChar() != ')' ) {
			if(f.getChar() == '('){
				pre = new And;
			}
			else 
				pre = createSHOPCondition( f, d );
			
			pre->SHOPparse( f, ts, d );

	}
	else ++f.c;
}

void HTNMethod::parseTaskList( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
f.next();
f.assert( "(" );
for ( f.next(); f.getChar() != ')'; f.next() ) {
		f.assert( "(" );
		std::string s = f.getToken();

		Task * task = new Task(s);
		task->SHOPparse( f, ts, d );
		tasks.push_back( task );
		d.tasks.insert( task );
	}
	++f.c;
f.next();
f.assert( ")" );
}