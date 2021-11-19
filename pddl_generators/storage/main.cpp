#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <getopt.h>
using namespace std;

#define PSYMB_BASE ('Z'-'A' + 1)
#define PSYMB_START 'A'
#define DEPOT_SYMBOL(x) ((char)(x<10)?(x +'0'):(x-10+'A'))
#define LAST_DEPOT  'Z'
#define MAX_DEPOTS  (LAST_DEPOT-'A'+11)
#define MAX_SA      302
#define MIN(x,y) ((x<y)?x:y)
#define MAX(x,y) ((x>y)?x:y)

// 2 corrisponde ad una probabilita' p=50% (1/2), 4 a 25% (1/4), etc
#define PBCOMPATIBILITIES 2
#define PBDEPOTSCONNECTED 2
#define MINSAPERDEPOT 1
#define SAPERCONTAINER 4
#define MINGOALCRATEPERDEPOT 1

// % deviation from the average dimension of the depots
#define SADEPOTDEVIATION 10

#define HOIST "hoist"
#define CRATE "crate"
#define DEPOT "depot"
#define CORRIDOR "transit"
#define CONTAINER "container"

#define MAX_PREF_IN_CONSTRAINTS 6
#define MAX_PREFERENCE          60

#define CRATES_PER_HOIST 2

void createSquareDepot(int);
void createLoad();
void createContainer(int,string,int);

void createCompatibilities();
void createLoadAreaConnections();
vector<string> appendVector(vector<string> global,vector<string> local);
void printOutput();
void createGoal();
void createGoalInCompatibilities();
void createRandomDepot();
void createGrid(int,int);
void createConnections();
void clearStoreareaArray();
void placeHoists();
void placeDoor();
void usage();
void computeWeights();
void drawMatrix(int **,int, int dep);
void createTils();
void allocateVector();
void allocateGlobalMap();
void connectDepots();
void createDepotConnections();
void addToGlobalMap();


const int MAXWEIGHT=10;
const int MAXPOWER=10;
const int MAXTIME=100;
const int MINWEIGHT=1;

int goalStored=1;


typedef struct _DEPOT_DESCRIPTION {
  int size;
  vector<string> *areas_from_door;
  vector<string> doors;
} depot_description;


int** s = new int* [MAX_SA];
int** globalMap;
string** globalCharArray;
vector<string> globalDefContainer;
vector<string> globalDefDepot;
vector<string> globalIns;
vector<string> globalDefStorearea;
vector<string> globalDefHoist;
vector<string> globalDefSc;
vector<string> globalDefCrate;
vector<string> globalConnections;
vector<string> globalClears;
vector<string> globalCrateOnContainer;
vector<string> globalCrateInContainer;
vector<string> globalScInContainer;
vector<string> globalLoadareaConnection;
vector<string> globalHoistAvailability;
vector<string> globalGoals;
vector<string> globalCompatibilities;
vector<string> globalWeights;
vector<string> globalLoads;
vector<string> globalTils;
vector<string> rightExtremes;
vector<string> leftExtremes;
vector<string> globalDepotConnections;
vector<string> globalDefCorridors;
vector<string> globalNumCrate;

vector<string> defSc;
vector<string> defCrate;
vector<string> connections;
vector<string> ins;
vector<string> defStorearea;
vector<string> crateOnContainer;
vector<string> crateInContainer;
vector<string> scInContainer;
vector<string> globalDoors;
vector<string> globalMapString;
vector<string> cratesInSameDepot;

depot_description *globalDepots = NULL;

vector<string> globalExtremes;
int globalConnIndex=0;
int rightConnected[MAX_SA];
int totalCrateNumber=5;
int totalDepotNumber=1;
int totalContainerNumber=1;
int totalHoistNumber=3;
int totalSaNumber=9;
int saDivision[MAX_SA];

int seed=0;
int compatibilityWarning=1;

string currentDepot;
int saDone=0;
int minGlobalY;
int maxGlobalY;
int maxGlobalX;

int gridDone=0;
int saNumber=0;
int hoistNumber=0;
char* filename="";
int compatibilities=1;
int randomDepot=0;
int derived=0;
int durative=0;
int timed=0;
int til=0;
int numeric=0;

int minLeft;
int maxDown;
int minUp;
int maxRight;
int oldminUp;
int oldmaxDown;

int maxSaInDepot = 0;


int currentX;
int currentY;
int corrRX;
int corrRY;
int corrLX;
int corrLY;
int oldcorrRX;
int oldcorrRY;
int oldcorrLX;
int oldcorrLY;

vector<string> pnames;
int pweight[MAX_PREFERENCE];
int ptotal[MAX_PREFERENCE];

static int simple_pref = 0;
static int constraints_pref = 0;
static int constraints = 0;
static int temporal_constraints = 0;
static int sometime_before = 0;
static int at_most_once = 0;
static int always_within = 0;
static int within = 0;
bool **crate_in_depot = NULL;
int sequence;
int last_w;
int weight_middle;
int weight_last;

int problem_number = -1;


string preference_symbol(int x)
{
  int r, q;
  string res = "";

  q = x - 1;
  do
    {
      r = q % PSYMB_BASE;
      q = q / PSYMB_BASE;
      res = to_string((char)(r + PSYMB_START)) + res;
    }
  while (q > 0);

  return res;
}

int main(int argc, char *argv[])
{
  allocateVector();
  allocateGlobalMap();
  char sw;
  int i=0;
  char *optarg;
  option opt;

  static struct option long_options[] = {};

  int option_index = 0;

  compatibilities=0;
  goalStored=0;


  while ( (sw = getopt_long(argc, argv, "hc:o:n:d:s:e:p:", long_options, &option_index)) != -1)
    {
      i++;
      switch (sw)
    {
    case 'p':
      problem_number = atoi(argv[optind-1]);
      break;
      /*
    case 'T': // durative actions
      timed=1;
      break;
    case 'N': // numeric
      numeric=1;
      break;
    case 'L': // TIL
      til=1;
      break;
    case 'X': // derived predicates
      derived=1;
      break;
    case 'D':
      durative = 1;
      break;
    case 'R': // random depot
      randomDepot=1;
      break;
    case 'J': // goals with predicates "in"
      goalStored=0;
      break;
    case 'K': // compatibilities off
      compatibilities=0;
      break;
      */
    case 'h': /* help option */
      usage();
      return 1;
      break;
    case 'c': /* crate number*/
      totalCrateNumber= atoi(argv[optind-1]);
      break;
    case 'o': /* container number*/
      totalContainerNumber= atoi(argv[optind-1]);
      break;
    case 'n': /* hoist number */
      totalHoistNumber= atoi(argv[optind-1]);
      break;
    case 'd': /* depot number */
      totalDepotNumber= atoi(argv[optind-1]);

      if (DEPOT_SYMBOL(totalDepotNumber-1) > LAST_DEPOT)
        {
          printf("\nError. Cannot create %d depots (MAX = %d)\n", totalDepotNumber, MAX_DEPOTS);
          exit(0);
        }

      break;
    case 's': /* storearea number */
      totalSaNumber= atoi(argv[optind-1]);

      if (totalSaNumber > (MAX_SA-2))
        {
          printf("\nError. Number of storeareas must be < %d\n", (MAX_SA-2));
          exit(0);
        }

      break;
    case 'e': /* seed of the random generator */
      seed=atoi(argv[optind-1]);
      break;
    case '?': /* unrecognized options */
      usage();
      exit(1);
      break;
    default: /* should not be reached */
      break;
    }
    }

  if (temporal_constraints)
    {
      constraints_pref = 1;
    }

  if (!constraints)
    {
      sometime_before = at_most_once = 1;
      within = always_within = 1;
    }

  //get file name
  filename=argv[optind];

  if (filename==0)
    {
      printf("Storage-Propositional Problem file generator\n");
      usage();
      return(1);
    }
  //input variables must be different from 0
  if(totalSaNumber==0 || totalHoistNumber==0 || totalContainerNumber==0 || totalCrateNumber ==0 || totalDepotNumber==0)
    {
      printf("\nError: Parameter values must be greater than zero\n");
      return(1);
    }

  if(totalCrateNumber > totalSaNumber)
    {
      printf("\nError: Number of crates is greater than the number of store-areas\n");
      return(1);
    }

  if(totalDepotNumber > totalSaNumber)
    {
      printf("\nError: Number of depots is greater than the number of store-area\n");
      return(1);
    }
  if(totalHoistNumber > totalSaNumber)
    {
      printf("\nError: Number of hoist is greater than the number of store areas\n");
      return(1);
    }
  if (((double)totalCrateNumber/(double)(max(totalContainerNumber-1,1)) <= (double)SAPERCONTAINER)&&totalContainerNumber>1)
    {
      cout << "\nWarning! Number of crates / Number of containers < "+to_string(SAPERCONTAINER)+"\n Containers will be filled randomly.\n";
    }
  if ((double)totalCrateNumber/(double)totalContainerNumber > (double)SAPERCONTAINER)
    {
      cout << "\nWarning! Number of crates / Number of containers > "+to_string(SAPERCONTAINER)+"\n One container is bigger than "+to_string(SAPERCONTAINER)+".\n";
    }

  if(totalCrateNumber > (totalSaNumber/2))
    {
      if (compatibilities)
    {
      printf("\nWarning! Number of crates > Number of store-areas / 2\nBecause of non-compatibility crates the problem could be unsolvable\n");
    }
    }
  else compatibilityWarning=0;

  if(seed==0)
    {
      seed=time(0);
    }

  srand(seed);

  int hoistDivision[MAX_SA];
  int saContainerDivision[MAX_SA];

  double probabilita=(double)SADEPOTDEVIATION/100;
  int media=totalSaNumber/totalDepotNumber;
  int saperdepot=max(media-(int)((double)media*probabilita),MINSAPERDEPOT);
  //cout << to_string(saperdepot);

  int saBound=totalSaNumber-saperdepot*totalDepotNumber;
  int scBound=totalCrateNumber;
  int hBound=totalHoistNumber;
  currentY=totalSaNumber/2;
  minGlobalY=totalSaNumber*2;
  maxGlobalY=0;




  createDepotConnections();

  /*** DIVISIONE DEGLI OGGETTI NEI VARI DEPOT ***/

  for (int i=0;i<totalDepotNumber-1;i++)
    {
      if (saBound>=0)
    {
      int random;
      int randomBound;
      if ((int) ((double)media*probabilita*2+1)<2) randomBound=2;
      else randomBound=(int) ((double)media*probabilita*2+1);
      if (saBound!=0) random=rand()%(min(saBound+1,randomBound));
      else random=0;
      saDivision[i]=saperdepot+random;
      saBound-=random;
    }
      else {cout<<"Error: The number of depots is too high!";cin.get();return 1;}
    }
  saDivision[totalDepotNumber-1]=saBound+saperdepot;



  for (i=0;i<totalDepotNumber;i++) hoistDivision[i]=0;
  int j=0;
  while (hBound>0)
    {
      {
    j=rand()%(totalDepotNumber+1);
    if (hBound>0)
      {
        if (saDivision[j]>0&&hoistDivision[j]<saDivision[j]) {hoistDivision[j]++;hBound--;}
      }
      }
    }

  if ((double)totalCrateNumber/(double)(max(totalContainerNumber-1,1)) > (double)SAPERCONTAINER)
    {
      for (int i=0;i<totalContainerNumber-1;i++)
    {
      if (scBound>=SAPERCONTAINER){
        saContainerDivision[i]=SAPERCONTAINER;
        scBound-=SAPERCONTAINER;}
    }
      saContainerDivision[totalContainerNumber-1]=scBound;
    }
  else
    {
      for (int i=0;i<totalContainerNumber-1;i++)
    {
      if (scBound>0)
        {
          saContainerDivision[i]=(rand()%(min(scBound,MINSAPERDEPOT)))+1;
          scBound-=saContainerDivision[i];
        }
      else {saContainerDivision[i]=0;scBound=0;}
    }
      saContainerDivision[totalContainerNumber-1]=scBound;
    }




  for(int i=0;i<totalContainerNumber;i++)
    {
      int saContainerNumber=saContainerDivision[i];
      string currentContainer=CONTAINER+to_string(i);
      globalDefContainer.push_back(currentContainer);
      //globalNumCrate.push_back("(= (num-crate "+currentContainer+") "+to_string(saContainerNumber)+")");
      if (saContainerNumber>0)
    {
      createContainer(saContainerNumber, currentContainer, i);
      globalDefCrate.insert( globalDefCrate.end(), defCrate.begin(), defCrate.end() );
      globalDefSc.insert( globalDefSc.end(), defSc.begin(), defSc.end() );
      globalCrateOnContainer.insert( globalCrateOnContainer.end(), crateOnContainer.begin(), crateOnContainer.end() );
      globalCrateInContainer.insert( globalCrateInContainer.end(), crateInContainer.begin(), crateInContainer.end() );
      globalScInContainer.insert( globalScInContainer.end(), scInContainer.begin(), scInContainer.end() );
    }
    }

  for(int i=0;i<totalDepotNumber;i++)
    {
      saNumber=saDivision[i];
      hoistNumber=hoistDivision[i];
      currentDepot=DEPOT+to_string((char)DEPOT_SYMBOL(i));
      globalDefDepot.push_back(currentDepot);

      if (saNumber>0){
    clearStoreareaArray();
    if (randomDepot) createRandomDepot();
    else createSquareDepot(i);

    drawMatrix(s,0,i);
    addToGlobalMap();
    saDone+=saNumber;

    globalDefStorearea.insert(globalDefStorearea.end(), defStorearea.begin(), defStorearea.end());
    globalConnections.insert(globalConnections.end(), connections.begin(), connections.end());
    globalIns.insert(globalIns.end(), ins.begin(), ins.end());
    globalConnIndex++;
      }
    }



  createLoadAreaConnections();
  connectDepots();

  saNumber=totalSaNumber;//drawMatrix si basa su questa var, serve grande per globalMap
  drawMatrix(globalMap,1,-1);



  //    if (timed||til) {
  computeWeights();//}
  //    if (til)
  createTils();

  if (numeric)
    createLoad();

  createGoal();
  if (goalStored) createCompatibilities();

  if (globalCompatibilities.size() <= 0 && totalCrateNumber > 1)
    globalCompatibilities.push_back("(compatible "+globalDefCrate[0]+" "+globalDefCrate[1]+") (compatible "+globalDefCrate[1]+" "+globalDefCrate[0]+")");


  printOutput();
  //cin.get();
  return EXIT_SUCCESS;
}

void addToGlobalMap()
{
  int oldX=0;

  if (oldcorrRY!=0)
    currentY=oldcorrRY-corrLY+minUp;

  for (int y=minUp-1;y<=maxDown+1;y++)
    {
      minGlobalY=min(currentY+y-minUp,minGlobalY);
      maxGlobalY=max(currentY+y-minUp,maxGlobalY);
      for (int x=minLeft-1;x<=maxRight+1;x++)
    {
      globalMap[currentY+y-minUp][currentX+x-minLeft]=s[y][x];
      if (s[y][x]==3
          && max(x,oldX)==x)
        {
          oldX=x;
          oldcorrRY=currentY+y-minUp;
        }
      maxGlobalX=currentX+x-minLeft;
    }
    }


  currentX+=maxRight-minLeft+2;

}

void createDepotConnections()
{
  for (int i=0;i<totalDepotNumber;i++)
    {
      rightConnected[i] = rand() % PBDEPOTSCONNECTED;
    }
  rightConnected[totalDepotNumber-1]=0;//ultimo non puo essere connesso a nulla
}


void createConnections()
{
  ins.clear();
  connections.clear();
  defStorearea.clear();

  minLeft=saNumber*2;
  maxRight=0;
  maxDown=0;
  minUp=saNumber*2;
  string minLeftS;
  string maxRightS;
  string minUpS;
  string maxDownS;

  oldminUp=minUp;
  oldmaxDown=maxDown;


  for (int i=1;i<=saNumber*2;i++)
    for (int j=1;j<=saNumber*2;j++)
      {
    if (defStorearea.size()<=saNumber)
      {
        string currentS=currentDepot+"-"+to_string(i)+"-"+to_string(j);
        string connectedS;
        if (s[i][j]>0)
          {
        if (j<minLeft)
          {
            //printf("\nMinLeft == %i\n", j);
            minLeftS=currentS;
            minLeft=min(j,minLeft);
            corrLX=j-1;
            corrLY=i;
          }
        if (j>maxRight)
          {
            //printf("\nMaxRight == %i\n", j);
            maxRightS=currentS;
            maxRight=max(j,maxRight);
            corrRX=j+1;
            corrRY=i;
          }
        if (i<minUp)
          {
            //printf("\nMinUp == %i\n", i);
            minUpS=currentS;
            minUp=min(i,minUp);
          }
        if (i>maxDown)
          {
            //printf("\nMaxDown == %i\n", i);
            maxDownS=currentS;
            maxDown=max(i,maxDown);
          }


        if (s[i][j]==2)
          globalDoors.push_back("(connected "+currentS+" loadarea)"+"\n\t(connected "+"loadarea "+currentS+")");

        ins.push_back("(in "+currentS+" "+currentDepot+")");

        defStorearea.push_back(currentS);

        if(s[i-1][j]>0)
          {
            connectedS=currentDepot+"-" + to_string(i-1) +"-"+ to_string(j);
            connections.push_back("(connected " + currentS + " "+connectedS+ ")");

          }
        if(s[i+1][j]>0)
          {
            connectedS=currentDepot+"-"+to_string(i+1)+"-"+to_string(j);
            connections.push_back("(connected "+currentS+" "+connectedS+")");

          }

        if(s[i][j+1]>0)
          {
            connectedS=currentDepot+"-"+to_string(i)+"-"+to_string(j+1);
            connections.push_back("(connected "+currentS+" "+connectedS+")");

          }
        if(s[i][j-1]>0)
          {
            connectedS=currentDepot+"-"+to_string(i)+"-"+to_string(j-1);
            connections.push_back("(connected "+currentS+" "+connectedS+")");
          }
          }
      }
      }

  globalExtremes.push_back(minLeftS);
  globalExtremes.push_back(maxRightS);
  if (rightConnected[globalConnIndex]==1)
    s[corrRY][corrRX]=3;
  if (globalConnIndex>0&&rightConnected[globalConnIndex-1]==1)
    s[corrLY][corrLX]=3;


  placeHoists();

}

void connectDepots()
{
  for (int i=1;i<globalExtremes.size()-1;i+=2)
    {
      if (rightConnected[i/2]==1)
    {
      string currentCorridor=CORRIDOR+to_string(globalDefCorridors.size());
      globalDefCorridors.push_back(currentCorridor);
      globalDepotConnections.push_back("(connected "+currentCorridor+" "+globalExtremes[i]+")\n\t(connected "+currentCorridor+" "+globalExtremes[i+1]+")");
    }
    }
}

void allocateVector()
{
  // alloca la prima dimensione
  for(int i=0; i<MAX_SA; i++)
    {
      s[i] = new int [MAX_SA];
    }

  // allocazione della seconda dimensione
  // e inizializzazione dell'Array
  for(int i=0; i < MAX_SA; i++)
    {
      for(int j=0; j < MAX_SA; j++)
    {
      s[i][j] = 0;
    }
    }
}

void allocateGlobalMap()
{
  globalMap = new int* [MAX_SA + 1];
  globalMap = &globalMap[1]; //dirty trick!

  globalCharArray = new string* [MAX_SA + 1];
  globalCharArray = &globalCharArray[1];

  // alloca la prima dimensione
  for(int i=-1; i<MAX_SA; i++)
    {
      globalMap[i] = new int [MAX_SA + 1];
      globalMap[i] = &globalMap[i][1];
      globalCharArray[i] = new string [MAX_SA + 1];
      globalCharArray[i] = &globalCharArray[i][1];
    }

  // allocazione della seconda dimensione
  // e inizializzazione dell'Array
  for(int i=-1; i<MAX_SA; i++)
    {
      for(int j=-1; j<MAX_SA; j++)
    {
      globalMap[i][j] = 0;
      globalCharArray[i][j]=" ";
    }
    }
}


void createTils()
{
  for (int i=0;i<globalDefDepot.size();i++)
    {
      globalTils.push_back("(at "+to_string(0)+"(available "+globalDefDepot[i]+"))");
    }
  for (int i=0;i<globalDefContainer.size();i++)
    {
      globalTils.push_back("(at "+to_string(rand()%MAXTIME)+"(available "+globalDefContainer[i]+"))");
    }

}

void createLoad()
{
  int random = 0;
  int powerok = 0;
  int crates = 0;
  int max_load = 0;
  int loads[totalHoistNumber];

  for (int i = 0; i < globalDefHoist.size(); i++)
    {
      random = rand()%(MAXWEIGHT-MINWEIGHT);
      random += MINWEIGHT;

      if (random == MAXWEIGHT)
    powerok = 1;

      if (!powerok && (i == (globalDefHoist.size()-1)))
    random = MAXWEIGHT;

      loads[i] = random;
      if (loads[i] > max_load)
    max_load = loads[i];

      globalLoads.push_back("(= (max-load " + globalDefHoist[i] + ") " + to_string(random) + ")");
    }


  crates = 0;
  for (int i = 0; i < totalHoistNumber; i++)
    {
      for (int k = 0; k < CRATES_PER_HOIST; k++)
    {

      if (crates >= totalCrateNumber)
        break;

      if (loads[i] > MINWEIGHT)
        random = rand() % (loads[i] - MINWEIGHT) + MINWEIGHT;
      else
        random = MINWEIGHT;

      globalLoads.push_back("(= (weight " + globalDefCrate[i] + ") " + to_string(random) + ")");
      crates++;
    }
    }

  for (int i = crates; i < totalCrateNumber;i++)
    {
      random = rand()%(max_load - MINWEIGHT);
      random += MINWEIGHT;
      globalLoads.push_back("(= (weight "+globalDefCrate[i]+") " + to_string(random)+")");
    }
}


void computeWeights()
{
  int random=0;

  for (int i=0;i<globalDefHoist.size();i++)
    {

      globalWeights.push_back("(= (loaded-weight "+globalDefHoist[i]+") "+"1"+")");
      random=rand()%MAXPOWER;
      globalWeights.push_back("(= (power "+globalDefHoist[i]+") "+to_string(random)+")");
    }

  for (int i=0;i<globalDefCrate.size();i++)
    {
      random=rand()%(MAXWEIGHT-MINWEIGHT);
      random+=MINWEIGHT;
      globalWeights.push_back("(= (weight "+globalDefCrate[i]+") "+to_string(random)+")");
    }
}


void drawMatrix(int **s,int global, int dep)
{
  vector<string> mapString;
  mapString.clear();
  int minY=0;
  int maxY=saNumber;
  int maxX=saNumber;
  int nextDepot=0;
  int changedDepot=1;
  int leglen = strlen(" Map of the Depots") + 1;
  int spacing = 0;

  if(global)
    {
      minY=minGlobalY;
      maxY=maxGlobalY;
      maxX=maxGlobalX;

      /*
      for (int j=0;j<=leglen;j++)
    {

      if (j == 0)
        globalCharArray[maxY][j] = "\n;";
      else
        globalCharArray[maxY][j] = "-";
    }
      */

      for (int j=0; j<=saNumber+spacing;j++)
    {

      globalCharArray[maxY][j] = "-";

      changedDepot=1;
      for (int i=minY+1;i<=maxY;i++)
        {

          if (j==0) {
        globalCharArray[i][j]="\n;";
        changedDepot=0;
          }
          else
        {
          if (s[i][j-1]==1)
            {
              globalCharArray[i][j+1] = DEPOT_SYMBOL(nextDepot);
              globalCharArray[maxY][j+1] = "-";
              changedDepot=0;
            }
          else if (s[i][j-1]==2)
            {
              globalCharArray[i][j+1] = "*";
              globalCharArray[maxY][j+1] = "-";
              changedDepot=0;
            }
          else if (s[i][j-1]==3)
            {
              globalCharArray[maxY][j+1] = "-";
              globalCharArray[i][j+1] = "=";
            }
          else
            globalCharArray[i][j+2] = " ";
        }
        }

      if(changedDepot) {
        nextDepot++;
        if (nextDepot == totalDepotNumber)
          break;
        spacing++;
      }
    }

      for (int i=minY;i<=maxY;i++)
    {
      for (int j=0;j<=saNumber+spacing+2;j++)
        {
          /*if (globalCharArray[i][j]==1) mapString.push_back( depotSymbol);
        else if (globalCharArray[i][j]==2) mapString.push_back( "*");
        else if (globalCharArray[i][j]==3) mapString.push_back( "=");
        else mapString.push_back( "-");*/
          mapString.push_back( globalCharArray[i][j]);
        }
    }

      globalMapString=mapString;
    }
  else
    for (int i=minY;i<=maxY;i++)
      {
    mapString.push_back( "\n;");
    for (int j=0;j<=saNumber;j++)
      {
        if (s[i][j]==1)
          mapString.push_back(""+DEPOT_SYMBOL(dep));
        else if (s[i][j]==2)
          mapString.push_back( "*");
        else if (s[i][j]==3)
          mapString.push_back( "=");
        else
          mapString.push_back( " ");
      }
      }



}

void createGrid(int i,int j)
{
  s[i][j]=1;
  while (gridDone<saNumber)
    {


      int down=rand()%4;
      int up=rand()%3;
      int right=rand()%9;
      int left=rand()%27;
      int whereNow=rand()%4;
      if(down!=1&&up!=1&&right>=4&&left>=16)
    {
      down=1;
      left=1;
      right=1;
      up=1;
    }

      if(down==1&&s[i+1][j]!=1&&gridDone<saNumber)
    {
      gridDone++;
      s[i+1][j]=1;
    }
      if(up==1&&s[i-1][j]!=1&&gridDone<saNumber)
    {
      gridDone++;
      s[i-1][j]=1;
    }
      if(right<4&&s[i][j+1]!=1&&gridDone<saNumber)
    {
      gridDone++;
      s[i][j+1]=1;
    }
      if(left<16&&s[i][j-1]!=1&&gridDone<saNumber)
    {
      gridDone++;
      s[i][j-1]=1;
    }
      if(gridDone<saNumber)
    {

      switch( whereNow )
        {

        case 0 :
          i++;

          break;
        case 1 :
          i--;

          break;
        case 2 :
          j++;

          break;
        case 3 :
          j--;

          break;
        }
      if (s[i][j]==0){s[i][j]=1;gridDone++;}
    }
    }
}

void createRandomDepot()
{
  gridDone=1;
  clearStoreareaArray();
  createGrid(saNumber,saNumber);

  placeDoor();
  createConnections();
}

void placeDoor()
{
  for (int i = 0; i < saNumber*2; i++)
    {
      if(s[i][saNumber] == 1 && s[i+1][saNumber] == 0)
    {
      s[i][saNumber]=2;
      return;
    }
    }

}


void createGoal()
{
  int j=0;
  int crateDivision[MAX_SA];
  for (int i=0;i<MAX_SA;i++)
    crateDivision[i]=0;

  int crateBound=totalCrateNumber;//-MINGOALCRATEPERDEPOT*totalDepotNumber;
  int crateNumber;
  int saAvailable[MAX_SA];

  crate_in_depot = (bool **)calloc(totalDepotNumber, sizeof(bool *));

  for (int i=0;i<totalDepotNumber;i++)
    {
      crate_in_depot[i] = (bool *)calloc(totalCrateNumber, sizeof(bool));

      if(!compatibilityWarning&&saDivision[i]>1)
    {
      if ((double)saDivision[i]/2>saDivision[i]/2 )saAvailable[i]=saDivision[i]/2+1;
      else saAvailable[i]=saDivision[i]/2;
    }
      else saAvailable[i]=saDivision[i];
    }


  for (int i=0;i<totalDepotNumber;i++)
    {

      if (crateBound>0){
    int random;
    if(saAvailable[i]-(MINGOALCRATEPERDEPOT*(totalDepotNumber-i-1))<=0) random=rand()%(saAvailable[i]);
    else random=rand()%(saAvailable[i]-(MINGOALCRATEPERDEPOT*(totalDepotNumber-i-1)));
    crateDivision[i]=min(MINGOALCRATEPERDEPOT+random,crateBound);

    crateBound-=crateDivision[i];
    saAvailable[i]-=crateDivision[i];}
      //    else {cout<<"ERRORE: FUNZIONO MALE!";cin.get();}
    }

  for (int i=0;i<totalDepotNumber;i++)
    {
      if (crateBound>0){

    crateDivision[i]+=min(saAvailable[i],crateBound);

    crateBound-=min(saAvailable[i],crateBound);
    saAvailable[i]-=min(saAvailable[i],crateBound);
      }
      //else {cin.get();}
    }


  for(int i=0;i<totalDepotNumber;i++)
    {

      crateNumber=crateDivision[i];
      currentDepot=globalDefDepot[i];
      cratesInSameDepot.clear();

      if (crateNumber>0)
    for (int k=0;k<crateNumber;k++)
      {
        if (!derived)
          {
        if (!goalStored)
          {
            globalGoals.push_back("(in "+globalDefCrate[j]+" "+currentDepot+")");
            cratesInSameDepot.push_back(globalDefCrate[j]);
            crate_in_depot[i][j] = true;
          }
        else globalGoals.push_back("(stored "+globalDefCrate[j]+")");

          }
        else
          {
        globalGoals.push_back("(stored "+globalDefCrate[j]+" "+currentDepot+")");
        globalGoals.push_back("(loadable "+globalDefCrate[j]+")");
          }
        j++;
      }

    }

  if(!goalStored)
    createGoalInCompatibilities();
}



void createGoalInCompatibilities()
{
  int groups;
  int i, j, n, start, end, add, g;
  int depotSaNumber;

  groups = (int)ceil((float)totalCrateNumber * totalDepotNumber / totalSaNumber);

  //printf("\nGruppi compatibilità = %i", groups);

  n = totalCrateNumber / groups;
  add = totalCrateNumber % groups;

  //printf("\nCrates per ogni gruppo = %i [%i]", n, add);

  g = 0;
  start = 0;
  end = MIN((start + n),  totalCrateNumber);
  if ((end < totalCrateNumber) && (g < add))
    end++;
  while (start < end)
    {
      //printf("\nNUOVO GRUPPO: %i - %i\n  -- num crates = %i", start, end-1, end - start);
      for (i = start; i < end; i++)
    for (j = i+1; j < end; j++)
      {
        globalCompatibilities.push_back("(compatible "+globalDefCrate[i]+" "+globalDefCrate[j]+")\n\t(compatible "+globalDefCrate[j]+" "+globalDefCrate[i]+")");
      }
      g++;
      start = end;
      end = MIN((start + n),  totalCrateNumber);
      if ((end < totalCrateNumber) && (g < add))
    end++;
    }
  //printf("\n");

  /*
  for (i =0;i<totalDepotNumber;i++)
    {
      depotSaNumber = saDivision[i];
      for (int i=0;i<cratesInSameDepot.size();i++)
    for (int j=i+1;j<cratesInSameDepot.size();j++)
      {

        int random=rand()%PBCOMPATIBILITIES;
        if (random==1)
          {
        globalCompatibilities.push_back("(compatible "+cratesInSameDepot[i]+" "+cratesInSameDepot[j]+")\n\t(compatible "+cratesInSameDepot[j]+" "+cratesInSameDepot[i]+")");
          }
      }
    }
  */
}

void createLoadAreaConnections()
{
  for (int i=0;i<totalCrateNumber;i++)
    {
      globalLoadareaConnection.push_back("(connected loadarea "+globalDefSc[i]+") "+"\n\t(connected "+globalDefSc[i]+" loadarea)");
    }
}

void createCompatibilities()
{
  for (int i=0;i<globalDefCrate.size();i++)
    for (int j=i+1;j<globalDefCrate.size();j++)
      {

    int random=rand()%PBCOMPATIBILITIES;
    if (random==1)
      {
        globalCompatibilities.push_back("(compatible "+globalDefCrate[i]+" "+globalDefCrate[j]+")\n\t(compatible "+globalDefCrate[j]+" "+globalDefCrate[i]+")");
      }
      }
}


void allocDepotDescription(int depot, int max_distance)
{
  if (globalDepots == NULL)
    {
      globalDepots = (depot_description *)calloc(totalDepotNumber, sizeof(depot_description));
    }

  globalDepots[depot].size = 0;
  globalDepots[depot].areas_from_door = new vector<string> [max_distance];
}


void insertInDepotDescription(int depot, int x, int y, int fromDoor, bool as_door, bool is_door)
{
  if ((fromDoor+1) >= globalDepots[depot].size)
    globalDepots[depot].size = fromDoor+1;

  if (is_door)
    {
      globalDepots[depot].doors.push_back("depot" + to_string((char)DEPOT_SYMBOL(depot)) + "-" + to_string(x) + "-" + to_string(y));
      return;
    }

  if (as_door)
    {
      globalDepots[depot].areas_from_door[0].push_back("depot" + to_string((char)DEPOT_SYMBOL(depot)) +
                               "-" + to_string(x) + "-" + to_string(y));
    }
  else
    {
      globalDepots[depot].areas_from_door[fromDoor].push_back("depot" + to_string((char)DEPOT_SYMBOL(depot)) +
                                  "-" + to_string(x) + "-" + to_string(y));
    }
}


void createSquareDepot(int depotNumber)
{
  int dimX=(int)sqrt((float)saNumber);
  int dimY;
  int doorX = 0, doorY = 0;
  int distance;
  bool hole;

  if (((saNumber/dimX)*dimX)!=saNumber)
    dimY=(int)(saNumber/dimX)+1;
  else
    dimY=(int)saNumber/dimX;

  int k=1;

  if (maxSaInDepot < saNumber)
    maxSaInDepot = saNumber;

  clearStoreareaArray();

  allocDepotDescription(depotNumber, (dimX + dimY));

  //first place door!!
  doorX = dimX;
  doorY = (rand()%dimY)+1;
  s[doorX][doorY] = 2;
  //insertInDepotDescription(depotNumber, doorX, doorY, 0);

  for (int i=1;i<=dimX;i++)
    for (int j=1;j<=dimY;j++)
      {
    if (k < saNumber)
      {
        if (s[i][j] == 0)
          {
        s[i][j]=1;
        //distance = abs(i - doorX) + abs(j - doorY);
        //insertInDepotDescription(depotNumber, i, j, distance);
        k++;
          }
      }
      }

  createConnections();



  for (int i=1;i<=dimX;i++)
    for (int j=1;j<=dimY;j++)
      {
    if (s[i][j] > 0)
      {

        if ((i == doorX) && (j == doorY))
          {
        insertInDepotDescription(depotNumber, i, j, 0, true, true);
        continue;
          }

        if (i < doorX)
          {
        distance = abs(i - doorX) + abs(j - doorY);
          }
        else
          {
        hole = false;
        for (int check = j; check < doorY; check++)
          if (s[i][check] <= 0)
            {
              hole = true;
              break;
            }

        if (!hole)
          {
            distance = abs(i - doorX) + abs(j - doorY);
          }
        else
          {
            distance = abs(i - 1 - doorX) + abs(j - doorY) + 1;
          }
          }

        //Check corridoi: stesso peso della porta davanti ai corridoi
        if (false && rightConnected[depotNumber])
          {
        if ((i == 1) && (j == dimY))
          {
            insertInDepotDescription(depotNumber, i, j, distance, true, false);
            continue;
          }
          }

        if (false && (depotNumber > 0) && (rightConnected[depotNumber-1]))
          {
        if ((i == 1) && (j == 1))
          {
            insertInDepotDescription(depotNumber, i, j, distance, true, false);

            continue;
          }
          }
        //fine check corridoi

        insertInDepotDescription(depotNumber, i, j, distance, false, false);
      }
      }

}




void clearStoreareaArray()
{
  for (int i=0;i<MAX_SA;i++)
    for (int j=0;j<MAX_SA;j++)
      s[i][j]=0;
}


void placeHoists()
{
  int i=0;
  int pos;
  int size;
  int areas[defStorearea.size()];

  size = defStorearea.size();

  for (i = 0; i < size; i++)
    areas[i] = i;

  i = 0;
  while (i<hoistNumber)
    {
      globalDefHoist.push_back(HOIST+to_string(globalDefHoist.size()));

      pos = rand() % size;

      globalHoistAvailability.push_back("(at "+globalDefHoist[globalDefHoist.size()-1]+" "+defStorearea[areas[pos]]+")\n\t(available "+globalDefHoist[globalDefHoist.size()-1]+")");

      areas[pos] = areas[--size];

      i++;
    }

  i = 0;
  while (i < size)
    {
      globalClears.push_back("(clear "+defStorearea[areas[i]]+")");
      i++;
    }

}

void createContainer (int saContainerNumber, string currentContainer, int container_number)
{
  crateOnContainer.clear();
  crateInContainer.clear();
  scInContainer.clear();
  defSc.clear();
  defCrate.clear();

  for (int i=0;i<saContainerNumber;i++)
    {
      string currentCrate=CRATE+to_string(i+globalDefCrate.size());
      defCrate.push_back(currentCrate);
      string currentSc="container-"+to_string(container_number)+"-"+to_string(i);
      crateOnContainer.push_back("(on "+currentCrate+" "+currentSc+")");
      crateInContainer.push_back("(in "+currentCrate+" "+currentContainer+")");
      defSc.push_back(currentSc);
      scInContainer.push_back("(in "+currentSc+" "+currentContainer+")");
    }
}

int search_in_preferences_table(string name)
{
  int i;

  for (i = 0; i < pnames.size(); i++)
    {
      if (pnames[i] == name)
    return i;
    }

  return -1;
}

int new_preference(int weight, string name)
{
  int p;

  p = search_in_preferences_table(name);

  if (p >= 0)
    {
      ptotal[p]++;
      return p;
    }

  p = pnames.size();
  pweight[pnames.size()] = weight;
  pnames.push_back(name);
  ptotal[p] = 1;

  return p;
}


string pref_name(int p)
{
  return (pnames[p] + preference_symbol(ptotal[p]));
}

void create_simple_preferences(ofstream *os)
{

  int i, j, m, k, p = 0;
  int weight;
  int pdiff = 0;
  int gap;

  pnames.clear();

  i = sequence = 0;
  if (totalCrateNumber == 1)
    pdiff = 1;

  for (i = 0; i < totalDepotNumber; i++)
    {
      for (j = 0; j < globalDepots[i].doors.size(); j++)
    {
      weight = globalDepots[i].size;
      p = new_preference(weight, "p" + to_string(weight));
      (*os) << "\n\t(preference " + pref_name(p) + " (clear " + globalDepots[i].doors[j] + "))";
      if (weight > sequence)
        sequence = weight;
    }

      for (j = 0; j < globalDepots[i].size; j++)
    {
      for (m = 0; m < globalDepots[i].areas_from_door[j].size(); m++)
        {
          if ((globalDepots[i].size - 1) > j)
        {
          weight = (globalDepots[i].size - 1) - j;
          p = new_preference(weight, "p" + to_string(weight));
          //p = new_preference((globalDepots[i].size - 1) - j, "p" + to_string((globalDepots[i].size - 1) - j - 1));
          (*os) << "\n\t(preference " + pref_name(p) + " (clear " + globalDepots[i].areas_from_door[j][m] + "))";
          if (weight > sequence)
            sequence = weight;
        }
        }
    }
    }


  if (constraints_pref)
    {
      if (at_most_once)
    pdiff++;

      if (sometime_before)
    pdiff++;
    }



  int w = sequence;
  if (constraints_pref)
    w += 3;
  //k = 0;
  gap = 1;
  (*os) << "\n";
  for (i = 1; i < totalDepotNumber; i++)
    {
      weight = w + gap;
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(w + gap, "p" + to_string(k + sequence));
      (*os) << "\n\t(forall (?c - crate)\n\t   (preference " + pref_name(p);
      (*os) << " (exists (?d - depot) (and (in ?c ?d) (not (= ?d " + globalDefDepot[i] + "))))))";
      //k++;
      w += gap;
      gap = globalDepots[i].size + 1;
      if (i == ((totalDepotNumber - 1)/ 2))
    weight_middle = weight;
      if (i == (totalDepotNumber - 1))
    weight_last = weight;
    }

  if (!constraints_pref)
    {

      (*os) << "\n";

      //k = 0;
      w = sequence;
      gap = 1;
      for (i = 1; i < totalDepotNumber; i++)
    {
      weight = w + gap;
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(w + gap, "p" + to_string(k + sequence));
      (*os) << "\n\t(forall (?h - hoist)";
      (*os) << "\n\t   (preference " + pref_name(p);
      (*os) << " (exists (?s - storearea ?d - depot)";
      (*os) << "\n\t\t\t      (and (at ?h ?s) (in ?s ?d) (not (= ?d " + globalDefDepot[i] +  "))))))";
      //k++;
      w += gap;
      gap = globalDepots[i].size + 1;
    }

      (*os) << "\n";
      weight = (int)ceil((float)weight_middle / 2);
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference((sequence + totalDepotNumber / 2) / 2, "p" + to_string((sequence + totalDepotNumber / 2) / 2));
      (*os) << "\n\t(forall (?c1 ?c2 - crate ?d - depot)\n\t   (preference " + pref_name(p);
      (*os) << " (imply (and (in ?c1 ?d) (in ?c2 ?d) (not (= ?c1 ?c2)))";
      (*os) << "\n\t\t\t          (compatible ?c1 ?c2))))";

      weight = (int)floor((float)weight_last / 2) + 1;
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference((int)ceil((float)(pweight[sequence + totalDepotNumber - 1] + 1) / 4),
      //         "p" + to_string((int)(ceil((float)(sequence + totalDepotNumber) / 4))));
      (*os) << "\n\n\t(forall (?c1 ?c2 - crate ?s1 ?s2 - storearea)\n\t   (preference " + pref_name(p);
      (*os) << " (imply (and (on ?c1 ?s1) (on ?c2 ?s2)";
      (*os) << "\n\t\t\t               (not (= ?c1 ?c2)) (connected ?s1 ?s2))";
      (*os) << "\n\t\t\t          (compatible ?c1 ?c2))))";


      (*os) << "\n";
      weight = pweight[sequence + totalDepotNumber - 2] + 1;//-1
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(pweight[sequence + totalDepotNumber - 1] + 1, "p" + to_string(sequence + totalDepotNumber));
      (*os) << "\n\t(forall (?d - depot ?s - storearea ?h - hoist)";
      (*os) << "\n\t   (preference " + pref_name(p);
      (*os) << " (imply (and (at ?h ?s) (in ?s ?d))";
      (*os) << "\n\t\t\t\t  (forall (?c - crate) (not (in ?c ?d))))))";
    }

}

int find_min_pref()
{
  int k, min, res;

  min = pweight[0];
  res = 0;

  for (k = 0; k < pnames.size(); k++)
    {
      if (pweight[k] < min)
    {
      res = k;
      min = pweight[k];
    }
    }

  return res;
}

void print_simple_preference_metric(ofstream *os)
{
  int j, k;
  bool first;

  first = true;
  (*os) << "\n\n(:metric minimize (+ ";

  while (pnames.size() > 0)
    {
      k = find_min_pref();

      for (j = 0; j < ptotal[k]; j++)
    {
      if (!first)
        (*os) << "\n\t\t     ";

      (*os) << "(* ";
      (*os) << pweight[k];
      (*os) << " (is-violated "+ pnames[k] + preference_symbol(j+1) + "))";

      first = false;
    }

      pnames[k] = pnames[pnames.size() - 1];
      pweight[k] = pweight[pnames.size() - 1];
      ptotal[k] = ptotal[pnames.size() - 1];
      pnames.pop_back();
    }

  (*os) << "))";
}

void create_constraints_pref(ofstream *os)
{
  int p = pnames.size();
  int time;
  int num;
  int diff = 0;
  int weight;

  if (totalCrateNumber == 1)
    diff = 1;

  (*os) << "\n\n(:constraints (and";

  if (!sometime_before)
    diff++;

  if (!at_most_once)
    diff++;

  if (totalCrateNumber > 1)
    {
      weight = (int)floor((float)weight_last / 2) + 1;
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(last_w + 1, "p" + to_string(sequence - diff + totalDepotNumber + 3));
      (*os) << "\n\t(forall (?c1 ?c2 - crate ?s1 ?s2 - storearea)";
      (*os) << "\n\t   (preference " + pref_name(p) +" (always (imply (and (on ?c1 ?s1) (on ?c2 ?s2)";
      (*os) << "\n\t\t\t\t\t        (not (= ?c1 ?c2)) (connected ?s1 ?s2))";
      (*os) << "\n\t\t\t\t\t   (compatible ?c1 ?c2)))))";

      weight = (int)ceil((float)weight_middle / 2);
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference((sequence - diff + 3 + totalDepotNumber / 2) * 2 - sequence + diff - 4, "p" +
      //         to_string(sequence - diff + 3 + totalDepotNumber / 2 - 1));
      (*os) << "\n\n\t(forall (?c1 ?c2 - crate ?d - depot)";
      (*os) << "\n\t   (preference " + pref_name(p) + " (always (imply (and (in ?c1 ?d) (in ?c2 ?d) (not (= ?c1 ?c2)))";
      (*os) << "\n\t\t\t\t\t  (compatible ?c1 ?c2)))))";
    }

  if (at_most_once)
    {
      weight = sequence + 2 - diff;
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(sequence + 2 - diff, "p" + to_string(sequence + 1 - diff));
      (*os) << "\n\n\t(forall (?c - crate)";
      (*os) << "\n\t   (preference " + pref_name(p);
      (*os) << " (at-most-once (exists (?h - hoist) (lifting ?h ?c)))))";
    }


  weight = sequence + 3 - diff;
  p = new_preference(weight, "p" + to_string(weight));
  //p = new_preference(sequence + 3 - diff, "p" + to_string(sequence + 2 - diff));
  (*os) << "\n\n\t(forall (?h - hoist)";
  (*os) << "\n\t   (preference " + pref_name(p);
  (*os) << " (sometime (exists (?c - crate) (lifting ?h ?c)))))";

  if (sometime_before)
    {
      if (totalCrateNumber > 1)
    {
      (*os) << "\n";
      num = rand() % (int)ceil((float)totalCrateNumber / 4) + 1;
      for (int i = 0; i < num; i++)
        {
          for (int j = num; j < totalCrateNumber; j++)
        {
          weight = sequence + 1;
          p = new_preference(weight, "p" + to_string(weight));
          //p = new_preference(sequence + 1, "p" + to_string(sequence));
          (*os) << "\n\t(preference " + pref_name(p) +" (sometime-before (exists (?d1 - depot) (in " + globalDefCrate[i] + " ?d1))";
          (*os) << "\n\t\t\t\t\t (exists (?d2 - depot) (in " + globalDefCrate[j] + " ?d2))))";
        }
        }
    }
    }

  time = 2 * (maxSaInDepot + 3) * ((int)floor((float)totalCrateNumber / totalHoistNumber) + 1)+ 2 * maxSaInDepot;
  if (temporal_constraints)
    {
      if (within)
    {
      weight = totalDepotNumber + 2;//+3
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(totalDepotNumber + 3, "p" + to_string(totalDepotNumber + 3));
      (*os) << "\n\n\t(forall (?c - crate)";
      (*os) << "\n\t   (preference " + pref_name(p) + " (within " +
        to_string(time) + " (exists (?d - depot) (in ?c ?d)))))";
    }

      if (always_within)
    {
      weight = totalDepotNumber + 1;//+2
      p = new_preference(weight, "p" + to_string(weight));
      //p = new_preference(totalDepotNumber + 2, "p" + to_string(totalDepotNumber + 2));
      (*os) << "\n\n\t(forall (?h - hoist)";
      (*os) << "\n\t   (preference " + pref_name(p) + " (always-within 3.5 (at ?h loadarea)";
      (*os) << "\n\t\t\t\t (exists (?a - storearea ?d - depot) (and (at ?h ?a) (in ?a ?d))))))";
    }
      /*
      p = new_preference(totalDepotNumber / 2, "p" + to_string(totalDepotNumber / 2));
      (*os) << "\n\n\t(forall (?c1 ?c2 - crate ?d - depot)";
      (*os) << "\n\t   (preference " + pref_name(p) + " (always-within 2.5 (and (in ?c1 ?d) (in ?c2 ?d) (not (compatible ?c1 ?c2)))";
      (*os) << "\n\t\t\t\t\t     (or (not (in ?c1 ?d)) (not (in ?c2 ?d))))))";
      */
    }


  (*os) << "))";

}


void create_constraints(ofstream *os)
{

  int crate_to_order[totalCrateNumber];
  int num_crate = 0;
  float time;

  (*os) << "\n\n(:constraints (and";

  (*os) << "\n\t(forall (?c - crate) (at end (exists (?d - depot) (in ?c ?d))))";

  if (at_most_once)
    {
      (*os) << "\n\n\t(forall (?c - crate) (at-most-once (exists (?h - hoist) (lifting ?h ?c))))";
    }

  if (sometime_before)
    {
      if (totalCrateNumber > 1)
    {
      (*os) << "\n";
      if (!derived && !goalStored)
        {

          num_crate = rand() % (int)ceil((float)totalCrateNumber / 4) + 1;

          for (int i = 0; i < num_crate; i++)
        {
          for (int j = num_crate; j < totalCrateNumber; j++)
            {
              (*os) << "\n\t(sometime-before (exists (?d1 - depot) (in " + globalDefCrate[i] + " ?d1))";
              (*os) << "\n\t\t\t (exists (?d2 - depot) (in " + globalDefCrate[j] + " ?d2)))";
            }
        }
        }
    }
    }


  if (within)
    {
      time = 2 * (maxSaInDepot + 3) * ((int)floor((float)totalCrateNumber / totalHoistNumber) + 1) + 2 * maxSaInDepot;
      (*os) << "\n\n\t(forall (?c - crate) (within " + to_string(time) + " (exists (?d - depot) (in ?c ?d))))";
    }

  if (always_within)
    {
      (*os) << "\n\n\t(forall (?h - hoist) (always-within 3.5 (at ?h loadarea)";
      (*os) << "\n\t\t\t\t\t (exists (?a - storearea ?d - depot) (and (at ?h ?a) (in ?a ?d)))))";
    }

  (*os) << "))";
}


void print_legenda(ofstream *os)
{
  int d;

  for (int i=0;i<globalDefDepot.size();i++)
    (*os) << "\n; " + to_string((char)DEPOT_SYMBOL(i)) + ": " + globalDefDepot[i] + " area";
  (*os) << "\n; *: Depot access point\n; =: Transit area";
}



void printOutput()
{

  string problem_string;
  string domain_string;

  ofstream out(filename);

  problem_string = "storage";
  domain_string = "Storage-Propositional";

  if (constraints)
    {
      if (numeric)
    domain_string = "Storage-MetricTimeConstraints";
      else if (durative)
    domain_string = "Storage-TimeConstraints";
    }
  else if (simple_pref && !constraints_pref)
    domain_string = "Storage-PropositionalSimplePreferences";
  else if (constraints_pref)
    {
      domain_string = "Storage-PropositionalPreferences";
      if (numeric)
    {
      domain_string = "Storage-MetricTimePreferences";
    }
      else if (durative)
    {
      domain_string = "Storage-TimePreferences";
    }
    }
  else
    {
      if (numeric)
    domain_string = "Storage-MetricTime";
      else if (durative)
    domain_string = "Storage-Time";
    }


  out <<"; Domain designed by Alfonso Gerevini and Alessandro Saetti\n; This file has been automatically generated by the generator available from\n; http://zeus.ing.unibs.it/ipc-5/generators/index.html\n;\n";

  out << "; Map of the Depots:";

  for (int i=0;i<globalMapString.size();i++)
    out << globalMapString[i];

  print_legenda(&out);

  if (problem_number < 0)
    out <<"\n\n(define (problem " + problem_string + ")";
  else
    out <<"\n\n(define (problem " + problem_string + "-" + to_string(problem_number) + ")";

  out <<"\n(:domain " + domain_string + ")";
  out <<"\n(:objects\n\t";

  for (int i=0;i<globalDefStorearea.size();i++)
    out << globalDefStorearea[i]+" ";
  for (int i=0;i<globalDefSc.size();i++)
    out << globalDefSc[i]+" ";
  out << "- storearea\n\t";



  for (int i=0;i<globalDefHoist.size();i++)
    out << globalDefHoist[i]+" ";
  out << "- hoist\n\t";

  for (int i=0;i<globalDefCrate.size();i++)
    out << globalDefCrate[i]+" ";
  out << "- crate\n\t";

  for (int i=0;i<globalDefContainer.size();i++)
    out << globalDefContainer[i]+" ";
  out << "- container\n\t";

  for (int i=0;i<globalDefDepot.size();i++)
    out << globalDefDepot[i]+" ";
  out << "- depot";

  if(!constraints)
    out << "\n\tloadarea ";

  if (globalDefCorridors.size() > 0)
    {
      if (constraints)
    out << "\n\t";
      for (int i=0;i<globalDefCorridors.size();i++)
    out << globalDefCorridors[i]+" ";
      out << "- transitarea";
    }
  else
    if (!constraints)
      out << "- transitarea";

  out << ")\n\n";
  out << "(:init";


  //CONNESSIONI

  for (int i=0;i<globalConnections.size();i++)
    out << "\n\t"+globalConnections[i];

  out << "";
  for (int i=0;i<globalDepotConnections.size();i++)
    out << "\n\t"+globalDepotConnections[i];

  out << "";
  for (int i=0;i<globalIns.size();i++)
    out << "\n\t" + globalIns[i];

  out << "";
  /*for (int i=0;i<totalCrateNumber;i++)
    {
    out << "\n  "+globalCrateOnContainer[i]+" ";
    out << "\n  "+globalCrateInContainer[i]+" ";
    out << "\n  "+globalScInContainer[i]+" ";
    out << "\n  "+globalLoadareaConnection[i]+" ";
    }*/
  for (int i=0;i<globalCrateOnContainer.size();i++)
    {
      out << "\n\t"+globalCrateOnContainer[i];
    }
  for (int i=0;i<globalCrateInContainer.size();i++)
    {
      out << "\n\t"+globalCrateInContainer[i];
    }
  for (int i=0;i<globalScInContainer.size();i++)
    {
      out << "\n\t"+globalScInContainer[i];
    }
  for (int i=0;i<globalLoadareaConnection.size();i++)
    {
      out << "\n\t"+globalLoadareaConnection[i];
    }

  out << "  ";
  for (int i=0;i<globalDoors.size();i++)
    out << "\n\t"+globalDoors[i];

  out << "  ";
  for (int i=0;i<globalClears.size();i++)
    out << "\n\t"+globalClears[i];

  out << "  ";
  for (int i=0;i<globalHoistAvailability.size();i++)
    out << "\n\t"+globalHoistAvailability[i];

  if(compatibilities)
    {
      out << "";
      for (int i=0;i<globalCompatibilities.size();i++)
    out <<"\n\t"+ globalCompatibilities[i];
    }

  if (timed||til)
    {out << "";
    for (int i=0;i<globalWeights.size();i++)
      out << "\n\t"+globalWeights[i];
    }

  if (numeric)
    {
      out << "";
      out << "\n\t(= (num-lift) 0)";
      for (int i=0;i<globalLoads.size();i++)
    out << "\n\t"+globalLoads[i];
    }

  if(til) {
    out << "";
    for (int i=0;i<globalTils.size();i++)
      out << "\n\t"+globalTils[i];
  }

  if (numeric) for (int i=0;i<globalNumCrate.size();i++)
    out << "\n\t"+globalNumCrate[i];


  out << ")";

  if (simple_pref || constraints_pref || !constraints)
    out << "\n\n(:goal (and";

  if (simple_pref || constraints_pref)
    {
      create_simple_preferences(&out);
    }
  else if (constraints)
    {
    }
  else
    {
      for (int i=0;i<globalGoals.size();i++)
    out << "\n\t"+globalGoals[i];
    }

  if (simple_pref || constraints_pref || !constraints)
    out << "))";

  if (constraints)
    {
      create_constraints(&out);
    }
  if (constraints_pref)
    {
      create_constraints_pref(&out);
    }

  if (simple_pref || constraints_pref)
    print_simple_preference_metric(&out);
  else
    {
      if (timed || til || constraints)
    {
      out<<"\n\n(:metric minimize (total-time))";
    }
      else if (numeric)
    {
      out<<"\n\n(:metric minimize (+ (* (- (num-lift) "+to_string(totalCrateNumber)+") " +
        to_string(totalCrateNumber) + ") (total-time)))";
    }
      else if (durative)
    {
      out<<"\n\n(:metric minimize (total-time))";
    }
    }
  out << "\n)\n";


}

//prints usage options to video
void usage()
{
  printf("Usage: gen-Storage [options] file");
  printf("\n\nCommand line options:\n");
  printf(" -h\t Display this help\n");
  printf(" -p\t Set the number of problem (header of the problem file)\n");
  printf(" -n\t Set the number of hoists (default 3)\n");
  printf(" -d\t Set the number of depots (default 1)\n");
  printf(" -o\t Set the number of containers (default 1)\n");
  printf(" -s\t Set the number of store-areas (default 9)\n");
  printf(" -c\t Set the number of crates (default 5)\n");
  printf(" -e\t Set seed for random number generator (must be different from 0)\n");
  /*
  printf("-X derived problem\n");
  printf("-T timed problem\n");
  printf("-N numeric problem\n");
  printf("-D problem for durative domain\n");
  printf("-L TIL problem (arrival times are random)\n");
  printf("-J goals are of 'in' type\n");
  printf("-K compatibilities are not generated\n");
  printf("-R depots are generated randomly\n");

  printf("--simple_pref Simple preferences\n");
  printf("--constraints_pref Constraints with preferences\n");
  printf("--constraints Simple constraints\n");
  printf("--metric-time-pref Temporal constraints with preferences\n");
  printf("--sometime-before Generate \"sometime-before\" constraints\n");
  printf("--at-most-once Generate \"at-most-once\" constraints\n");
  printf("--always-within Generate \"always-within\" constraints\n");
  printf("--within Generate \"within\" constraints\n");
  */
  //system("read");
}
