#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "rtree.h"
#include <chrono> 
using namespace std::chrono; 

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

    ofstream outfile;
    outfile.open(outputFile);
     
    // ofstream custom_input("custom_input.txt");

	// Create a brand new file

	FileHandler fh = fm.CreateFile("temp.txt");
    int count=0;
    int queryCount=0;
    int insertCount=0; 
    while (getline(infile, line))
    {
        count++;
        istringstream iss(line);
        string op;
        iss >> op;
        if (op == "BULKLOAD") {
            // continue;
            outfile << "BULKLOAD"<<endl;
            outfile <<endl; 

            string infile ;
            int num;
            iss>> infile;
            iss>> num;

            FileHandler f = fm.OpenFile(infile.c_str());

            tree.bulkload(num,f,fh);

            fm.CloseFile(f);

        } else if (op == "INSERT") {
            // continue;
            insertCount++;
            outfile<<"INSERT"<<endl;
            outfile<<endl;

            vector<int> p;
            int px;
            while (iss >> px) {
                p.push_back(px);
            }

            tree.insert(p,fh);
          
        } else if (op == "QUERY") {
            queryCount++;
            vector<int> p;
            int px;

            while (iss >> px) {
                p.push_back(px);
            }
            bool print_comments = false;
            
            bool que = tree.query(p,fh,print_comments);
            if(que ){
                outfile <<"TRUE"<< endl;
                outfile<<endl;
            }
            else{
                outfile <<"FALSE"<<endl;
                outfile<<endl;
            }
        }
    }
    fm.CloseFile (fh);
    outfile.close();
    fm.DestroyFile("temp.txt");
    return 0;
}

