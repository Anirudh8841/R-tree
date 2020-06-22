#include "rtree.h"

int root_id=-1, maxCap=0 ,d=0,node_size=0,num_nodes=0,max_num_nodes=0;


Node fetch(int id,FileHandler& fh){

    int pagenum = floor(id/max_num_nodes);
    int offset = id%max_num_nodes;

    PageHandler ph = fh.PageAt(pagenum);
    char *data = ph.GetData ();
    Node node(0,-1);

    root_id=0;
    memcpy (&node.id,&data[offset], sizeof(int));
    memcpy (&node.parent_id,&data[offset+4], sizeof(int));

    int temp=8+offset;
    for(int i=0;i<d;i++){
        memcpy ( &node.minmbr[i],&data[temp], sizeof(int));
        temp=temp+4;
    }
    for(int i=0;i<d;i++){
        memcpy (&node.maxmbr[i], &data[temp],  sizeof(int));
        temp=temp+4;
    }
    for(int i=0;i<maxCap;i++){
        memcpy ( &node.children[i].id, &data[temp], sizeof(int));
        temp=temp+4;
    }

    for(int i=0;i<maxCap;i++){
        for(int j=0;j<d;j++){
            memcpy (&node.children[i].minmbr[j], &data[temp],  sizeof(int));
            temp=temp+4;
        }
        for(int j=0;j<d;j++){
            memcpy (&node.children[i].maxmbr[j], &data[temp],  sizeof(int));
            temp=temp+4;
        }   
    }
    return node;    
}

void save(Node node,FileHandler& fh){

    int pagenum = floor(node.id/max_num_nodes);
    int offset = node.id%max_num_nodes;

    PageHandler ph = fh.PageAt(pagenum);
    char *data = ph.GetData ();
    // Node node(0,-1);

    // root_id=0;
    memcpy (&data[offset],&node.id, sizeof(int));
    memcpy (&data[offset+4], &node.parent_id, sizeof(int));

    int temp=8+offset;
    for(int i=0;i<d;i++){
        memcpy ( &data[temp], &node.minmbr[i], sizeof(int));
        temp=temp+4;
    }
    for(int i=0;i<d;i++){
        memcpy (&data[temp],  &node.maxmbr[i], sizeof(int));
        temp=temp+4;
    }
    for(int i=0;i<maxCap;i++){
        memcpy ( &data[temp],  &node.children[i].id, sizeof(int));
        temp=temp+4;
    }

    for(int i=0;i<maxCap;i++){
        for(int j=0;j<d;j++){
            memcpy (&data[temp],  &node.children[i].minmbr[j],  sizeof(int));
            temp=temp+4;
        }
        for(int j=0;j<d;j++){
            memcpy (&data[temp],  &node.children[i].maxmbr[j],  sizeof(int));
            temp=temp+4;
        }   
    }
    return ;    
}

long double calculateVolume(const vector<int> &minmbr, const vector<int>& maxmbr){
    long double vol =1;
    for (int i = 0; i < minmbr.size(); i++)
    {
        int lb = minmbr[i];
        int ub = maxmbr[i];
        if(lb==INT_MIN){
            break;
        }
        vol = vol*(ub-lb);
    }
    return vol;
}


void updateParentOnDisk(int nodeID, int new_parentID, FileHandler& fh ){
    if(nodeID=-1){
        return;
    }  
    Node node = fetch(nodeID,fh);
    node.parent_id = new_parentID;
    save(node,fh);
}

void simpleBubbleUpToTheRoot(Node currNode, FileHandler& fh){
    if(currNode.parent_id==-1){
        //no parent so no need to do anything
        return;
    }

    Node parentNode = fetch(currNode.parent_id,fh);

    //update this parentNode's MBR
    for(int i=0;i<d;i++){
        parentNode.minmbr[i] = min(parentNode.minmbr[i], currNode.minmbr[i]);
        parentNode.maxmbr[i] = max(parentNode.maxmbr[i], currNode.maxmbr[i]);
    }

    //update the MBR of that child in parent children list which matches currNode's id
    for(Entry child : parentNode.children){
        if(child.id==currNode.id){

            for(int i=0;i<d;i++){
                child.minmbr[i] = min(child.minmbr[i], currNode.minmbr[i]);
                child.maxmbr[i] = max(child.maxmbr[i], currNode.maxmbr[i]);
            }

            break;
        }
    }


    save(parentNode,fh); //since parentNode is changed so update it in page
    simpleBubbleUpToTheRoot(parentNode,fh); //recurse
}
void addChild(Node& currNode, int childID,const vector<int> &child_minmbr,const vector<int> &child_maxmbr, FileHandler& fh){
    //calculate the count of valid children in currNode
    int valid_children_count = 0;
    int invalid_childIndex= -1;
    for(int i=0;i<currNode.children.size();i++){
        Entry child = currNode.children[i];
        if (child.minmbr[0] == INT_MAX && child.maxmbr[0]==INT_MIN) {
            //this is not a valid child he's just a place filler
            invalid_childIndex = i;
            break;
        }
        valid_children_count++;
    }

    //create newChild_entry
    Entry newChild_entry(childID); 

    newChild_entry.minmbr  = child_minmbr;
    newChild_entry.maxmbr = child_maxmbr;

    if(valid_children_count<maxCap){
        //so no splitting happens simple insert happens

        //insert the newChild_entry
        currNode.children[invalid_childIndex] = newChild_entry;

        //since currNode is changed so update it in the page
        save(currNode,fh); //creation a  function which fetches the page using currNode's id and replaces it with updated currNode
        simpleBubbleUpToTheRoot(currNode,fh); //start from this node and update till the root

    }
    else if(valid_children_count==maxCap){
        //splitting happens here
        Node L1 (currNode.id, currNode.parent_id); //L1 is basically updated currentNode
        Node L2 (num_nodes, currNode.parent_id); //L2 is the new created split node
        num_nodes = num_nodes + 1; //since a new node is created
        vector<Entry> E_set = currNode.children;
        E_set.push_back(newChild_entry);
        //E_set vector is overflowing because of maxCap breach

        //PICKSEED algo.
        //In the E_set select those two entries which are farthest in distance 
        //lets calculate euclidean distance using center points in each dimension
        long double maxDistance = -1;
        Entry e1;
        Entry e2;
        for(int i=0; i< E_set.size(); i++){
            for(int j=i+1; j<E_set.size(); j++){
                Entry child1 = currNode.children[i];
                Entry child2 = currNode.children[j];

                if(child2.minmbr[0]==INT_MAX && child2.maxmbr[0]==INT_MIN){
                    continue; //invalid child found so skip
                }

                if(child1.minmbr[0]==INT_MAX && child1.maxmbr[0]==INT_MIN){
                    break; //invalid child and the upcoming ones will be invalid too as j>i
                }

                long double euclidDist = 0.0;
                for(int k=0; k<d; k++){
                    //in each dimension get the midPoint of the two children and use it for euclidean distance
                    double midPoint1 = (child1.minmbr[k]+child1.maxmbr[k])/2;
                    double midPoint2 = (child2.minmbr[k]+child2.maxmbr[k])/2;
                    euclidDist += (midPoint1-midPoint2)*(midPoint1-midPoint2);
                }

                if(euclidDist>maxDistance){
                    maxDistance = euclidDist;
                    e1 = child1;
                    e2 = child2;
                }
            }
        }

        //now e1 gets added to L1 and e2 gets added to L2

        vector<Entry> group1; //empty vector
        vector<Entry> group2; //empty vector

        group1.push_back(e1);
        group2.push_back(e2);

        //update MBR of L1 and L2
        incorporate_child_mbr(L1,e1);
        incorporate_child_mbr(L2,e2);

        //go over to entries of the E_set and assign them to the group1/L1 or group2/L2
        //on the basis of which group causes least increase in volume
       
    //     for(Entry child: E_set){
    //         if(child==e1 || child==e2){
    //             continue; //already added these two
    //         }

    //         long double childVol = calculateVolume(child.minmbr, child.maxmbr);
    //         long double newVol1 = calculateVolume( generateMinimum(child.minmbr, L1.minmbr), 
    //                                                generateMaximum(child.maxmbr, L1.maxmbr) );

    //         long double newVol2 = calculateVolume( generateMinimum(child.minmbr, L2.minmbr), 
    //                                                generateMaximum(child.maxmbr, L2.maxmbr) );

    //         long double increase1 = abs(newVol1-childVol);
    //         long double increase2 = abs(newVol2-childVol);

    //         if(increase1<increase2){
    //             group1.push_back(child);
    //             incorporate_child_mbr(L1,child);
    //         }
    //         else if(increase1>increase2){
    //             group2.push_back(child);
    //             incorporate_child_mbr(L2,child);
    //         }
    //         else{
    //             //tie breaker case 1
    //             long double L1Vol = calculateVolume(L1.minmbr, L1.maxmbr);
    //             long double L2Vol = calculateVolume(L2.minmbr, L2.maxmbr);
    //             if(L1Vol<L2Vol){
    //                 group1.push_back(child);
    //                 incorporate_child_mbr(L1,child);
    //             }
    //             else if(L2Vol<L1Vol){
    //                 group2.push_back(child);
    //                 incorporate_child_mbr(L2,child);
    //             }
    //             else{
    //                 //tie breaker case 2
    //                 if(group1.size()<group2.size()){
    //                     group1.push_back(child);
    //                     incorporate_child_mbr(L1,child);
    //                 }
    //                 else{
    //                     group2.push_back(child);
    //                     incorporate_child_mbr(L2,child);
    //                 }
    //             }
    //         }
    //     }

    //     //update the children of L1 and L2
    //     L1.children = group1;
    //     L2.children = group2;

    //     //save nodes L1 and L2 to page
    //     saveNodeOnDisk(L1);
    //     saveNodeOnDisk(L2);

    //     //for entries in group1 abd group2 and update their parents
    //     for(Entry e : group1){
    //         updateParentOnDisk(e.id,L1.id); //updates parent id of node having id = e.id 
    //     }
    //     for(Entry e : group2){ updateParentOnDisk(e.id,L2.id);}


    //     //since we have created a new child L2
    //     //we call addChild function to the parent of currNode/L1
    //     int parent_id = L1.parent_id;
    //     if(parent_id==-1){
    //         //means this is root
    //         //so new root creation happens
    //         Node newRoot = new Node(T.num_nodes, -1);
    //         T.num_nodes = T.num_nodes+1;

    //         //create two entries for newRoot
    //         Entry ent1 = new Entry(L1.id);
    //         Entry ent2 = new Entry(L2.id);
    //         ent1.minmbr = L1.minmbr.copy();
    //         ent1.maxmbr = L1.maxmbr.copy();
    //         ent2.minmbr = L2.minmbr.copy();
    //         ent2.maxmbr = L2.maxmbr.copy();

    //         //update MBR of newRoot
    //         for(int i=0;i<d;i++){
    //             newRoot.minmbr[i] = min(newRoot.minmbr[i], L1.minmbr[i], L2.minmbr[i]);
    //             newRoot.maxmbr[i] = min(newRoot.maxmbr[i], L1.maxmbr[i], L2.maxmbr[i]);
    //         }

    //         //add the two children to newRoot
    //         newRoot.children[0] = ent1;
    //         newRoot.children[1] = ent2;
    //         updateParentOnDisk(ent1.id, newRoot.id);
    //         updateParentOnDisk(ent2.id, newRoot.id);

    //         saveNodeOnDisk(newRoot);
    //     }
    //     else{
    //         Node parentNode = fetchNodeFromDisk(parent_id);

    //         //update MBR of parent first by incorporating L1 and L2 and then call addChild
    //         for(int i=0;i<d;i++){
    //             parentNode.minmbr[i] = min(parentNode.minmbr[i], L1.minmbr[i], L2.minmbr[i]);
    //             parentNode.maxmbr[i] = min(parentNode.maxmbr[i], L1.maxmbr[i], L2.maxmbr[i]);
    //         }

    //         addChild(parentNode, L2.id, L2.minmbr, L2.maxmbr);
    //     }


    }

}
void insert_and_update(Node& currNode,const vector<int>& P, FileHandler& fh){
    
    if(currNode.children[0].id==-1){
        for(int i=0;i<d;i++){
            currNode.minmbr[i] = min(currNode.minmbr[i],P[i]);
            currNode.minmbr[i] = max(currNode.minmbr[i],P[i]);
        }
        addChild(currNode,-1,P,P,fh);
    }
    else{
        // Entry min_MBR_child;

    }
}



void RTree::insert(const vector<int>& p, FileHandler& fh, FileManager& fm){
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


    }
    else{
        Node rootNode = fetch(root_id,fh);
        insert_and_update(&rootNode, p);

        // int nn = num_nodes/max_num_nodes;
        // PageHandler ph = fh.PageAt(nn);
        // char *data = ph.GetData ();
        // int nodes_in_page = num_nodes%max_num_nodes;
        // int mem_offset = nodes_in_page*(node_size);

        // // check if 
        // if(PAGE_CONTENT_SIZE-mem_offset >= node_size){

        // }
        // else{
        //     //shift to next page
        // }
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