#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <stdlib.h>
#include <iterator>     // std::ostream_iterator

enum typeStatus {ON,OFF};
typeStatus typing;

enum problemTypes {STRIPS,SIMPLETIME,NUMERIC,HARDNUMERIC,TIMED};
problemTypes probType;


using namespace std;

int rnd(int limit) {
	return (int) ((((double)((long int)limit)*random()))/(RAND_MAX+1.0));
};

typedef map<int,set<int> > graph;
class Map {
private:
	int services;
	graph path;
	graph road;
	int distance;


	void explore(graph & g,int start,set<int> & reached)
	{
		queue<int> togo;
		
		togo.push(start);
		reached.insert(start);
		while(!togo.empty())
		{
			int loc = togo.front();
			togo.pop();
			
			for(set<int>::const_iterator i = g[loc].begin();i != g[loc].end();++i)
			{
				if(find(reached.begin(),reached.end(),*i)==reached.end())
				{
					togo.push(*i);
					reached.insert(*i);
				};
			};
		};
	};

	void connect(graph & g) 
	{
		set<int> reached;
		int start = rnd(services);
		explore(g,start,reached);
		while(reached.size()!=services)
		{
			int next;
			for(int i = 0;i < services;++i)
			{
				if(find(reached.begin(),reached.end(),i)==reached.end())
				{
					next = i;
					break;
				};
			};
			g[start].insert(next);
			start = next;
			explore(g,start,reached);
		};
		
	};

public:
	Map(int s,int d) : services(s), distance(d)
	{
		for(int i = 0;i < s;++i)
		{
			for(int j = 0;j < 2;++j)
			{
				int f = rnd(s);
				int t = rnd(s);
				if(path[t].find(f)==path[t].end())
					path[f].insert(t);
			};
			for(int j = 0;j < 4;++j)
			{
				int f = rnd(s);
				int t = rnd(s);
				if(road[t].find(f)==road[t].end())
					road[f].insert(t);
			};
		};
		connect(path);
		connect(road);
	};

	void write(ostream & o) const
	{
		if(typing==OFF)
		{
			for(int i = 0;i < services;++i)
			{
				o << "\t(LOCATION s" << i << ")\n";
			};
		
			for(graph::const_iterator i = path.begin();i != path.end();++i)
			{
				for(set<int>::const_iterator j = i->second.begin();
							j != i->second.end();++j)
				{
					if(i->first == *j) continue;
				
					o << "\t(LOCATION p" << 
					i->first << "-" << *j << ")\n";
				};
			};
		};
		for(graph::const_iterator i = path.begin();i != path.end();++i)
		{
			for(set<int>::const_iterator j = i->second.begin();
							j != i->second.end();++j)
			{
				if(i->first == *j) continue;
				if(path.find(*j)->second.find(i->first) 
				     != path.find(*j)->second.end()
				   && *j < i->first) continue;
				
				o << "\t(path s" << i->first << " p" << 
				i->first << "-" << *j << ")\n\t(path p" <<
				i->first << "-" << *j << " s" << i->first 
				<< ")\n\t(path s" << *j << " p" << i->first 
				<< "-" << *j << ")\n\t(path p" <<
				i->first << "-" << *j << " s" << *j << ")\n";

				if(!distance) continue;

				int d1 = rnd(distance)+1;
				int d2 = rnd(distance)+1;

				o << "\t(= (time-to-walk s"
				<< i->first << " p" << 
				i->first << "-" << *j << ") " << d1 << ")\n\t(= (time-to-walk p"
				<< i->first << "-" << *j << " s" << i->first << ") " << d1 
				<< ")\n\t(= (time-to-walk s" << *j << " p" << 
				i->first << "-" << *j << ") " << d2 << ")\n\t(= (time-to-walk p"
				<< i->first << "-" << *j << " s" << *j << ") " << d2 << ")\n";
			};
		};
		for(graph::const_iterator i = road.begin();i != road.end();++i)
		{
			for(set<int>::const_iterator j = i->second.begin();
							j != i->second.end();++j)
			{
				if(i->first == *j) continue;
				if(road.find(*j)->second.find(i->first) 
				     != road.find(*j)->second.end()
				   && *j < i->first) continue;

				o << "\t(link s" << i->first << " s" << *j << ")\n\t(link s" <<
					*j << " s" << i->first 
					<< ")\n";

				if(!distance) continue;

				int d1 = rnd(distance)+1;
				
				o << "\t(= (time-to-drive s"
				<< i->first << " s" << *j << ") " << d1 
				<< ")\n\t(= (time-to-drive s" << *j << " s" << i->first 
				<< ") " << d1 
				<< ")\n";
			};
		};
	};

	void objects(ostream & o) const
	{
		for(int i = 0;i < services;++i)
		{
			o << "\ts" << i;
			if(typing==ON)
				o << " - location";
			o << "\n";
		};
		for(graph::const_iterator i = path.begin();i != path.end();++i)
		{
			for(set<int>::const_iterator j = i->second.begin();
							j != i->second.end();++j)
			{
				if(i->first == *j) continue;
				
				o << "\tp" << 
				i->first << "-" << *j;
				if(typing==ON)
					o << " - location";
				o << "\n";
			};
		};
	};

};

ostream & operator <<(ostream & o,const Map & m)
{
	m.write(o);
	return o;
};

class locatable {
protected:
	int location;
	int destination;

	bool interesting;
public:
	locatable(int locs) : location(rnd(locs)), destination(rnd(locs)), interesting(true) {};

	void uninteresting() {interesting = false;};
	bool isInteresting() const {return interesting;};
	virtual void write(ostream&o) const = 0;
};

ostream & operator << (ostream & o,const locatable & l)
{
	l.write(o);
	return o;
};

class truck : public locatable {
private:
	static int trucks;
	int id;
public:
	truck(int locs) : locatable(locs), id(++trucks)
	{
		if(rnd(10) < 3) uninteresting();
	};

	void write(ostream & o) const
	{
		o << "\t(at truck" << id << " s" << location << ")\n\t(empty truck" 
			<< id << ")\n";
		if(typing==OFF)
			o << "\t(TRUCK truck" << id << ")\n";
		if(probType==HARDNUMERIC)
		{
			o << "\t(= (load truck" << id << 
				") 0)\n\t(= (fuel-per-minute truck" << id << 
					") 10)\n";
		};
	};

	static void objects(ostream & o)
	{
		for(int i = 1;i <= trucks;++i)
		{
			o << "\ttruck" << i;
			if(typing==ON)
				o << " - truck";
			o << "\n";
		};
	};

	void goal(ostream & o) const
	{
		if(interesting) o << "\t(at truck" << id << " s" << destination << ")\n";
	};
};

int truck::trucks = 0;

class package : public locatable {
private:
	static int packages;
	int id;
public:
	package(int locs) : locatable(locs), id(++packages)
	{
		if(rnd(100) < 5) uninteresting();
	};

	void write(ostream & o) const
	{
		o << "\t(at package" << id << " s" << location 
			<< ")\n";
		if(typing==OFF)
			o << "\t(OBJ package" << id << ")\n";
	};

	static void objects(ostream & o)
	{
		for(int i = 1;i <= packages;++i)
		{
			o << "\tpackage" << i;
			if(typing==ON)
				o << " - obj";
			o << "\n";
		};
	};

	void goal(ostream & o) const
	{
		if(interesting) o << "\t(at package" << id << " s" << destination << ")\n";
	};
};

int package::packages = 0;

class driver : public locatable {
private:
	static int drivers;
	int id;
public:
	driver(int locs) : locatable(locs), id(++drivers)
	{
		if(rnd(10) < 3) uninteresting();
	};

	void write(ostream & o) const
	{
		o << "\t(at driver" << id << " s" << location << ")\n";
		if(typing==OFF)
			o  << "\t(DRIVER driver" << id << ")\n";
	};

	static void objects(ostream & o)
	{
		for(int i = 1;i <= drivers;++i)
		{
			o << "\tdriver" << i;
			if(typing==ON)
				o << " - driver";
			o << "\n";
		};
	};

	void goal(ostream & o) const
	{
		if(interesting) o << "\t(at driver" << id << " s" << destination << ")\n";
	};
};

int driver::drivers = 0;

class Problem {
private:
	int ndrivers;
	int npackages;
	int ntrucks;

	Map m;
	vector<driver> drivers;
	vector<truck> trucks;
	vector<package> packages;

public:
	Problem(int s,int d,int p,int t,int dist = 0) : 
	ndrivers(d), npackages(p), ntrucks(t), m(s,dist)
	{
		for(int i = 0; i < ndrivers;++i)
		{
			drivers.push_back(driver(s));
		};
		for(int i = 0; i < ntrucks;++i)
		{
			trucks.push_back(truck(s));
		};
		for(int i = 0; i < npackages;++i)
		{
			packages.push_back(package(s));
		};
	};

	void write(ostream & o) const
	{
		o << "(define (problem DLOG-" << ndrivers << "-" << ntrucks << "-" << npackages
			<< ")\n\t(:domain driverlog)\n\t(:objects\n";
		driver::objects(o);
		truck::objects(o);
		package::objects(o);
		m.objects(o);
		o << "\t)\n\t(:init\n";
		copy(drivers.begin(),drivers.end(),ostream_iterator<driver>(o,""));
		copy(trucks.begin(),trucks.end(),ostream_iterator<truck>(o,""));
		copy(packages.begin(),packages.end(),ostream_iterator<package>(o,""));
		o << m;
		if(probType==NUMERIC)
		{
			o << "\t(= (driven) 0)\n\t(= (walked) 0)\n";
		};
		if(probType==HARDNUMERIC)
		{
			o << "\t(= (fuel-used) 0)\n";
		};
		o << ")\n\t(:goal (and\n";
		for(vector<driver>::const_iterator i = drivers.begin(); i != drivers.end();++i)
		{
			i->goal(o);
		};
		for(vector<truck>::const_iterator i = trucks.begin(); i != trucks.end();++i)
		{
			i->goal(o);
		};
		for(vector<package>::const_iterator i = packages.begin(); i != packages.end();
										++i)
		{
			i->goal(o);
		};
		o << "\t))\n\n";
		if(probType==SIMPLETIME || probType==TIMED)
			o << "(:metric minimize (total-time))\n";
		if(probType==NUMERIC)
		{
			o << "(:metric minimize (+ (+ (* " << rnd(4)+1 <<
				" " << " (total-time)) (* " << rnd(4)+1 <<
				" " << " (driven))) (* " << rnd(4)+1 <<
				" " << " (walked))))\n";
		};
		if(probType==HARDNUMERIC)
		{
			o << "(:metric minimize (+ (* " << rnd(4)+1 <<
				" (total-time)) (* " << rnd(4)+1 <<
					" (fuel-used))))\n";
		};
	
		o << "\n)\n";
	};
};

ostream & operator<<(ostream & o,const Problem & p)
{
	p.write(o);
	return o;
};



int main(int argc,char * argv[])
{
	typing = ON;
	probType = STRIPS;
	if(argc < 6)
	{
		cout << "Usage: dlog [-u|-s|-t|-n|-h] <seed> <#road junctions> <#drivers> <#packages> <#trucks> [distance]\n"
			<< "\tIf distance is a positive value then the distances between\n\tlocations is set randomly using this value as the bound.\n\n\t-n: Numeric\n\t-u: Untyped\n\t-t: Timed\n\t-s: Simple time\n\t-h: Hard numeric\n\n";
		exit(0);
	};
	while(argv[1][0]=='-')
	{
		switch(argv[1][1])
		{
			case 'u':
				typing = OFF;
				break;
			case 't':
				probType = TIMED;
				break;
			case 's':
				probType = SIMPLETIME;
				break;
			case 'n':
				probType = NUMERIC;
				break;
			case 'h':
				probType = HARDNUMERIC;
				break;
			default:
				break;
		};
		++argv;
		--argc;
	};
	srandom(atoi(argv[1]));
	Problem p(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),argc==7?atoi(argv[6]):0);
	cout << p;
	return 0;
};

			
