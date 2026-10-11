#include "include.h"

class DomainCreator{
	public:
	DomainCreator(){}
	static Domain* create( String& name, bool condeffects = true, bool typed = true, bool cons = true, bool costs = true){
		Domain* nd = new Domain;
		nd->name = name;
		nd->condeffects = condeffects;
		nd->typed = typed;
		nd->cons = cons;
		nd->costs = costs;
		return nd;
	}
};
