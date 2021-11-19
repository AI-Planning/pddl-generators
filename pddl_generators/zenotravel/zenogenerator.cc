#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <stdlib.h>

#include <iterator>     // std::ostream_iterator

enum domaintype {STRIPS,NUMERIC,SIMPLE,TIME,COMPLEX};
enum typingStatus {ON,OFF};

domaintype domainType;
typingStatus typing;

using namespace std;

int rnd(int limit) {
	return (int) ((((double)((long int)limit)*random()))/(RAND_MAX+1.0));
};

double rnd() {
	return ((double) random())/(RAND_MAX+1.0);
};

class Map {
private:
	int locations;
	int distance;

	vector<vector<int> > path;

public:
	Map(int s,int d) : locations(s), distance(d), path(s,vector<int>(s,0))
	{
		for(int i = 0;i < s;++i)
		{
			for(int j = i+1;j < s;++j)
			{
				path[i][j] = rnd(distance/2) + distance/2;
				path[j][i] = path[i][j];
			};
		};
	};

	void write(ostream & o) const
	{
	    for(int i = 0;i < locations;++i)
	    {
		if(typing==OFF)
			o << "\t(city city" << i << ")\n";
		if(domainType!=SIMPLE && domainType!=STRIPS)
		{
		  for(int j = 0;j < locations;++j)
		    {
			
		      o << "\t(= (distance city" << i << " city" << j << ") "
					<< path[i][j] << ")\n";
		    };
		};
	    };
	};

	void objects(ostream & o) const
	{
		for(int i = 0;i < locations;++i)
		{
			o << "\tcity" << i;
			if(typing==ON) o << " - city";
			o << "\n";
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

#define REPORT(x,y) o << "\t(= (" << x << " plane" << id << ") " << y << ")\n";

class plane : public locatable {
private:
	static int planes;
	int id;

	int slowburn, slowspeed, fuel, capacity, fastspeed, fastburn,
		refuelrate, zoomlimit;
public:
	plane(int locs, int distance) : locatable(locs), id(++planes),
		slowburn(1+rnd(5)),
		slowspeed(rnd(100)+100), fuel(rnd(slowburn*distance)),
		capacity((int)((2.1+rnd())*slowburn*distance)),
		fastspeed((int)((1.0+rnd()*2.0)*slowspeed)),
		fastburn((int)((2.0+rnd()*2.0)*slowburn)),
		refuelrate(2*(1+rnd(slowburn*distance))),
		zoomlimit(1+rnd(10))
		
	{
		if(rnd(10) < 7) uninteresting();
		
	};

	void write(ostream & o) const
	{
		o << "\t(at plane" << id << " city" << location << ")\n";
		if(typing==OFF)
			o << "\t(aircraft plane" << id << ")\n";
		switch(domainType)
		{
			case NUMERIC:
				REPORT("capacity",capacity)
				REPORT("fuel",fuel)
				REPORT("slow-burn",slowburn)
				REPORT("fast-burn",fastburn)
				REPORT("onboard",0)
				REPORT("zoom-limit",zoomlimit)
				break;
			case TIME:
				REPORT("slow-speed",slowspeed)
				REPORT("fast-speed",fastspeed)
			case STRIPS:
			case SIMPLE:
				o << "\t(fuel-level plane" << id << " fl" <<
					(fuel % 7) << ")\n";
			
				break;
			case COMPLEX:
				REPORT("slow-speed",slowspeed)
				REPORT("fast-speed",fastspeed)
				REPORT("capacity",capacity)
				REPORT("fuel",fuel)
				REPORT("slow-burn",slowburn)
				REPORT("fast-burn",fastburn)
				REPORT("refuel-rate",refuelrate)
				
			default:
				break;
		};
	};

	static void objects(ostream & o)
	{
		for(int i = 1;i <= planes;++i)
		{
			o << "\tplane" << i;
			if(typing==ON) o << " - aircraft";
			o << "\n";
		};
	};

	void goal(ostream & o) const
	{
		if(interesting) o << "\t(at plane" << id << " city" << destination << ")\n";
	};
};

int plane::planes = 0;

class person : public locatable {
private:
	static int people;
	int id;
public:
	person(int locs) : locatable(locs), id(++people)
	{
		if(rnd(100) < 3) uninteresting();
	};

	void write(ostream & o) const
	{
		o << "\t(at person" << id << " city" << location 
			<< ")\n";
		if(typing==OFF)
		{
			o << "\t(person person" << id << ")\n";
		};
	};

	static void objects(ostream & o)
	{
		for(int i = 1;i <= people;++i)
		{
			o << "\tperson" << i;
			if(typing==ON) o << " - person";
			o << "\n";
		};
	};

	void goal(ostream & o) const
	{
		if(interesting) o << "\t(at person" << id << " city" << destination << ")\n";
	};
};

int person::people = 0;



class Problem {
private:
	int npeople;
	int nplanes;

	Map m;
	
	vector<plane> planes;
	vector<person> people;

public:
	Problem(int s,int pl,int pe,int dist = 0) : 
	npeople(pe), nplanes(pl), m(s,dist)
	{
		for(int i = 0; i < nplanes;++i)
		{
			planes.push_back(plane(s,dist));
		};
		for(int i = 0; i < npeople;++i)
		{
			people.push_back(person(s));
		};
	};

	void write(ostream & o) const
	{
		o << "(define (problem ZTRAVEL-" << nplanes << "-" << npeople 
			<< ")\n(:domain zeno-travel)\n(:objects\n";
		plane::objects(o);
		person::objects(o);
		m.objects(o);
		if(domainType==SIMPLE || domainType==STRIPS)
		{
			for(int i = 0;i < 7;++i)
			{
			  o << "\tfl" << i;
			  if(typing==ON) o << " - flevel";
			  o << "\n";
			};
		};
		o << "\t)\n(:init\n";
		copy(planes.begin(),planes.end(),ostream_iterator<plane>(o,""));
		copy(people.begin(),people.end(),ostream_iterator<person>(o,""));
		o << m;
		if(domainType==SIMPLE || domainType==STRIPS)
		{
			for(int i = 0;i < 6;++i)
			{
				o << "\t(next fl" << i << " fl" << i+1 << ")\n";
			};

			if(typing==OFF)
			{
			  for(int i = 0;i < 7;++i)
			    {
			      o << "\t(flevel fl" << i << ")\n";
			    };
			};
		};
		if(domainType==NUMERIC || domainType==COMPLEX)
		{
			o << "\t(= (total-fuel-used) 0)\n";
		};
		if(domainType==COMPLEX || domainType==TIME)
		{
			o << "\t(= (boarding-time) 0.3)\n\t(= (debarking-time) 0.6)\n";
		};
		o << ")\n(:goal (and\n";
		
		for(vector<plane>::const_iterator i = planes.begin(); i != planes.end();++i)
		{
			i->goal(o);
		};
		for(vector<person>::const_iterator i = people.begin(); i != people.end();
										++i)
		{
			i->goal(o);
		};
		o << "\t))\n\n";
		if(domainType==NUMERIC || domainType==COMPLEX) 
		{
		  int w1 = 1+rnd(5);
		  int w2 = 1+rnd(5);
		  o << "(:metric minimize (+ (* " << w2 << " (total-time)) "
			 << " (* ";
			if(domainType==COMPLEX) 
			{
			  o << ((double) w1)/1000.0;
			}
		        else
			{
			  o << w1;
			};
		        o << " (total-fuel-used))))\n";
		};
		if(domainType==SIMPLE || domainType==TIME) 
		{
			o << "(:metric minimize (total-time))\n";
		};
		o << ")\n";
	};
};

ostream & operator<<(ostream & o,const Problem & p)
{
	p.write(o);
	return o;
};



int main(int argc,char * argv[])
{
	if(argc < 4)
	{
		cout << "Usage: ztravel [-n|-s|-t|-c|-u] <seed> <#cities> <#planes> <#people> [distance]\n"
			<< "\t-n: Numeric\n\t-s: Simple time\n\t-t: Time\n\t-c: Complex\n\t-u: Untyped\n\n"
			<< "\tIf distance is a positive value then the distances between\n\tlocations is set randomly using this value as the bound.\n\n";
		exit(0);
	};
	typing = ON;
	domainType = STRIPS;
	while(argv[1][0] == '-')
	{
		switch(argv[1][1])
		{
			case 'n':
				domainType = NUMERIC;
				break;
			case 's':
				domainType = SIMPLE;
				break;
			case 't':
				domainType = TIME;
				break;
			case 'c':
				domainType = COMPLEX;
				break;
			case 'u':
				typing = OFF;
				break;
			default:
				domainType = STRIPS;
		};
		--argc;
		++argv;
	};
	srandom(atoi(argv[1]));
	Problem p(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),argc>=6?atoi(argv[5]):0);
	cout << p;
	return 0;
};



