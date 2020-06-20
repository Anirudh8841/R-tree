#include "rtree.h"

int maxCap=0 ,d=0,node_size=0,num_nodes=0,max_num_nodes=0;

void RTree::insert(vector<int> p, FileHandler& fh, FileManager& fm){
    //root node
    if(num_nodes==0){

        PageHandler ph = fh.NewPage();
        char *data = ph.GetData ();
        Node num(0,-1);

        memcpy (&data[0], &num.id, sizeof(int));
        memcpy (&data[4], &num.parent_id, sizeof(int));

        num.minmbr = p;
        num.maxmbr = p;
        
        int temp=8;
        for(int i=0;i<d;i++){
            memcpy (&data[temp], &num.minmbr[i], sizeof(int));
            temp=temp+4;
        }
        for(int i=0;i<d;i++){
            memcpy (&data[temp], &num.maxmbr[i], sizeof(int));
            temp=temp+4;
        }
        for(int i=0;i<maxCap;i++){
            memcpy (&data[temp], &num.children[i].id, sizeof(int));
            temp=temp+4;
        }
        num.children[0].minmbr = p;
        num.children[0].maxmbr = p;

        for(int i=0;i<maxCap;i++){
            for(int j=0;j<d;j++){
                memcpy (&data[temp], &num.children[i].minmbr[j], sizeof(int));
                temp=temp+4;
            }
            for(int j=0;j<d;j++){
                memcpy (&data[temp], &num.children[i].maxmbr[j], sizeof(int));
                temp=temp+4;
            }   
        }

        fh.FlushPages ();
        num_nodes++;

        // fm.CloseFile(fh);

    }
    else{
        int nn = num_nodes/max_num_nodes;
        PageHandler ph = fh.PageAt(nn);
        char *data = ph.GetData ();
        int nodes_in_page = num_nodes%max_num_nodes;
        int mem_offset = nodes_in_page*(node_size);
        
        if()
    }
    
}
// void RTree::insert(vector<int> p,FileManager fm,FileHandler fh){
    //root node
    // if(num_nodes==0){
        // Create a new page
        // PageHandler ph = fh.NewPage ();
        // char *data = ph.GetData ();

        // int num = 5;
	    // memcpy (&data[0], &num, sizeof(int));

        // fh.FlushPages ();
	    // cout << "Data written and flushed" << endl;

        // fm.CloseFile(fh);

       
    // }
    // else{

    // }
    // num_nodes++;
//     return;
// }