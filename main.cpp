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
    if (argc != 5) {
        cout << "usage: ./rtree query.txt maxCap d output.txt" << endl;
        return 1;
    }

    string inputFile = argv[1];
    int maxCap = stoi(argv[2]);
    int dimension = stoi(argv[3]);
    string outputFile = argv[4];

    RTree tree(0, maxCap,dimension);
    std::ifstream infile(inputFile);
    
    string line;

    FileManager fm;
	// Create a brand new file
	FileHandler fh = fm.CreateFile("temp.txt");
    // fm.CloseFile(fh);
	// cout << "File created " << endl;
	
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
            tree.insert(p,fh,fm);

            // FileManager fm;
            // tree.insert(p, fm, fh);

            // Reopen the same file, but for reading this time
            // FileHandler fh = fm.OpenFile ("temp.txt");
            // cout << "File opened" << endl;

            // Get the very first page and its data
            // PageHandler ph = fh.FirstPage ();
            // char *data = ph.GetData ();

            // cout<<"done"<<endl;
            // Node n(0,-1);
            // cout<<" "<< sizeof(n)<<endl;
            // // Output the first integer
            // // cout<< sizeof(n)<<endl;
            // int n;
            // memcpy (&n, &data[0], sizeof(n));
            // cout << "First number: " << n.id <<" " << endl;
            // fm.CloseFile(fh);
	        // fm.DestroyFile ("temp.txt");

        } else if (op == "QUERY") {
            continue;
        }
    }

    return 0;
}

