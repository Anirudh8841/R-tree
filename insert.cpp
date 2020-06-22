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
// vector<int> fetch(int i,Node nn){

//     Node n(5,-1);
//     nn.id = n.id;
//     return n;
// }
// void insertPoint()
int main(){

    Node currNode(10,-1);
    // Node n1 = fetch(1,n);
    int count=0;
    for(Entry child : currNode.children){

        cout<<"id "<< count<<endl;
        count++;

        if(child.id==currNode.id){

            for(int i=0;i<d;i++){
                child.minmbr[i] = min(child.minmbr[i], currNode.minmbr[i]);
                child.maxmbr[i] = max(child.maxmbr[i], currNode.maxmbr[i]);
            }

            break;
        }
    }
    return 0;
}