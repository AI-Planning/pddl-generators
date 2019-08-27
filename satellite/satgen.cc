using namespace std;

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <functional>
#include <iterator>
#include <fstream>

enum probTypes {STRIPS,NUMERIC,SIMPLETIME,TIMED,COMPLEX};
probTypes probType;

enum typeStatus {ON,OFF};
typeStatus typing;

enum metricSort {HARD,EASY};
metricSort metricType;

double tightness;

int rnd(int limit) {
	return (int) ((((double)((long int)limit)* random()))/(RAND_MAX+1.0));
};

double rnd() {
	return ((double) random())/(RAND_MAX+1.0);
};

class ProblemObject {
private:
	enum OutStatus {OBJECT,INIT,GOAL};
	static OutStatus outStatus;
public:
	virtual void object(ostream & o) const = 0;
	virtual void init(ostream & o) const = 0;
	virtual void goal(ostream & o) const = 0;
	void write(ostream & o) const
	{
		switch(outStatus)
		{
			case OBJECT:
				object(o);
				break;
			case INIT:
				init(o);
				break;
			case GOAL:
				goal(o);
				break;
			default:
				break;
		};
	};

	static void Objects() {outStatus = OBJECT;};
	static void Inits() {outStatus = INIT;};
	static void Goals() {outStatus = GOAL;};
};

ProblemObject::OutStatus ProblemObject::outStatus = OBJECT;

ostream & operator<<(ostream & o,const ProblemObject & p)
{
	p.write(o);
	return o;
};

class direction : public ProblemObject {
protected:
	static int id;
	static vector<direction *> allDirections;

	int myId;

	string nm;
	vector<double> slewTimes;

	virtual direction * clone() const = 0;

public:
	direction() : myId(id++)
	{
		for(int i = 0;i < myId;++i)
		{
			double a = rnd()*100.0;
			double b = rnd()*100.0;

			slewTimes.push_back(abs(a-b));
		};
	};
	string name() const
	{
		return nm;
	};
	void object(ostream & o) const
	{
		o << "\t" << name();
		if(typing==ON)
			o << " - direction";
		o << "\n";
	};
	void init(ostream & o) const
	{
		if(typing==OFF)
			o << "\t(direction " << name() << ")\n";
		if(probType==TIMED || probType==COMPLEX || probType==NUMERIC)
		{
			o.precision(4);
			for(int i = 0;i < myId;++i)
			{
				o << "\t(= (slew_time " << name() << " " <<
					allDirections[i]->name() << ") " << slewTimes[i]
					<< ")\n";
				o << "\t(= (slew_time " << allDirections[i]->name() << " " <<
					name() << ") " << slewTimes[i]
					<< ")\n";
			};
		};
	};
	void goal(ostream & o) const
	{};
};



int direction::id = 0;
vector<direction *> direction::allDirections = vector<direction *>();

class target : public direction {
private:
	static string types[2];
	static int count;
	int type;

	direction * clone() const
	{
		return new target(*this);
	};
public:
	target() : direction(), type(rnd(2))
	{
		++count;
		nm = types[type];
		ostringstream s;
		s << myId;
		nm += s.str();
		allDirections.push_back(this->clone());
	};
	static int howMany() {return count;};
	bool operator==(const target & t) const
	{
		return myId == t.myId;
	};
};

string target::types[2] = {"Star","GroundStation"};
int target::count = 0;

class mode : public ProblemObject {
private:
	static vector<int> supportedModes;

	static string types[4];

	static int id;
	int myID;

	int type;

public:
	mode() : myID(id++), type(rnd(4))
	{
		supportedModes.push_back(false);
	};

	static int howMany()
	{
		return id;
	};
	bool operator==(const mode & m) const
	{
		return myID == m.myID;
	};
	string name() const
	{
		ostringstream s;
		s << types[type] << myID;
		return s.str();
	};
	void supported() {supportedModes[myID] = true;};
	void object(ostream & o) const
	{
		o << "\t" << types[type] << myID;
		if(typing==ON)
			o << " - mode";
		o << "\n";
	};
	void init(ostream & o) const;
	void goal(ostream & o) const
	{};
	bool isSupported() const {return supportedModes[myID];};
};

vector<int> mode::supportedModes = vector<int>();

string mode::types[4] = {"infrared","image","spectrograph","thermograph"};
int mode::id = 0;

class observation : public direction {
private:
	static string types[3];

	int type;
	bool interesting;
	vector<mode> images;
	vector<int> dataSize;

	direction * clone() const
	{
		return new observation(*this);
	};
public:
	static vector<observation *> allObservations;

	observation();
	int getDataSize(int mdnum) const
	{
		return dataSize[mdnum];
	};
	void goal(ostream & o) const
	{
		if(interesting && metricType==EASY)
		{

			for(vector<mode>::const_iterator i = images.begin();
								i != images.end();++i)
			{
				if(i->isSupported())
					o << "\t(have_image " << name() << " " << i->name() << ")\n";
			};
		};
	};
};

string observation::types[3] = {"Star","Phenomenon","Planet"};
vector<observation*> observation::allObservations = vector<observation *>();

void mode::init(ostream & o) const
{
	if(typing==OFF)
		o << "\t(mode " << types[type] << myID << ")\n";
	if(probType==NUMERIC || probType==COMPLEX)
	{
		for(vector<observation*>::const_iterator i =
					observation::allObservations.begin();
			i != observation::allObservations.end();++i)
		 		o << "\t(= (data " << (*i)->name() << " " << types[type] << myID
					<< ") " << (*i)->getDataSize(myID) << ")\n";
	};
};

template<class T>
class selection {
private:
	vector<T> choices;
	int sz;

	struct Id {
		void operator()(T t){};
	};

public:
	selection() : sz(0) {};
	selection(int n) : sz(n)
	{
		for(int i = 0;i < n;++i)
			choices.push_back(T());
	};

	T & selectOne()
	{
		return choices[rnd(sz)];
	};

	int size() const
	{
		return sz;
	};

	template<class U>
	void selectSeveral(vector<T> & ts,int n,U fn)
	{
		int i;
		for(i = 0; sz && (i < n);++i)
		{
			T t = selectOne();
			choices.erase(remove(choices.begin(),choices.end(),t),
							choices.end());
			--sz;
			fn(t);
			ts.push_back(t);

		};
		for(--i; i >= 0;--i)
		{
			choices.push_back(ts[i]);
			++sz;
		};
	};

	void selectSeveral(vector<T> & ts,int n)
	{
		selectSeveral(ts,n,Id());
	};

	void write(ostream & o) const
	{
		copy(choices.begin(),choices.end(),ostream_iterator<T>(o,""));
	};
};

template<class T>
ostream & operator<<(ostream & o,const selection<T> & s)
{
	s.write(o);
	return o;
};


template<class D,class T,class U>
D * selectOne(selection<T> & cs1,selection<U> & cs2)
{
	int c = rnd(cs1.size()+cs2.size());
	if(c < cs1.size()) return new T(cs1.selectOne());
	return new U(cs2.selectOne());
};

// Not quite a singleton:

class problem {
private:
	static problem * p;
public:
	selection<mode> modes;
	selection<target> targets;
	selection<observation> observations;

	problem(int ts,int os,int ms) : modes(ms), targets(ts)
	{
		p = this;
		observations = selection<observation>(os);
	};

	static problem * instance()
	{
		return p;
	};
	void write(ostream & o) const
	{
		o << modes << targets << observations;
	};
};

problem * problem::p = 0;

ostream & operator<<(ostream & o,const problem & p)
{
	p.write(o);
	return o;
};

observation::observation() : direction(), type(rnd(3)), interesting(rnd(10) < 9)
{
	int ims = 1+rnd(mode::howMany()/3);
	problem::instance()->modes.selectSeveral(images,ims);

	nm = types[type];
	ostringstream s;
	s << myId;
	nm += s.str();
	allDirections.push_back(this->clone());
	for(int i = 0;i < mode::howMany();++i)
		dataSize.push_back(1+rnd((int) (1000*tightness)));
	allObservations.push_back(new observation(*this));
};

class instrument : public ProblemObject {
private:
	static int id;
	int myID;

	vector<mode> supportedModes;
	vector<double> calibrationTimes;
	vector<target> targets;
public:
	instrument() : myID(id++)
	{
		int modes = 1+rnd(3);
		problem::instance()->modes.selectSeveral(supportedModes,modes,
							mem_fun_ref(&mode::supported));

		int targs = 1+rnd(target::howMany()/3);
		problem::instance()->targets.selectSeveral(targets,targs);
		for(int i = 0;i < targs;++i)
			calibrationTimes.push_back(100*rnd());

	};
	string name() const
	{
		ostringstream nm;
		nm << "instrument" << myID;
		return nm.str();
	};
	void object(ostream & o) const
	{
		o << "\tinstrument" << myID;
		if(typing==ON)
			o << " - instrument";
		o << "\n";
	};
	void init(ostream & o) const
	{

		if(typing==OFF)
			o << "\t(instrument instrument" << myID << ")\n";
		for(int i = 0;i < supportedModes.size();++i)
			o << "\t(supports instrument" << myID << " "
			  << supportedModes[i].name() << ")\n";
		o.precision(3);
		for(int i = 0;i < targets.size();++i)
		{
			o << "\t(calibration_target instrument" << myID <<
				" " << targets[i].name() << ")\n";
			if(probType==COMPLEX || probType==TIMED)
				o << "\t(= (calibration_time instrument" << myID <<
					" " << targets[i].name() << ") " <<
					calibrationTimes[i] << ")\n";
		};

	};
	void goal(ostream & o) const
	{};
};

int instrument::id = 0;

class satellite : public ProblemObject {
private:
	static int id;
	int myID;
	const direction * d;
	vector<instrument> instruments;

	const direction * end;
	bool interesting;
	double fuel;
public:
	satellite(int is) : myID(id++),
				d(selectOne<direction,target,observation>
				(problem::instance()->targets,
				problem::instance()->observations)),
		end(selectOne<direction,target,observation>
				(problem::instance()->targets,
				problem::instance()->observations)),
				interesting(rnd(5) < 2),
				fuel(100*(1+rnd()))
	{
		int insts = 1+rnd(is);
		for(int i = 0;i < insts;++i)
			instruments.push_back(instrument());
	};

	void object(ostream & o) const
	{
		o << "\tsatellite" << myID;
		if(typing==ON)
			o << " - satellite";
		o << "\n";

		copy(instruments.begin(),instruments.end(),
			ostream_iterator<instrument>(o,""));
	};
	void init(ostream & o) const
	{
		if(typing==OFF)
			o << "\t(satellite satellite" << myID << ")\n";
		copy(instruments.begin(),instruments.end(),
			ostream_iterator<instrument>(o,""));
		for(int i = 0; i < instruments.size(); ++i)
			o << "\t(on_board " << instruments[i].name() <<
					" satellite" << myID << ")\n";
		o << "\t(power_avail satellite" << myID << ")\n"
		  << "\t(pointing satellite" << myID << " " << d->name()
			<< ")\n";
		if(probType==NUMERIC || probType==COMPLEX)
			o << "\t(= (data_capacity satellite" << myID << ") 1000)\n";
		o.precision(3);
		if(probType==NUMERIC)
			o << "\t(= (fuel satellite" << myID << ") " << fuel << ")\n";

	};
	void goal(ostream & o) const
	{
		if(interesting)
			o << "\t(pointing satellite" << myID << " " << end->name() << ")\n";
	};
};

int satellite::id = 0;

void doMetric()
{
	switch(probType)
	{
		case SIMPLETIME:
		case TIMED:
			cout << "(:metric minimize (total-time))\n";
			break;
		case NUMERIC:
			if(metricType==EASY)
			{
				cout << "(:metric minimize (fuel-used))\n";
				return;
			};
		case COMPLEX:
			if(metricType==EASY)
			{
				cout << "(:metric minimize (total-time))\n";
			}
			else
			{
				cout << "(:metric maximize (data-stored))\n";
			};
			break;
		default:
			break;
	};
};


int main(int argc,char * argv[])
{
	if(argc < 2)
	{
		cout << "Usage: satgen [-T <d>|-u|-s|-t|-n|-c] <seed> <#s> <#i> <#m> <#t> <#o>\n\n\twhere: #s = number of satellites, #i = max instruments/satellite,\n\t\t#m = number of modes, #t = number of targets\n\t\t#o = number of observations\n\t-u: untyped\n\t-s: Simple-time\n\t-t: Time\n\t-n: Numeric\n\t-c: Complex\n\t-T: tightness of data in store capacities [0,1)\n\n";
		exit(0);
	};

	typing = ON;
	probType = STRIPS;
	metricType = EASY;
	tightness = 0.3;

	while(argv[1][0]=='-')
	{
		switch(argv[1][1])
		{
			case 'u':
				typing=OFF;
				break;
			case 't':
				probType=TIMED;
				break;
			case 's':
				probType=SIMPLETIME;
				break;
			case 'n':
				probType=NUMERIC;
				break;
			case 'c':
				probType=COMPLEX;
				break;
			case 'h':
				metricType = HARD;
				break;
			case 'T':
				tightness = strtod(argv[2],0);
				++argv;
				--argc;
				break;
			default:
				break;
		};
		++argv;
		--argc;
	};

	srandom(atoi(argv[1]));

	problem pp(atoi(argv[5]),atoi(argv[6]),atoi(argv[4]));
	int numsats = atoi(argv[2]);
	int numinsts = atoi(argv[3]);
	vector<satellite> sats;
	for(int i = 0;i < numsats;++i)
		sats.push_back(satellite(numinsts));

	cout << "(define (problem strips-sat-x-1)\n(:domain satellite)\n(:objects\n";
	copy(sats.begin(),sats.end(),ostream_iterator<satellite>(cout,""));
	cout << *problem::instance();
	cout << ")\n(:init\n";
	ProblemObject::Inits();
	copy(sats.begin(),sats.end(),ostream_iterator<satellite>(cout,""));
	cout << *problem::instance();
	if(probType==NUMERIC || probType==COMPLEX)
		cout << "\t(= (data-stored) 0)\n";
	if(probType==NUMERIC)
		cout << "\t(= (fuel-used) 0)\n";
	cout << ")\n(:goal (and\n";
	ProblemObject::Goals();
	copy(sats.begin(),sats.end(),ostream_iterator<satellite>(cout,""));
	cout << *problem::instance();
	cout << "))\n";
	doMetric();
	cout << "\n)\n";
	return 0;
};
