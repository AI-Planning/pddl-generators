using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>

int rnd(int limit) {
    return 1+(int) ((((double)((long int)limit)*random()))/(RAND_MAX+1.0));
};

int rnd2(int limit) {
    return limit/2 + rnd(limit/2);
};

int rnd3(int locs,int limit)
{
    return (int) (locs*(1.0+(double)(((long int)(limit-1)) * random())/(RAND_MAX+1.0)));
};


struct locatable {
    int location;
    locatable(int locs) : location(rnd(locs)) {};
    locatable() : location(0) {};
};


struct Truck : public locatable {
    int capacity;
    int speed;
    Truck(int locs,int maxc) : locatable(locs), capacity(rnd2(maxc)),
            speed(rnd(10))
    {};
};

struct Pallet : public locatable {
    int topcrate;
    int dtopcrate;
    Pallet(int locs) : locatable(locs), topcrate(0), dtopcrate(0) {};
};

struct Crate : public locatable {
    static int maxWeight;
    int weight;
    int pallet;
    int surface;

    int dpallet;
    int dsurface;

    Crate(int ps,int maxw) : locatable(), pallet(rnd(ps)),
                            weight(rnd(maxw)), dpallet(0), dsurface(0)
     {
        maxWeight = weight>maxWeight?weight:maxWeight;
     };
};

int Crate::maxWeight = 0;

struct Hoist : public locatable {
    int powr;

    Hoist(int locs) : locatable(locs), powr(rnd(10))
    {};
};

struct Map {
    vector< vector<int> > distances;

    Map(int locs)
    {
        for(int i = 0;i < locs;++i)
        {
            distances.push_back(vector<int>());
            for(int j = 0;j < locs;++j)
                distances[i].push_back(rnd(10));
        };
    };
};

struct DepotDescriptor {
    int numDepots;
    int numDistributors;
    int numTrucks;
    int numPallets;
    int numHoists;
    int numCrates;

    DepotDescriptor(int dps,int dsts,int trks,int plts,int hsts,int crts) :
        numDepots(dps), numDistributors(dsts), numTrucks(trks), numPallets(plts),numHoists(hsts), numCrates(crts) {};
};



class Depot {
private:
    int seed;

    int numDepots;
    int numDistributors;

    vector<Truck> trucks;
    vector<Pallet> pallets;
    vector<Hoist> hoists;
    vector<Crate> crates;

    Map * m;

    void location(ostream & o,int i) const
    {
        if(i < numDepots)
        {
            o << "depot" << i;
        }
        else
        {
            o << "distributor" << i - numDepots;
        };
    };

    void location(ostream & o,const locatable & l) const
    {
        location(o,l.location-1);
    };




public:
    Depot(unsigned int s,const DepotDescriptor & d) :
        seed(s)
    {
        srandom(seed);
        numDepots = d.numDepots;
        numDistributors = d.numDistributors;
        int numTrucks = d.numTrucks;
        int locs = numDepots+numDistributors;
        m = new Map(locs);
        int numPallets = max(d.numPallets,locs);
        int numHoists = max(d.numHoists,locs);

        for(int i = 0;i < numTrucks;++i)
        {
            Truck t(locs,0);
            trucks.push_back(t);
        };

        for(int i = 0;i < numHoists;++i)
        {
            Hoist h(locs);
            if(i < locs) h.location = i+1;
            hoists.push_back(h);
        };

        for(int i = 0;i < numPallets;++i)
        {
            Pallet p(locs);
            if(i < locs) p.location = i+1;
            pallets.push_back(p);
        };

        for(int i = 0;i < d.numCrates;++i)
        {
            Crate c(numPallets,0);
            c.location = pallets[c.pallet-1].location;
            c.surface = pallets[c.pallet-1].topcrate;
            pallets[c.pallet-1].topcrate = i+1;
            crates.push_back(c);
        };

        for(int i = 0;i < 2*d.numCrates; ++i)
        {
            int c = rnd(d.numCrates)-1;
            if(crates[c].dpallet) continue;
            crates[c].dpallet = rnd(numPallets);
            crates[c].dsurface = pallets[crates[c].dpallet-1].dtopcrate;
            pallets[crates[c].dpallet-1].dtopcrate = c+1;
        };


    };

    ~Depot() {delete m;};

    void write(ostream & o) const
    {
        o << "(define (problem depot-" << numDepots << "-" << numDistributors << "-" << trucks.size() << "-" << pallets.size() << "-" << hoists.size() << "-" << crates.size() << ") (:domain depots)\n(:objects\n\t";
        for(int i = 0;i < numDepots;++i)
        {
            o << "depot" << i << " ";
        };
        o << "- Depot\n\t";
        for(int i = 0;i < numDistributors;++i)
        {
            o << "distributor" << i << " ";
        };
        o << "- Distributor\n\t";
        for(int i = 0;i < trucks.size();++i)
        {
            o << "truck" << i << " ";
        };
        o << "- Truck\n\t";
        for(int i = 0;i < pallets.size();++i)
        {
            o << "pallet" << i << " ";
        };
        o << "- Pallet\n\t";
        for(int i = 0;i < crates.size();++i)
        {
            o << "crate" << i << " ";
        };
        o << "- Crate\n\t";
        for(int i = 0;i < hoists.size();++i)
        {
            o << "hoist" << i << " ";
        };
        o << "- Hoist";
        o << ")\n(:init\n";
        for(int i = 0;i < pallets.size();++i)
        {
          o << "\t(at pallet" << i << " ";
          location(o,pallets[i]);
          o << ")\n\t(clear ";
          if(pallets[i].topcrate)
            {
              o << "crate" << pallets[i].topcrate-1 << ")\n";
            }
          else
            {
              o << "pallet" << i << ")\n";
            };
        };
        for(int i = 0;i < trucks.size();++i)
        {
          o << "\t(at truck" << i << " ";
          location(o,trucks[i]);
          o << ")\n";
        };
        for(int i = 0;i < hoists.size();++i)
        {
          o << "\t(at hoist" << i << " ";
          location(o,hoists[i]);
          o << ")\n\t(available hoist" << i << ")\n";
        };
        for(int i = 0;i < crates.size();++i)
        {
            o << "\t(at crate" << i << " ";
            location(o,crates[i]);
            o << ")\n\t(on crate" << i << " ";
            if(crates[i].surface)
            {
                o << "crate" << crates[i].surface-1 << ")\n";
            }
            else
            {
                o << "pallet" << crates[i].pallet-1 << ")\n";
            };
        };

        o << ")\n\n(:goal (and\n";
        for(int i = 0;i < crates.size();++i)
        {
            if(crates[i].dpallet)
            {
                if(crates[i].dsurface)
                {
                    o << "\t\t(on crate" << i << " crate" <<
                            crates[i].dsurface-1 << ")\n";
                }
                else
                {
                    o << "\t\t(on crate" << i << " pallet" <<
                            crates[i].dpallet-1 << ")\n";
                };
            };
        };
        o << "\t)\n)";
        o << ")\n";

    };

};

ostream & operator<<(ostream& o,const Depot & d)
{
    d.write(o);
    return o;
};

void usage()
{
    cout << "Usage: depots -e <#depots> -i <#distributors> -t <#trucks> "
         << "-p <#pallets> -h <#hoists> -c <#crates> [-s <random_seed>]\n\n"
         << "\tAll numbers are positive integers (minimal 1).\n\n";

    exit(0);
};



DepotDescriptor commandLine(int & seed,int argc, char * argv[])
{
    int numDepots=0;
    int numDistributors=0;
    int numTrucks=0;
    int numPallets=0;
    int numHoists=0;
    int numCrates=0;

    char option;

    if (argc % 2 != 0 || argc < 12 || argc > 14) {
        usage();
    }

    while(argc>0)
    {
      if(*argv[0]=='-'){
        char o = *++argv[0];

        --argc;
        ++argv;
        if(argc <= 0) usage();

        switch(o){
        case 'e':
          numDepots=atoi(argv[0]);
          break;

        case 'i':
          numDistributors=atoi(argv[0]);
          break;

        case 't':
          numTrucks=atoi(argv[0]);
          break;

        case 'p':
          numPallets=atoi(argv[0]);
          break;

        case 'h':
          numHoists=atoi(argv[0]);
          break;

        case 'c':
          numCrates=atoi(argv[0]);
          break;

        case 's':
          seed=atoi(argv[0]);
          break;

        default:
          cout <<  "\n\nunknown option: " << option << " entered\n\n";
          usage();
        }
      }

      if(argc <= 0) usage();
      --argc;
      ++argv;
    }

    if(numDepots<1||numDistributors<1||numTrucks<1||numPallets<1||numHoists<1||numCrates<1){
      usage();
    }

    return DepotDescriptor(numDepots,numDistributors,numTrucks,numPallets,numHoists,numCrates);
};



int main(int argc, char *argv[]) {
    int seed = (int)time(NULL);

    DepotDescriptor d = commandLine(seed, --argc, ++argv);
    Depot dp(seed, d);
    cout << dp;

    return 0;
};
