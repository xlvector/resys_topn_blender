#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

typedef map< int, float > ItemMap;
typedef vector< pair<int,float> > ItemList;
typedef map<int, ItemList> UserPreferenceList;

template < typename A, typename B >
bool GreaterSecond(const pair<A, B> & a, const pair<A,B> & b){
     return a.second > b.second;
}

float rand01(){
     return (float)(rand() % 1000000) / 1000000;
}

void combine(const vector< UserPreferenceList > & reco, const vector<float> & weight, UserPreferenceList & ret, int N){
     float sum_weight = 0;
     for(int i = 0; i < weight.size(); ++i) sum_weight += weight[i];
     for(UserPreferenceList::const_iterator u = reco[0].begin(); u != reco[0].end(); ++u){
          ItemMap ru;
          for(int k = 0; k < reco.size(); ++k){
               UserPreferenceList::const_iterator pku = reco[k].find(u->first);
               if(pku == reco[k].end()) continue;
               for(ItemList::const_iterator i = pku->second.begin(); i != pku->second.end(); ++i){
                    if(ru.find(i->first) == ru.end()) ru[i->first] = 0;
                    ru[i->first] += weight[k] * i->second;
               }
          }
          ItemList vru(ru.begin(), ru.end());
          sort(vru.begin(), vru.end(), GreaterSecond<int,float>);
          ret[u->first] = vru;
     }
}

float recall(const UserPreferenceList & reco, const UserPreferenceList & test, int N){
     float ret = 0;
     float n = 0;
     for(UserPreferenceList::const_iterator u = test.begin(); u != test.end(); ++u){
          n += (float)(u->second.size());
          UserPreferenceList::const_iterator pu = reco.find(u->first);
          if(pu == reco.end()) continue;
          map<int,float> ru(pu->second.begin(), pu->second.begin() + N);
          for(int i = 0; i < u->second.size(); ++i){
               if(ru.find(u->second[i].first) != ru.end()){
                    ++ret;
               }
          }
     }
     return ret / n;
}

void learningWeight(const vector< UserPreferenceList > & reco, vector< float > & weight, const UserPreferenceList & test, int N){
     srand(time(0));
     int K = reco.size();
     weight = vector<float>(K, 0);
     for(int k = 0; k < weight.size(); ++k){
          weight[k] = 0.2 + 0.8 * rand01();
     }
     for(int step = 0; step < 30; ++step){
          int i = rand() % K; //select the weight of algo we should change
          float w0 = weight[i];
          UserPreferenceList combine_ret;
          combine(reco, weight, combine_ret, N);
          float r0 = recall(combine_ret, test, N);
          if(rand01() > 0.7) weight[i] += 0.1 * (rand01() - 0.5);// change the weight of algo i
          else weight[i] = rand01();
          combine_ret.clear();
          combine(reco, weight, combine_ret, N);
          float r1 = recall(combine_ret, test, N);
          if(r1 < r0){ //if changing weight can not improve result, using previous weight
               weight[i] = w0;
          }
          cout << step << "\t" << max<float>(r0, r1) << endl;
     }
}

void loadUserPreferenceListBySimpleFormat(const char * file, UserPreferenceList & reco){
     //u1 i1 p1 i2 p2 ...
     //u2 i1 p1 i2 p2 ...
     ifstream in(file);
     string line;
     while(getline(in,line)){
          istringstream iss(line);
          int uid, iid;
          float preference;
          iss >> uid; //read userid
          int tn = 0;
          while(iss >> iid >> preference && ++tn < 100){ //read (itemid,preference) pair
               reco[uid].push_back(make_pair<int,float>(iid, preference));
          }
     }
     in.close();
}

void loadTestSet(const char * file, UserPreferenceList & test){
     ifstream in(file);
     string line;
     while(getline(in,line)){
          istringstream iss(line);
          int uid, iid;
          iss >> uid; //read userid
          while(iss >> iid){
               test[uid].push_back(make_pair<int,float>(iid, 1));
          }
     }
     in.close();
}

void loadUserPreferenceListByJSON(const char * file, UserPreferenceList & reco){
     
}

int main(int argc, char ** argv){
     ifstream in("algo.txt");
     string line;
     vector< UserPreferenceList > algos;
     vector< string > algoname;
     vector< float > weight;
     UserPreferenceList test;
     loadTestSet("test.txt", test);
     while(getline(in,line)){
          UserPreferenceList reco;
          loadUserPreferenceListBySimpleFormat(line.c_str(), reco);
          algos.push_back(reco);
          algoname.push_back(line);
          float rc = recall(reco, test, 10);
          cout << "algo / recall : " << line << "\t" << rc << endl;
     }
     learningWeight(algos, weight, test, 10); // learning weight of top-10 recommendation
     for(int i = 0; i < weight.size(); ++i){
          cout << algoname[i] << "\t" << weight[i] << endl;
     }
     return 0;
}
