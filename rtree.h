#ifndef RTREE_H
#define RTREE_H

#include <iostream>
#include <bits/stdc++.h> 
#include "file_manager.h"

using namespace std;

extern int root_id, maxCap,d,num_nodes, max_num_nodes;
extern int fetch_time;

extern int node_size;

struct Entry
{
    vector<int> minmbr;
    vector<int> maxmbr;
    int id;
    Entry(){
        // minmbr =  vector<int>(d,INT_MAX);
        // maxmbr =  vector<int>(d,INT_MIN);
    }
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

class RTree
{
public:
    // RTree();
    // we will use initial root as 0 and leaf node children -1
    // int root;
    // Node fetch(int id, FileHandler& fh);

    RTree(int id, int maxChild,int dimension)
    {
        maxCap = maxChild;
        d = dimension;
        node_size =  (2*maxCap+2+maxCap*(2*d+1))*4; // 64 bytes 288
        
        max_num_nodes = floor(PAGE_CONTENT_SIZE/node_size);
        num_nodes=0;
    }
    void insert(const vector<int>& p, FileHandler& fh); // at the end - update root
    bool query(const vector<int>& p, FileHandler& fh,bool print_comments); // at the end - update root
    void bulkload(int num, FileHandler& fo, FileHandler& fh); // at the end - update root
    void print_t(int nodeID,FileHandler& fh);
    int ret_root_id(); 
    ~RTree(){};
    // void insert(vector<int> p, FileManager fm,FileHandler fh); // at the end - update root

};

#endif