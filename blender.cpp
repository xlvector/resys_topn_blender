#include <vector>
#include <map>
#include <string>
#include <fstream>
using namespace std;

typedef map< int, float > ItemMap;
typedef vector< pair<int,float> > ItemList;
typedef map<int, ItemList> UserPreferenceList;

template < typename A, typename B >
bool GreaterSecond(const pair<A, B> & a, const pair<A,B> & b){
     return a.second > b.second;
}

void combine(const vector< ItemMap > & reco, const vector< float > & weight, ItemMap & ret){
     int K = reco.size();
     float sum = 0;
     for(int k = 0; k < K; ++k){
          sum += weight[k];
          for(ItemMap::const_iterator i = reco[k].begin(); i != reco[k].end(); ++i){
               if(ret.find(i->first) == ret.end()) ret[i->first] = 0;
               ret[i->first].likeness += weight[k] * i->second;
          }
     }
     for(ItemMap::iterator i = ret.begin(); i != ret.end(); ++i)
          i->second /= sum;
}

void combine(const vector< ItemMap > & reco, const vector< float > & weight, ItemList & ret, int N){
     ItemMap retmap;
     combine(reco, weight, retmap);
     ret = ItemList(retmap.begin(), retmap.end());
     sort(ret.begin(), ret.end(), GreaterSecond<int,float>);
}

float recall(const UserPreferenceList & reco, const UserPreferenceList & test){
     float ret = 0;
     float n = 0;
     for(UserPreferenceList::const_iterator u = test.begin(); u != test.end(); ++u){
          map<int,float> ru = map<int,float>(reco[u->first].begin(), reco[u->first].end());
          for(int i = 0; i < u->second.size(); ++i){
               if(ru.find(u->second[i].first) != ru.end()){
                    ++ret;
               }
          }
          n += (float)(u->second.size());
     }
     return ret / n;
}
