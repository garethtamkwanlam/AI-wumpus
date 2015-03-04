#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <sstream>
#include <iterator>
#include <stdlib.h>


using namespace std;


//Stench: “y” means you can smell the Wumpus in an adjacent cell
//Breeze: “y” means you can tell there is a pit in an adjacent cell
//Glitter: “y” means there is gold in the current cell.
//Bump: “y” means you tried to move forward but ran into a wall and did not move.
//Scream: “y” means you shot the Wumpus and it is now dead and no longer dangerous. You can still smell it if you are close enough.

const int WEST_I = 0;
const int EASTI = 2;
const int WESTI = 4;
const int SOUTHI = 3;
const int NORTHI = 1;
const int NORTH_I = 5;

const int HOME = 99;

const char EAST = 'E';
const char WEST = 'W';
const char SOUTH = 'S';
const char NORTH = 'N';

//vector<Room*> rooms;

const int WUMPUS = 99;
const int PIT = 100;
const int SAFE = 101;
const int BLOCK = 102;
const int MAYWUM = 103;
const int MAYDAN = 104;
const int UNKNOWN = 105;

int goHome = true;
int tracex = 0;
int tracey = 0;

const int range = 4;

int destinationx = -99;
int destinationy = -99;


char maindir = EAST;

queue<char> priorityactionqueue;
queue <string> actionqueue;
queue <vector<int> > sstepqueue;
queue <vector<int> > sdestqueue;
queue <char> parsequeue;

bool stench, breeze,glitter,bump,scream;

int roomprop[range][range];

int roommap[range][range];

bool checkfordone(){

  bool done = true;

  for ( int i = 0 ;i < range; i++) {
    for ( int j = 0; j < range; j++ ){
      if (roommap[i][j] == SAFE){
        done = false;
      }
    }
  }
  return done;
}

void given(){
  roommap[1][0] = SAFE;
  roommap[0][1] = SAFE;
  roommap[1][1] = SAFE;
}

bool deplicateDest(vector<int> vv){

  bool found = false;

  int xx = vv[0];
  int yy = vv[1];
  int zz = vv[2];

  for ( int i = 0 ; i < sdestqueue.size(); i++){

    vector<int> test = sdestqueue.front();

    sdestqueue.pop();
    if (test[0] == xx && test[1] == yy ){
      found = true;
    }
    //int te[] = { test[0], test[1], test[2] };
    vector<int> vv ;
    vv.push_back(test[0]);
    vv.push_back(test[1]);
    vv.push_back(test[2]);
    sdestqueue.push(vv);
  }
  return found;
}

char convertDirToChar(int dirr){
  char n ;
  if (dirr == 0 || dirr == 4){
    n = WEST;
  }else if ( dirr == 2){
    n = EAST;
  }else if ( dirr == 3) {
    n = SOUTH;
  }else if (dirr == 1 || dirr == 5){
    n = NORTH;
  }
  return n;
}

int convertCharToDir(char dirr){
  int n ;
  if (dirr == NORTH){
    n = NORTHI;
  }else if ( dirr == EAST){
    n = EASTI;
  }else if ( dirr == SOUTH) {
    n = SOUTHI;
  }else if (dirr == WEST){
    n = WESTI;
  }
  return n;
}

bool isblock(int x, int y){

  bool pass = false;
  if (roommap[x][y] == BLOCK){
    pass = true;
  }

  return pass;

}

bool maypit(int x, int y){

  bool pass = false;
  if (roommap[x][y] == MAYDAN){
    pass = true;
  }

  return pass;

}

bool maystench(int x, int y){

  bool pass = false;
  if (roommap[x][y] == MAYWUM){
    pass = true;
  }

  return pass;

}


int getabsoluate(int big, int small){

  int sbol;

  if (big >= small){
    sbol = big - small;
  }else {

    int mi = big - small;
    sbol = mi * -1;
  }

  return sbol;
}

void changeForward(char maindir, string output){

  if (output == "forward" ){

    if (maindir == NORTH){
      tracey = tracey + 1;
    }else if (maindir == EAST){
      tracex = tracex + 1;
    }else if (maindir == SOUTH){
      tracey = tracey - 1;
    }else if (maindir == WEST){
      tracex = tracex - 1;
    }

  }

  if (roommap[tracex][tracey] != HOME){
    roommap[tracex][tracey] = SAFE;
  }
  cout << "#  status on roommap[" << tracex << "][" << tracey << "] : " <<  roommap[tracex][tracey] << endl;
  cout << "#  tracex : " << tracex << " tracey : "  << tracey << endl;
}

bool checkstring(string s ){
  bool passed = false;
  //Regex regexf = "
  if (s.find("RL") == string::npos
  && s.find("LLL") == string::npos
  && s.find("LR") == string::npos
  && s.find("RRR") == string::npos
  && s.find("FRRF") == string::npos
  && s.find("FLLF") == string::npos
  && s.find("FRFRFRFR") == string::npos
  && s.find("FLFLFLFL") == string::npos
  // && s.find("FFRRFFFRRF") == string::npos
  //&& s.find("FFLLFFFLLF") == string::npos
  //&& s.find("FLFLF") == string::npos
){
  passed = true;
}
return passed;
}
void debugsstepQueue(){
  int qsize = sstepqueue.size();
  cout << " # sstep queue size : " << sstepqueue.size() << endl;
  int qtemp = qsize;
  string ii = " ";
  while (qtemp > 0){
    vector<int> c = sstepqueue.front();
    ostringstream s;
    int uu = c[0];
    int jj = c[1];
    int kk = c[2];
    s << "" << c[0] << " " << c[1] << " " << c[2];
    //std::string query(s.str());
    string temp = s.str();

    cout << " # temp sstep : " << temp << endl;
    ii += temp ;
    ii = ii + " ";
    int vv[] = { uu,jj,kk};
    vector<int> iiq;//(begin(vv), end(vv));
    iiq.push_back(uu);
    iiq.push_back(jj);
    iiq.push_back(kk);
    //vector<int> iiq (uu, jj, kk);
    sstepqueue.pop();
    sstepqueue.push(iiq);
    qtemp = qtemp - 1 ;
  }
}


void debugactionQueue(){
  int qsize = actionqueue.size();
  cout << " # action queue size : " << actionqueue.size() << endl;
  int qtemp = qsize;
  string ii = " ";
  while (qtemp > 0){
    string c = actionqueue.front();
    //std::string query(s.str());
    string temp = c;

    cout << " # temp action : " << temp << endl;
    ii += temp ;
    ii = ii + " ";
    //vector<int> iiq (uu, jj, kk);
    actionqueue.pop();
    actionqueue.push(temp);
    qtemp = qtemp - 1 ;
  }
}

void emptysdest(){

  int qsize = sdestqueue.size();
  //int qtemp2 = sdestqueue.size();
  while (qsize > 0){
    sdestqueue.pop();
    qsize = qsize - 1;
  }
}

void searchDest(){

  int atx = tracex;
  int aty = tracey;
  char atdir = maindir;

  int idir = convertCharToDir(atdir);

  cout << "# idir : " << idir << endl;
  cout << " # tracex : " << atx << endl;
  cout << " # tracey : " << aty << endl;

  int q[3] = {atx, aty, idir};
  std::vector<int> qint ;//( begin(q), end(q));
  qint.push_back(atx);
  qint.push_back(aty);
  qint.push_back(idir);

  // int xx1, yy1, dd1;

  sdestqueue.push(qint);

  bool run = true;

  cout << " # maindir is " << maindir << endl;

  int counter = 0;
  //bool once = true;

  while (run){

      int xx, yy, dd;

    vector<int> qdest = sdestqueue.front();
    xx = qdest[0];
    yy = qdest[1];
    dd = qdest[2];

    char cdd = convertDirToChar(dd);

    cout << " # pcurrent : " << xx << "," << yy << "," << dd << endl;


      //	cout << " # size of sstepqueue is " << sstepqueue.size() << endl;

    if (roommap[xx][yy] == SAFE){

      cout << " ## found destination " << endl;
      destinationx = xx;
      destinationy = yy;

      emptysdest();

      run = false;
      break;
    }else {

      sdestqueue.pop();

      if (dd == NORTH_I){
        dd = NORTHI;
      }else if (dd == WEST_I){
        dd = WESTI;
      }
      //F
      int fy  = yy;
      int fx = xx;
      if (dd == WESTI){
        fx = xx - 1;
      }else if (dd == EASTI){
        fx = xx + 1;
      }else if (dd == SOUTHI){
        fy = yy - 1;
      }else if ( dd == NORTHI){
        fy = yy + 1;
      }

    //	int vvf[] = { fx,fy,dd };
      vector<int> qfdest;//(begin(vvf), end(vvf));
      qfdest.push_back(fx);
      qfdest.push_back(fy);
      qfdest.push_back(dd);

      //R
      int dl = dd - 1;
      if (dl == NORTH_I){
        dl = NORTHI;
      }else if (dl == WEST_I){
        dl = WESTI;
      }

    //	int vvr[] = { xx,yy,dl};
      vector<int> qldest;//(begin(vvr), end(vvr));
      qldest.push_back(xx);
      qldest.push_back(yy);
      qldest.push_back(dl);
      //L
      int dr = dd + 1;
      if (dr == NORTH_I){
        dr = NORTHI;
      }else if (dr == WEST_I){
        dr = WESTI;
      }
    //	int vvl[] = { xx,yy,dr};
      vector<int> qrdest;//(begin(vvl), end(vvl));
      qrdest.push_back(xx);
      qrdest.push_back(yy);
      qrdest.push_back(dr);

      bool addforward, addright, addleft;

      if ( !deplicateDest(qfdest) && !maypit(qfdest[0],qfdest[1])  && !maystench(qfdest[0],qfdest[1]) && !isblock(qfdest[0], qfdest[1])){
      sdestqueue.push(qfdest);
      }

      if (!deplicateDest(qrdest)){
      sdestqueue.push(qrdest);
      }

      if (!deplicateDest(qldest)){
      sdestqueue.push(qldest);
      }
    }
  }


}
void updatedir ( char direction, string output){

  if (direction == EAST ) {
    if ( output == "right" ) {
      maindir = SOUTH;
    }else if (output == "left"){
      maindir = NORTH;
    }
  }

  else if (direction == SOUTH){
    if (output == "right"){
      maindir = WEST;
    } else if (output == "left"){
      maindir = EAST;
    }
  }

  else if (direction == WEST){
    if (output == "right"){
      maindir = NORTH;
    }else if (output == "left"){
      maindir = SOUTH;
    }
  }

  else if (direction == NORTH){
    if (output == "right"){
      maindir = EAST;
    }else if (output == "left"){
      maindir = WEST;
    }

  }

}
void emptysset(){

  int qsize = sstepqueue.size();
  int qtemp2 = sstepqueue.size();
  while (qtemp2 > 0){
    sstepqueue.pop();
    qtemp2 = qtemp2 - 1;
  }
}



void emptyact(){

  int qsize = actionqueue.size();
  int qtemp2 = actionqueue.size();
  while (qtemp2 > 0){
    actionqueue.pop();
    qtemp2 = qtemp2 - 1;
  }
}

bool deplicate(vector<int> vv){

  bool found = false;

  int xx = vv[0];
  int yy = vv[1];
  int zz = vv[2];

  for ( int i = 0 ; i < sstepqueue.size(); i++){

    vector<int> test = sstepqueue.front();

    sstepqueue.pop();

    if (test[0] == xx && test[1] == yy && test[2] == zz){

      found = true;

    }

    //int te[] = { test[0], test[1], test[2] };
    vector<int> vv ;//(begin(te), end(te));
    vv.push_back(test[0]);
    vv.push_back(test[1]);
    vv.push_back(test[2]);
    sstepqueue.push(vv);

  }

  return found;


}

void searchPath(){

  int atx = tracex;
  int aty = tracey;
  char atdir = maindir;

  int idir = convertCharToDir(atdir);

  cout << " # idir : " << idir << endl;

  std::vector<int> qint ;//( begin(q), end(q));
  qint.push_back(atx);
  qint.push_back(aty);
  qint.push_back(idir);

  // int xx1, yy1, dd1;

  sstepqueue.push(qint);
  //
  // destinationx = 1;
  // destinationy = 1;

   if ( (destinationx == -99 && destinationy == -99 )
   || priorityactionqueue.size() == 0 || parsequeue.size() == 0){
   	cout << " # destination reset " << endl ;
  // 	//	searchDest();
   	searchDest();
   }

  while (true){

    int xx, yy, dd;

    vector<int> qaction = sstepqueue.front();
    xx = qaction[0];
    yy = qaction[1];
    dd = qaction[2];


    // if (print == true){
    // 	cout << " # dd " << qaction[2] << endl;
    // 	print = false;
    // }
    char cdd = convertDirToChar(dd);

    cout << " # ppath : " << xx << "," << yy << "," << dd << endl;


    //	cout << " # size of sstepqueue is " << sstepqueue.size() << endl;

    if (xx == destinationx && yy == destinationy){

      cout << " ## found destination " << endl;
      if (actionqueue.size() != 0 && actionqueue.front() != ""){
        string opath = actionqueue.front();
        cout << " # found the path " << opath << endl;
        int strsize = opath.size();
        int strstemp = strsize;
        for ( int i = 0; i < strsize; i++){
          parsequeue.push(opath.at(i));
        }
      }
      emptyact();
      emptysset();
      break;
    }else {
    sstepqueue.pop();

    string opath = "";
    //char ba;
    if ( actionqueue.size() != 0 && actionqueue.front() != ""){
      opath = actionqueue.front();
      actionqueue.pop();
    }




    if (dd == NORTH_I){
      dd = NORTHI;
    }else if (dd == WEST_I){
      dd = WESTI;
    }
    //F
    int fy  = yy;
    int fx = xx;

    if (dd == WESTI){
      fx = xx - 1;
    }else if (dd == EASTI){
      fx = xx + 1;
    }else if (dd == SOUTHI){
      fy = yy - 1;
    }else if ( dd == NORTHI){
      fy = yy + 1;
    }
    if(fy >= 0 && fx >= 0){

      vector<int> qf;//(begin(vvf), end(vvf));
      qf.push_back(fx);
      qf.push_back(fy);
      qf.push_back(dd);

      string f = "F";
      string fpath = opath + f;

      if ( !deplicate(qf) && !maypit(qf[0],qf[1])  && !maystench(qf[0],qf[1])  && !isblock(qf[0], qf[1]) && checkstring(fpath)){
        sstepqueue.push(qf);
        actionqueue.push(fpath);
      }
    //	 cout << " # fpath : " << fx << "," << fy << "," << dd << endl;
    }

      //R
      int dl = dd - 1;
      if (dl == NORTH_I){
        dl = NORTHI;
      }else if (dl == WEST_I){
        dl = WESTI;
      }


      //int vvr[] = { xx,yy,dl};
      if (xx >= 0 && yy >= 0){
      vector<int> ql;//(begin(vvr), end(vvr));
      ql.push_back(xx);
      ql.push_back(yy);
      ql.push_back(dl);
      //std::vector<int> ql (fx, fy, dl);

      string l = "L";
      string lpath = opath + l;
      lpath = opath + l;
      if (!deplicate(ql) && !maypit(ql[0],ql[1])  && !maystench(ql[0],ql[1])  && !isblock(ql[0], ql[1]) && checkstring(lpath)){
        sstepqueue.push(ql);
        actionqueue.push(lpath);

    //	cout << " # lpath : " << fx << "," << fy << "," << dl << endl;
      }


      cout << " #  get to here " << endl;
      //L
      string r= "R";
      int dr = dd + 1;
      if (dr == NORTH_I){
        dr = NORTHI;
      }else if (dr == WEST_I){
        dr = WESTI;
      }
      //int vvl[] = { xx,yy,dr};
      vector<int> qr;//(begin(vvl), end(vvl));
      qr.push_back(xx);
      qr.push_back(yy);
      qr.push_back(dr);

      string rpath = opath + r;

      rpath = opath + r;
      if (!deplicate(qr) && !maypit(qr[0],qr[1])  && !maystench(qr[0],qr[1])  && !isblock(qr[0], qr[1]) && checkstring(rpath)){
        sstepqueue.push(qr);
        actionqueue.push(rpath);
      }

    //	cout << " # rpath : " << fx << "," << fy << "," << dr << endl;
    }

      // cout << " # out fpath " << fpath << endl;;
      // cout << " # out rpath  " << rpath << endl;
      // cout << " # out lpath " << lpath << endl;



    }
  }
}



  void populatePriorityQueue(){

    if (parsequeue.size() == 0){
      cout << " # nothing I can get, searching for the Path " << endl;
      searchPath();
    }

    cout << " # there are " << parsequeue.size() << " item in the queue " << endl;

    char first, second, third;

    if (parsequeue.size() >= 3){
      first = parsequeue.front();
      parsequeue.pop();
      second = parsequeue.front();
      parsequeue.pop();
      third = parsequeue.front();
      parsequeue.pop();
    }else if (  parsequeue.size() == 2){
      first = parsequeue.front();
      parsequeue.pop();
      second = parsequeue.front();
      parsequeue.pop();

    }else if ( parsequeue.size() == 1){
      first = parsequeue.front();
      parsequeue.pop();
    }

    cout << " # first is " << first << " second is " << second << " third is " << third << endl;

    priorityactionqueue.push(first);
    priorityactionqueue.push(second);
    priorityactionqueue.push(third);

  }

  int main(){
    string output;
    cout << " # maindirection " << maindir << endl; //assuming that every is maindir
    //set everybox to be unknown
    for ( int s = 0 ; s < 40; s++){
      for (int g = 0; g< 40; g++){
        roommap[s][g] = UNKNOWN;
      }
    }

    bool init = true;
    char rl, rf, rr, dt, h;

    roommap[0][0] = HOME;

    while(1){

      string str;
      string output;

      getline(cin, str);

      rl = str[0];
      rf = str[2];
      rr = str[4];
      dt = str[6];
      h = str[8];

      if (rl == 'y') {stench = true;} else { stench= false;};
      if (rf == 'y') {breeze = true;} else { breeze = false;};
      if (rr == 'y') {glitter = true;} else { glitter = false;};
      if (dt == 'y') {bump = true;} else { bump = false;};
      if (h == 'y')  {scream = true;} else { scream = false;};


      /*-----------------------------------------Update KB-----------------------------------------------------*/

      cout << "#  breeze : " << breeze << " bump : " << bump << "stench : " << stench << endl;

      given();

      int yy = tracey + 1;
      int xx = tracex + 1;
      int xm = tracex - 1;
      int ym = tracey - 1;

      cout << "# maindir is " << maindir << endl;

      if (stench){

        string output ;

        if (roommap[tracex][yy] > BLOCK){
          roommap[tracex][yy] = MAYWUM;
        }

        if (roommap[tracex][ym] > BLOCK){
          roommap[tracex][ym] = MAYWUM;
        }

        if (roommap[xx][tracey] > BLOCK){
          roommap[xx][tracey] = MAYWUM;
        }

        if (roommap[xm][tracey] > BLOCK){
          roommap[xm][tracey] = MAYWUM;
        }

        if (maindir == NORTH ){
          if (roommap[tracex][yy] == MAYWUM
            && roommap[tracex][ym] != MAYWUM && roommap[xx][tracey] != MAYWUM && roommap[xm][tracey] != MAYWUM){
              output = "shoot";
              cout <<  output << endl;
              continue;
            }
          }else if (maindir == SOUTH){
            if (roommap[tracex][ym] == MAYWUM
              && roommap[tracex][yy] != MAYWUM && roommap[xx][tracey] != MAYWUM && roommap[xm][tracey] != MAYWUM){
                output = "shoot";
                cout <<  output << endl;
                continue;
              }
            }else if (maindir == WEST){
              if (roommap[xm][tracey] == MAYWUM
                && roommap[tracex][yy] != MAYWUM && roommap[xx][tracey] != MAYWUM && roommap[tracex][yy] != MAYWUM){
                  output = "shoot";
                  cout <<  output << endl;
                  continue;
                }

              }else if (maindir == EAST){
                if (roommap[xx][tracey] == MAYWUM
                  && roommap[tracex][yy] != MAYWUM && roommap[xm][tracey] != MAYWUM && roommap[tracex][yy] != MAYWUM){
                    output = "shoot";
                    cout <<  output << endl;
                    continue;
                  }
                }

              }

              if (glitter){
                cout << " #  3 enter in else statement " << endl;
                cout << "Grab" << endl;
                continue;
              }

              if (bump){

                /* encounter a wall, then turn right*/
                if (maindir == NORTH){
                  roommap[tracex][yy] = BLOCK;
                }else if (maindir == EAST){
                  roommap[xx][tracey] = BLOCK;
                }else if (maindir == SOUTH){
                  roommap[tracex][ym] = BLOCK;
                }else if (maindir == WEST){
                  roommap[xm][tracey] = BLOCK;
                }
                string output = "right";
                cout << output << endl;
                continue;
              }
              if (breeze){

                if (roommap[tracex][yy] > BLOCK){
                  roommap[tracex][yy] = MAYDAN;
                }
                if (roommap[tracex][ym] > BLOCK){
                  //cout << "#it is true" << endl;
                  roommap[tracex][ym] = MAYDAN;
                }
                if (roommap[xx][tracey]  > BLOCK ){
                  roommap[xx][tracey] = MAYDAN;
                }
                if (roommap[xm][tracey]  > BLOCK ){
                  roommap[xm][tracey] = MAYDAN;
                }


              }else if (!stench && !breeze){

                cout << " # nothing happen " << endl;

                cout << " # tracex[" << tracex << "][" << tracey << "] is SAFE " << endl;

                if (roommap[tracex][yy] > BLOCK){
                  roommap[tracex][yy] = SAFE;
                }

                if (roommap[tracex][ym] > BLOCK){
                  roommap[tracex][ym] = SAFE;
                }

                if (roommap[xx][tracey] > BLOCK){
                  roommap[xx][tracey] = SAFE;
                }

                if (roommap[xm][tracey] > BLOCK){
                  roommap[xm][tracey] = SAFE;
                }
              }
              cout << "#  NORTH status on roommap[" << tracex << "][" << yy << "] : " <<  roommap[tracex][yy] << endl;
              cout << "#  SOUTH status on roommap[" << tracex << "][" << ym << "] : " <<  roommap[tracex][ym] << endl;
              cout << "#  EAST on roommap[" << xx << "][" << tracey << "] : " <<  roommap[xx][tracey] << endl;
              cout << "#  WEST on roommap[" << xm << "][" << tracey << "] : " <<  roommap[xm][tracey] << endl;


              if (priorityactionqueue.size() == 0){
            //		cout << " # nothing come up, populating the priority Queue" << endl;
                populatePriorityQueue();
              }
              char re = priorityactionqueue.front();
              if (re == 'R'){
                output = "right";
                updatedir(maindir, output);
              }else if (re == 'L'){
                output = "left";
                updatedir(maindir, output);
              }else {
                output = "forward";
              }
              cout << " # size of the priorityactionqueue is " << priorityactionqueue.size() << endl;
              priorityactionqueue.pop();
              changeForward(maindir, output);
              cout << output << endl;
              continue;

            }
          }
