#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "rtree.h"

using namespace std;


/*
storing node in page ( suppose given maxCap=K and Dimension=d ):

    nodeID, parentID,
    node_MBR_LB1, node_MBR_LB2, ... , node_MBR_LBd ,
    node_MBR_UP1, node_MBR_UB2, ... , node_MBR_UBd , 

    child1_ID, child2_ID, ... ,childK_ID ,

    child1_MBR_LB1, child1_MBR_LB2, ... , child1_MBR_LBd ,
    child1_MBR_UB1, child1_MBR_UB2, ... , child1_MBR_UBd ,

    child2_MBR_LB1, child2_MBR_LB2, ... , child2_MBR_LBd ,
    child2_MBR_UB1, child2_MBR_UB2, ... , child2_MBR_UBd ,
    .
    .
    .
    childK_MBR_LB1, childK_MBR_LB2, ... , childK_MBR_LBd ,
    childK_MBR_UB1, childK_MBR_UB2, ... , childK_MBR_UBd 



    TOTAL space used = (2K+3)d + 2
*/

int main(int argc, char const *argv[])
{
    if (argc != 4) {
        cout << "usage: ./rtree query.txt maxCap output.txt" << endl;
        return 1;
    }

    string inputFile = argv[1];
    int maxCap = stoi(argv[2]);
    string outputFile = argv[3];

    RTree tree(0, maxCap);
    std::ifstream infile(inputFile);
    string line;
    while (getline(infile, line))
    {
        istringstream iss(line);
        string op;
        iss >> op;
        if (op == "BULKLOAD") {
            continue;
        } else if (op == "INSERT") {
            vector<int> p;
            int px;
            while (iss >> px) {
                p.push_back(px);
            }
            tree.insert(p);
        } else if (op == "QUERY") {
            continue;
        }
    }

    return 0;
}

