#include<iostream>
#include <bits/stdc++.h> 

using namespace std;

int maxCap =10,d=2;

struct Entry
{
    vector<int> minmbr;
    vector<int> maxmbr;
    int id;
    
    Entry(int i){
        minmbr =  vector<int>(d,INT_MAX);
        maxmbr =  vector<int>(d,INT_MIN);
        id=i;
    }
    ~Entry(){
        minmbr.clear();
        maxmbr.clear();
    };
};

class Node
{
public:
    int id;
    vector<int> minmbr;
    vector<int> maxmbr;
    int parent_id;
    vector<Entry> children;
    

    Node(int my_id, int parent) : id(my_id), parent_id(parent) { //dummy node just for size
        minmbr =  vector<int>(d,INT_MAX);
        maxmbr =  vector<int>(d,INT_MIN);
        Entry e(-1);
        for(int i=0;i<maxCap;i++){
            children.push_back(e); // all leaves
        }
        // cout<<"in "<< children[0].id<<endl; 
    }
    ~Node(){
        minmbr.clear();
        maxmbr.clear();
        children.clear();

    };

};
Node fetch(int i,Node& nn){

    Node n(5,-1);
    nn.id = n.id;
    return n;
}
// void insertPoint()
int main(){

   for(int i=0;i<100;i++){
        cout << "INSERT "<< i+1 << " "<<i+2<<endl;
        cout << "QUERY "<< i+1 << " "<<i+2<<endl;
   }
}