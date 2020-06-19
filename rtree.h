#include <iostream>
#include <vector>
#include "file_manager.h"

using namespace std;

int maxCap = 0;

struct Entry
{
    vector<int> minmbr;
    vector<int> maxmbr;
    int id;
};

class Node
{
public:
    int id;
    vector<int> minmbr;
    vector<int> maxmbr;
    int parent_id;
    vector<Entry> children;

    Node(int my_id, int parent) : id(my_id), parent_id(parent) {}

};

class RTree
{
public:
    // we will use initial root as 0 and leaf node children -1
    int root;
    Node fetch(int id);

    void insert(vector<int> p); // at the end - update root
    RTree(int id, int maxChild) : root(id)
    {
        maxCap = maxChild;
    }
};