#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <queue>
#include <functional>
#include <iterator>


using namespace std;

enum typeStatus {ON,OFF};
typeStatus typing;

int rnd(int limit) {
	return (int) ((((double)((long int)limit)*random()))/(RAND_MAX+1.0));
};

int rnd1(int limit) {
	return 1+(int) ((((double)((long int)limit)*random()))/(RAND_MAX+1.0));
};

int rnd2(int limit) {
	return limit/2 + rnd(limit/2);
};

int rnd3(int locs,int limit)
{
	return (int) (locs*(1.0+(double)(((long int)(limit-1)) * random())/(RAND_MAX+1.0)));
};

struct Waypoint{
	bool soil;
	bool rock;
	bool sunny;
	bool visited_s;
	bool visited_r;
	set<int> can_visit;
	Waypoint(int locs): visited_s(false), visited_r(false) {
		int x = rnd(locs);
		soil=(x%2==0);
		x = rnd(locs);
		rock=(x%2==0);
		x = rnd(10);
		sunny=(x<3);

	};
};



struct Camera{

	int cal_targ;
	int onboard;
	bool colour;
	bool high_res;
	bool low_res;
	Camera(int rovs,int obs) : cal_targ(rnd(obs)),onboard(rnd(rovs)){

		int x = rnd(7);
		colour=(x%2==0);
		x/=2;
		high_res=(x%2==0);
		x/=2;
		low_res=(x%2==0);
	};
	Camera(int rovs,int obs,int i) : cal_targ(rnd(obs)), onboard(i) {
		int x = rnd(7);
		colour=(x%2==0);
		x/=2;
		high_res=(x%2==0);
		x/=2;
		low_res=(x%2==0);
	};
	int getmode() const
	{
		int nmodes = colour+high_res+low_res;
		int tmode = rnd1(nmodes);
		nmodes = 0;
		if(colour && tmode)
		{
			--tmode;
			nmodes = 3;
		};
		if(high_res && tmode)
		{
			--tmode;
			nmodes = 1;
		};
		if(low_res && tmode)
		{
			nmodes = 2;
		};
		return nmodes;
	};
};

struct Objective{
	set<int> vis_from;
	vector<int> requests;
	Objective(int locs) {
		int x = rnd1(locs);
		for(int i = 0;i<x;i++){
			int y = rnd(locs);
			vis_from.insert(y);
                }
        }

    void make_visible_from(const set<int> & locations){
        for(int loc1: vis_from){
            if(locations.find(loc1) != locations.end()) return;
        }


        //Make it visible from a random location. Selecting the yth random element from a set in
        // a bit of a hacky way
        int y = rnd(locations.size());
        for(int loc2: locations){
            if (y-- == 0){
                vis_from.insert(loc2);
                return;
            }
        }
    }
};



typedef map<int,set<int> > graph;
struct Map {

	int waypoints;
	graph path;
	// neighbours[l]: sorted locations i with an edge l->i or i->l in path
	vector<vector<int> > neighbours;


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
				if(reached.find(*i)==reached.end())
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
		int start = rnd(waypoints);
		explore(g,start,reached);
		// reached only grows, so the search for the first unreached location
		// can continue from the previous one.
		int next = 0;
		while(reached.size()!=waypoints)
		{
			for(int i = next;i < waypoints;++i)
			{
				if(reached.find(i)==reached.end())
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


	Map(int s) : waypoints(s)
	{
		for(int i = 0;i < s;++i)
		{
			for(int j = 0;j < 5;++j)
			{
				int f = rnd(s);
				int t = rnd(s);
				if(f==t) continue;
				if(path[t].find(f)==path[t].end())
					path[f].insert(t);
			};

		};
		connect(path);

		neighbours.resize(s);
		for(graph::const_iterator i = path.begin();i != path.end();++i)
		{
			for(set<int>::const_iterator j = i->second.begin();
							j != i->second.end();++j)
			{
				neighbours[i->first].push_back(*j);
				neighbours[*j].push_back(i->first);
			};
		};
		for(int i = 0;i < s;++i)
		{
			sort(neighbours[i].begin(),neighbours[i].end());
			neighbours[i].erase(unique(neighbours[i].begin(),neighbours[i].end()),
						neighbours[i].end());
		};
	};
	Map(){};

	void write(ostream & o) const
	{

		for(graph::const_iterator i = path.begin();i != path.end();++i)
		{
			for(set<int>::const_iterator j = i->second.begin();
							j != i->second.end();++j)
			{
				if(i->first == *j) continue;
				if(path.find(*j)->second.find(i->first)
				     != path.find(*j)->second.end()
				   && *j < i->first) continue;

				o << "\t(visible waypoint" << i->first << " waypoint" <<
				*j << ")\n\t(visible waypoint" <<
				*j << " waypoint" << i->first
				<< ")\n";


			};
		};

	};

	int size() const {return waypoints;};

};

struct Rover{
	vector<pair<int,int> > travs;
	vector<int> cams;
	int rechargerate;
	int location;
	bool soil;
	bool rock;
	bool image;

        set<int> reachable_locs;

	Rover(int locs,Map & m) : location(rnd(locs)){
		rechargerate=rnd1(10)+10;
		int x = rnd(7);
		soil=x%2==0?true:false;
		x/=2;
		rock=x%2==0?true:false;
		x/=2;
		image=x%2==0?true:false;

		// Use a positive radius, so that each rover can traverse at least one edge.
		int radius = max(1, locs/3 + rnd(locs));
		vector<int> reachables;
		// in_travs[i]: some pair in travs has i as second element
		vector<bool> in_travs(locs,false);
		size_t front = 0;
                reachable_locs.insert(location);
		reachables.push_back(location);
		while(radius && front < reachables.size()){
				int l = reachables[front++];

				for(int i : m.neighbours[l]){
					if(!in_travs[i]){
						int y = rnd(10);
						if(y>2 || travs.empty()){
							travs.push_back(make_pair(l,i));
							travs.push_back(make_pair(i,l));
							in_travs[i] = in_travs[l] = true;
							reachables.push_back(i);
                                                        reachable_locs.insert(i);
						}
					}
				}
				radius--;

		}



	};

	void makeVisible(int gp,const Map & mp){

		vector<pair<int,int> >::const_iterator vi = travs.begin();
		for(;(vi != travs.end());++vi){
			graph::const_iterator l1 = mp.path.find(vi->first);
			graph::const_iterator l2 = mp.path.find(vi->second);

			if((l1 != mp.path.end() && l1->second.find(gp)!=l1->second.end()) ||
			   (l2 != mp.path.end() && l2->second.find(gp)!=l2->second.end())){

					break;

			}

		}
		if(vi==travs.end()){
			// This used to add an edge to a copy of the map (passed by
			// value), which had no effect on the problem. Avoid the copy and
			// only draw the random number to keep the random sequence.
			rnd(travs.size());
		}
	}


	void makeChargeable(vector<Waypoint> & waypoints){
		vector<pair<int,int> >::const_iterator vi = travs.begin();
		for(;(vi != travs.end());++vi){
			int l1 = vi->first;


			if(waypoints[l1].sunny){
				break;
			}
		}
		if(vi==travs.end()){
			int w = rnd(travs.size());
			vector<pair<int,int> >::pointer it = &travs[w];
			waypoints[it->first].sunny = true;
		}
	}


};

struct Goal {
	string s1;
	int i;
	string s2;
	Goal(string st,int j):s1(st), i(j), s2("") {};
	Goal(string st1,int j,string st2):s1(st1),i(j),s2(st2){};
	Goal(){}
};

ostream & operator <<(ostream & o,const Goal & g)
{
	o << g.s1 << g.i << g.s2;
	return o;
};

struct RoverDescriptor {
	enum ProblemType {STRIPS,NUMERIC,SIMPLETIMED,TIMED};
	int numRovers;
	int numWaypoints;
	int numModes;
	int numCameras;
	int numObjectives;
	int numgoals;

	ProblemType probtype;


	RoverDescriptor(int drs,int dws,int dms,int dcs,int ngs,ProblemType tp = RoverDescriptor::STRIPS) :
		numRovers(drs), numWaypoints(dws), numObjectives(dms), numCameras(dcs),numgoals(ngs),
		probtype(tp)

	{};
};


// Sequence of (objective,rover) pairs where each pair occurs once for every
// traversal of the rover from or to a location the objective is visible from.
// There can be very many, so equal pairs are stored once with the accumulated
// count.
struct SuitablePairs {
	vector<pair<int,int> > pairs;
	vector<size_t> ends; // ends[r]: number of elements in pairs[0..r]

	size_t size() const {return ends.empty() ? 0 : ends.back();};

	const pair<int,int> & operator[](size_t i) const
	{
		return pairs[upper_bound(ends.begin(),ends.end(),i) - ends.begin()];
	};
};

SuitablePairs getsuitable(const vector<Rover> & rovers,const vector<Objective> & objectives,int locs){
	SuitablePairs ps;
	size_t total = 0;
	for(int k = 0;k<rovers.size();++k){
		if(rovers[k].cams.empty()) continue;

		// touching[l]: number of traversals of rover k that start or end at l
		vector<int> touching(locs,0);
		vector<pair<int,int> >::const_iterator vi = rovers[k].travs.begin();
		for(;(vi != rovers[k].travs.end());++vi){
			++touching[vi->first];
			if(vi->second != vi->first) ++touching[vi->second];
		}
		for(int v =0;v<objectives.size();++v){
			size_t count = 0;
                    for(auto loc_vis_from :  objectives[v].vis_from){
				count += touching[loc_vis_from];
			}
			if(count){
				total += count;
				ps.pairs.push_back(make_pair(v,k));
				ps.ends.push_back(total);
			}
		}
	}
	return ps;
}

vector<int> rock_sites(const vector<Rover> & rovers,const vector<Waypoint> & waypoints){
	set<pair<int,int> > ps;
	for(int k = 0;k<rovers.size();++k){

		// touched[v]: some traversal of rover k starts or ends at v
		vector<bool> touched(waypoints.size(),false);
		vector<pair<int,int> >::const_iterator vi = rovers[k].travs.begin();
		for(;(vi != rovers[k].travs.end());++vi){
			touched[vi->first] = touched[vi->second] = true;
		}
		for(int v =0;v<waypoints.size();++v){

					if(touched[v]){

						if(rovers[k].rock && waypoints[v].rock){
						  ps.insert(make_pair(k,v));
						}
					}

		}
	}
	vector<int> vps;
	for(set<pair<int,int> >::iterator it = ps.begin(); it!=ps.end();++it){
	  vps.push_back(it->second);
	}
	return vps;
}

vector<int> soil_sites(const vector<Rover> & rovers,const vector<Waypoint> & waypoints){
	set<pair<int,int> > ps;
	for(int k = 0;k<rovers.size();++k){

		// touched[v]: some traversal of rover k starts or ends at v
		vector<bool> touched(waypoints.size(),false);
		vector<pair<int,int> >::const_iterator vi = rovers[k].travs.begin();
		for(;(vi != rovers[k].travs.end());++vi){
			touched[vi->first] = touched[vi->second] = true;
		}
		for(int v =0;v<waypoints.size();++v){

					if(touched[v]){

						if(rovers[k].soil && waypoints[v].soil){
						  ps.insert(make_pair(k,v));
						}
					}

		}
	}
	vector<int> vps;
	for(set<pair<int,int> >::iterator it = ps.begin(); it!=ps.end();++it){
	  vps.push_back(it->second);
	}

	return vps;
}

class RoverDom {

private:
	int seed;
	RoverDescriptor::ProblemType probtype;

	int numRovers;
	int numWaypoints;
	int numCameras;
	int numObjectives;
	int numgoals;
	Map m;
	int GP;

	vector<Rover> rovers;
	vector<Camera> cameras;
	vector<Objective> objectives;
	vector<Waypoint> waypoints;
	vector<Goal> thegoals;



	void location(ostream & o,int i) const
	{
			o << "waypoint" << i;


	};

	void observation(ostream & o,int i) const
	{

			o << "objective" << i;


	};

 	void location(ostream & o,const Rover & l) const
	{
		location(o,l.location);
	};


 	void target(ostream & o,const Camera & l) const
	{
		observation(o,l.cal_targ);
	};

	void mounted(ostream & o,int i) const
	{

			o << "rover" << i;


	};
	void mounted(ostream & o,const Camera & l) const
	{
		mounted(o,l.onboard);
	};


public:
	RoverDom(unsigned int s,const RoverDescriptor & d) :
		seed(s), probtype(d.probtype)
	{
		srandom(s);
		numRovers = d.numRovers;
		numWaypoints = d.numWaypoints;
		numCameras = d.numCameras;
		numObjectives = d.numObjectives;
		numgoals = d.numgoals;

		Map mp(numWaypoints);
		m = mp;


		GP = rnd(numWaypoints);

		for(int i = 0;i < numRovers;++i)
		{
			Rover r(numWaypoints,mp);

			r.makeVisible(GP,mp);
			rovers.push_back(r);
		};
		for(int i = 0;i < numCameras;++i)
		{
			Camera c(numRovers,numObjectives);
			rovers[c.onboard].image = true;
			cameras.push_back(c);
		};

		for(int i = 0;i<rovers.size();++i){
			if(rovers[i].image){
				int j = 0;
				for(;j<cameras.size();++j){
					if(cameras[j].onboard == i){
						rovers[i].cams.push_back(j);
					}
				}
				if(j==cameras.size() && rovers[i].cams.empty()){
					Camera c(numRovers,numObjectives,i);
					cameras.push_back(c);
					rovers[i].cams.push_back(cameras.size()-1);
				}
			}
		}
		for(int i = 0;i < numObjectives;++i)
		{
			Objective o(numWaypoints);
			objectives.push_back(o);
		};


		for(int i = 0;i < numWaypoints;++i)
		{
			Waypoint w(numWaypoints);
			waypoints.push_back(w);
		};

		for(int i = 0;i < numRovers;++i)
		{

			rovers[i].makeChargeable(waypoints);


		};


		int numsoilgoals = rnd1(numgoals) + numgoals/3;
		int numrockgoals = rnd1(numgoals) + numgoals/3;
		int numimagegoals = rnd1(numgoals) + numgoals/3;

		vector<int> accessible_for_soil = soil_sites(rovers,waypoints);
		set<int> ss;
		copy(accessible_for_soil.begin(),accessible_for_soil.end(),inserter(ss,ss.begin()));
		numsoilgoals = min(numsoilgoals,(int)ss.size());

		if(!accessible_for_soil.empty()){
			for(int i = 0;i<numsoilgoals;){

					int fi = rnd(accessible_for_soil.size());
					int s = accessible_for_soil[fi];

					if(!waypoints[s].visited_s){
					  Goal g("(communicated_soil_data waypoint",s,")");
					  waypoints[s].visited_s = true;
					  thegoals.push_back(g);
					  ++i;
					}

			}
		}
		vector<int> accessible_for_rock = rock_sites(rovers,waypoints);

		ss.clear();
		copy(accessible_for_rock.begin(),accessible_for_rock.end(),inserter(ss,ss.begin()));
		numrockgoals = min(numrockgoals,(int)ss.size());

		if(!accessible_for_rock.empty()){
			for(int i = 0;i<numrockgoals;){

					int fi = rnd(accessible_for_rock.size());
					vector<int>::pointer s = &accessible_for_rock[fi];

					if(!waypoints[*s].visited_r){

						Goal g("(communicated_rock_data waypoint",*s,")");
						waypoints[*s].visited_r = true;
						thegoals.push_back(g);
						++i;
					}


			}
		}

                //Ensure that all cameras can be calibrated
                for (const Camera & c : cameras) {
                    objectives[c.cal_targ].make_visible_from(rovers[c.onboard].reachable_locs);
                }

		SuitablePairs image_suitable_rovers = getsuitable(rovers,objectives,numWaypoints);
		numimagegoals = min((int)image_suitable_rovers.size(),numimagegoals);
		for(int i = 0;i<numimagegoals;++i){
				int fi = rnd(image_suitable_rovers.size());
				const pair<int,int> * f = &image_suitable_rovers[fi];
				int x = rnd(rovers[(*f).second].cams.size());
				int w = (*f).first;

				int xth = (rovers[(*f).second].cams)[x];

				int mode = cameras[xth].getmode();

				switch(mode)
				{
					case 1:
					{
						if(find(objectives[w].requests.begin(),objectives[w].requests.end(),1)==objectives[w].requests.end()){
							Goal g("(communicated_image_data objective",w," high_res)");
							objectives[w].requests.push_back(1);
							thegoals.push_back(g);
						}
					};
						break;

					case 2:
					{
						if(find(objectives[w].requests.begin(),objectives[w].requests.end(),2)==objectives[w].requests.end()){
							Goal g("(communicated_image_data objective",w," low_res)");
							objectives[w].requests.push_back(2);
							thegoals.push_back(g);
						}
					};

						break;

					default:
					{
						if(find(objectives[w].requests.begin(),objectives[w].requests.end(),3)==objectives[w].requests.end()){
							Goal g("(communicated_image_data objective",w," colour)");
							objectives[w].requests.push_back(3);
							thegoals.push_back(g);
						}
					};
				}



		}





	};

	void write(ostream & o) const
	{
		o << "(define (problem roverprob" << seed << ") (:domain Rover)\n(:objects\n\t";

		o << "general ";
		if(typing==ON) o << "- Lander\n\t";

		o << "colour high_res low_res ";
		if(typing==ON) o << "- Mode\n\t";

		for(int i = 0;i < numRovers;++i)
		{
			o << "rover" << i << " ";
		};
		if(typing==ON) o << "- Rover\n\t";

		for(int i = 0;i < numRovers;++i)
		{
			o << "rover" << i << "store ";
		};
		if(typing==ON) o << "- Store\n\t";
		for(int i = 0;i < numWaypoints;++i)
		{
			o << "waypoint" << i << " ";
		};
		if(typing==ON) o << "- Waypoint\n\t";

		for(int i = 0;i < cameras.size();++i)
		{
			o << "camera" << i << " ";
		};
		if(typing==ON) o << "- Camera\n\t";

		for(int i = 0;i < numObjectives;++i)
		{
			o << "objective" << i << " ";
		};
		if(typing==ON) o << "- Objective\n\t";

		o << ")\n(:init\n";

		m.write(o);

		if(typing==OFF)
		{
				o << "\t(lander general)\n";
		}
		if(typing==OFF)
		{
				o << "\t(mode colour)\n";
				o << "\t(mode high_res)\n";
				o << "\t(mode low_res)\n";

		};

		if(probtype==RoverDescriptor::NUMERIC)
			o << "\t(= (recharges) 0)\n";

		for(int i = 0;i<waypoints.size();++i){
			if(typing==OFF)
			{
				o << "\t(waypoint waypoint" << i << ")\n";


			};
			if(waypoints[i].soil){
				o << "\t(at_soil_sample waypoint" << i << ")\n";
			}
			if(waypoints[i].rock){
				o << "\t(at_rock_sample waypoint" << i << ")\n";
			}

			if(waypoints[i].sunny && (probtype==RoverDescriptor::NUMERIC || probtype==RoverDescriptor::TIMED)){
				o << "\t(in_sun waypoint" << i << ")\n";
			}
		};


		o << "\t(at_lander general waypoint" << GP << ")\n";
		o << "\t(channel_free general)\n";

		for(int i = 0;i < rovers.size();++i)
		{
			if(probtype == RoverDescriptor::NUMERIC || probtype == RoverDescriptor::TIMED) {
				o << "\t(= (energy rover" << i << ") 50)\n";
			}
			if(probtype == RoverDescriptor::TIMED){
				o << "\t(= (recharge-rate rover" << i << ") " << rovers[i].rechargerate << ")\n";
			}
			if(typing==OFF)
			{
				o << "\t(rover rover" << i << ")\n";
				o << "\t(store rover" << i << "store)\n";

			};
			o << "\t(at rover" << i << " ";
			location(o,rovers[i]);
			o << ")\n";
			o << "\t(available rover" << i << ")\n";
			o << "\t(store_of rover" << i << "store rover" << i << ")\n";
			o << "\t(empty rover" << i << "store)\n";

			if(rovers[i].soil){
				o << "\t(equipped_for_soil_analysis rover" << i << ")\n";
			}
			if(rovers[i].rock){
				o << "\t(equipped_for_rock_analysis rover" << i << ")\n";
			}
			if(rovers[i].image){
				o << "\t(equipped_for_imaging rover" << i << ")\n";
			}

			for(int j = 0; j<rovers[i].travs.size();++j){
				int from = (rovers[i].travs)[j].first;
				int to = (rovers[i].travs)[j].second;
				o << "\t(can_traverse rover" << i << " waypoint" <<  from << " waypoint" << to << ")\n";
			}

		};

		for(int i = 0;i < cameras.size();++i)
		{
			if(typing==OFF)
			{
				o << "\t(camera camera" << i << ")\n";

			};
			o << "\t(on_board camera" << i << " ";
			mounted(o,cameras[i]);
			o << ")\n";
			o << "\t(calibration_target camera" << i << " ";
			target(o,cameras[i]);
			o << ")\n";

			if(cameras[i].colour){
				o << "\t(supports camera" << i << " colour)\n";
			}
			if(cameras[i].high_res){
				o << "\t(supports camera" << i << " high_res)\n";
			}
			if(cameras[i].low_res){
				o << "\t(supports camera" << i << " low_res)\n";
			}

		};

		for(int i = 0;i < objectives.size();++i)
		{
			if(typing==OFF)
			{
				o << "\t(objective objective" << i << ")\n";

			};

			for(int loc_vis_from : objectives[i].vis_from){
				o << "\t(visible_from objective" << i << " waypoint" << loc_vis_from << ")\n";
			}
		};

		o << ")\n\n(:goal (and\n";

		copy(thegoals.begin(),thegoals.end(),ostream_iterator<Goal>(o,"\n"));

		if(probtype==RoverDescriptor::TIMED || probtype==RoverDescriptor::SIMPLETIMED){
			o << "\t)\n)\n\n(:metric minimize (total-time))\n)\n";
		}
		else {
			if(probtype==RoverDescriptor::NUMERIC)
			{
				o << "\t)\n)\n\n(:metric minimize (recharges))\n)\n";
			}
			else
			{
				o << "\t)\n)\n)\n";
			};
		}

	};

};
ostream & operator<<(ostream& o,const RoverDom & d)
{
	d.write(o);
	return o;
};



void usage(int u)
{
  cout << u;
	cout << "Usage: rovgen [-s|-t|-n|-u|-f <filename>]\n\t\t<seed> <#rovers> <#waypoints> <#objectives> <#cameras> <#n-goals> \n\n\tOptions:\n\tu: untyped version\n\ts: simple-time\n\tt: time\n\tn: numeric\n\tf: optional file for output\n\n\tAll numbers are integers.\n\n";

	exit(0);
};



RoverDescriptor commandLine(int & seed,string & filename,int argc, char * argv[])
{
	RoverDescriptor::ProblemType probtype = RoverDescriptor::STRIPS;
	typing=ON;

	int nxt = 0;
	int val[5];

	if(argc <= 0) usage(1);

	seed = atoi(argv[0]);
	--argc;
	++argv;

	while(argc>0)
	{
		if(*argv[0]=='-')
		{
			switch(char o = argv[0][1])
			{
				case 't':
					probtype = RoverDescriptor::TIMED;
					break;
				case 's':
					probtype = RoverDescriptor::SIMPLETIMED;
					break;
				case 'n':
					probtype = RoverDescriptor::NUMERIC;
					break;
				case 'u':
					typing=OFF;
					break;
				default:
					--argc;
					++argv;
					if(argc < 0) usage(2);
					switch(o)
					{
						case 'f':
							filename = string(argv[0]);
							break;
						default:
							usage(3);
					};
			};
			--argc;
			++argv;
		}
		else
		{
			if(nxt == 5) usage(4);
			val[nxt++] = atoi(argv[0]);
			++argv;
			--argc;
		};
	};

	if(nxt < 5) usage(5);

	return RoverDescriptor(val[0],val[1],val[2],val[3],val[4],probtype);
};



int main(int argc,char * argv[])
{
	int seed;
	string filename("");

	RoverDescriptor d = commandLine(seed,filename,--argc,++argv);
	RoverDom rp(seed,d);

	//ostream o;
	//if(filename != "")
	//{
	//	ofstream o(filename.c_str());
	//	o << rp;
	//}
	//else
	//{
		cout << rp;
		//};

	return 0;
};
