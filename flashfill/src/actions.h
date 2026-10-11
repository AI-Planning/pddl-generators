#ifndef ACTIONS_H
#define ACTIONS_H

#include "include.h"

class Actions{
	protected:
	MapStrSVec _action_names;	// Action prefixes to actions
	Domain *od; 				// Original Domain
	Domain *cd; 				// Compiled Domain

	public:
	Actions() {}
	Actions( Domain *_od, Domain *_cd ): od( _od ), cd(_cd){ }

	Action* getAction( Domain *d, const String &name ){
		return d->actions.get( name );
	}

	Action* createAction( const String &name, const StringVec& parameters = StringVec() ){		
		Action *act = cd->createAction( name, parameters );	
		_action_names[ name ].emplace_back( name );
		return act;
	}

	void createValuedActions( 	const String &name_prefix, unsigned bound1, unsigned offset1 = 0, const StringVec& parameters = StringVec() ){		
		for( unsigned i = offset1; i < bound1; i++ ){
			SStream ss;
			ss << name_prefix << "-" << i;
			String name = ss.str();
			createAction( name, parameters );
			_action_names[ name_prefix ].emplace_back( name );
		}
	}

	void createDValuedActions(	const String &name_prefix, 
								unsigned bound1, unsigned bound2, 
								unsigned offset1 = 0, unsigned offset2 = 0, 
								const StringVec &parameters  = StringVec() ){
		for( unsigned i = offset1; i < bound1; i++ ){
			SStream ss;
			ss << name_prefix << "-" << i;
			String name = ss.str();
			createValuedActions( name, bound2, offset2, parameters );
			_action_names[ name_prefix ].emplace_back( name );
		}	
	}


	void createTValuedActions(	const String &name_prefix, unsigned bound1, unsigned bound2, unsigned bound3,
								unsigned offset1 = 0, unsigned offset2 = 0, unsigned offset3 = 0,
								const StringVec &parameters  = StringVec() ){
		for( unsigned i = offset1; i < bound1; i++ ){
			SStream ss;
			ss << name_prefix << "-" << i;
			String name = ss.str();
			createDValuedActions( name, bound2, bound3, offset2, offset3, parameters );
			_action_names[ name_prefix ].emplace_back( name );
		}	
	}

	StringVec getNames( const String& name ){
		MapStrSVec::iterator it = _action_names.find( name );
		if( it == _action_names.end() )
			return StringVec();
		return (it->second);
	}

	void addPrecondition( 	const String &action_name, const String &predicate_name, 
							bool negative_precondition = false, const IntVec& parameters = IntVec() ){
		cd->addPre( negative_precondition, action_name, predicate_name, parameters );
	}
	
	void addEffect(	const String &action_name, const String &predicate_name, 
					bool negative_precondition = false, const IntVec& parameters = IntVec() ){
		cd->addEff( negative_precondition, action_name, predicate_name, parameters );
	}
	
	void addCost( const String &name, int cost ){
		cd->addCost( name, cost );
	}

	void copyPreconditions( const String &oname, const String &cname ){
		Action *act = od->actions.get( oname );
		cd->setPre( cname, act->pre );
	}

	void copyEffects( const String &oname, const String &cname ){
		Action *act = od->actions.get( oname );
		cd->setEff( cname, act->eff );
	}

	void addParameters( const String& name, unsigned offset, unsigned extra_size , bool is_precondition = true ){
		Action *act = cd->actions.get( name );
		if( is_precondition )
			act->pre->addParams( offset, extra_size );
		else
			act->eff->addParams( offset, extra_size );
	}

	void addActionToInstruction( const String& action_name, const String& instruction ){
		// Action to instruction
		act2ins[ action_name ].emplace_back( instruction );
	}
};

#endif
