#ifndef RTREE_H
#define RTREE_H

#include <iostream>
#include <bits/stdc++.h> 
#include "file_manager.h"

using namespace std;

extern int maxCap,d,num_nodes, max_num_nodes;

extern int node_size;

struct Entry
{
    vector<int> minmbr;
    vector<int> maxmbr;
    int id;
    Entry(int i){
        minmbr =  vector<int>(d,INT_MIN);
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
    
    Node(){
        id=-1;
    }

    Node(int my_id, int parent) : id(my_id), parent_id(parent) { //dummy node just for size
        minmbr =  vector<int>(d,INT_MIN);
        maxmbr =  vector<int>(d,INT_MIN);
        Entry e(-1);
        for(int i=0;i<maxCap;i++){
            children.push_back(e); // all leaves
        }
        cout<<"in "<< children[0].id<<endl; 
    }
    ~Node(){
        minmbr.clear();
        maxmbr.clear();
        children.clear();

    };

};

class RTree
{
public:
    // RTree();
    // we will use initial root as 0 and leaf node children -1
    int root;
    // Node fetch(int id);

    RTree(int id, int maxChild,int dimension) : root(id)
    {
        maxCap = maxChild;
        d = dimension;

        node_size =  sizeof(Node(root,-1));
        
        max_num_nodes = floor(PAGE_CONTENT_SIZE/node_size);
        num_nodes=0;
    }
    void insert(vector<int> p, FileHandler& fh, FileManager& fm); // at the end - update root

    ~RTree(){};
    // void insert(vector<int> p, FileManager fm,FileHandler fh); // at the end - update root

};

#endif