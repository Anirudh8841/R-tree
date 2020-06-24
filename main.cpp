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

// string inputFile ="Testcases/TC_1/queries_2_10_100_100.txt" ;
//     int maxCap = 10;
//     int dimension = 2;
//     string outputFile = "output_tc1.txt";

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
    int count=0;
	// fm.PrintBuffer();
    while (getline(infile, line))
    {
        count++;
        istringstream iss(line);
        string op;
        iss >> op;
        if (op == "BULKLOAD") {
            cout<<"BULKLOAD"<<endl;
            cout<<endl;
            // continue;
            string infile ;
            int num;
            iss>> infile;
            iss>> num;

            // cout<< num << " st "<<infile<< endl;

            FileHandler f = fm.OpenFile(infile.c_str());

            tree.bulkload(num,f,fh);
            

            fm.CloseFile(f);

            // tree.print_t(tree.ret_root_id(),fh);
            // fm.ClearBuffer();
            // fm.PrintBuffer();
            // f.CloseFile(infile.c_str());
            

        } else if (op == "INSERT") {
            // count++;
            if(count>1000){
                cout<<"INSERT"<<endl;
                cout<<endl;

            }
            vector<int> p;
            int px;
            while (iss >> px) {
                p.push_back(px);
            }

            tree.insert(p,fh);
            if(count==807){
                // cout<<"in c100"<<endl;
                // tree.print_t(tree.ret_root_id(),fh);
            }
           
          
        } else if (op == "QUERY") {
            vector<int> p;
            int px;
            while (iss >> px) {
                p.push_back(px);
            }
            bool print_comments = false;
            // if(count==1059){
            //     print_comments=true;
            // }
            bool que = tree.query(p,fh,print_comments);
            if(que ){
                cout <<"TRUE"<< endl;
                cout<<endl;
            }
            else{
                cout <<"FALSE"<<endl;
                cout<<endl;
            }
        }
    }
    fm.CloseFile (fh);
    fm.DestroyFile("temp.txt");

    return 0;
}

