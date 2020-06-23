#include "rtree.h"

int root_id=-1, maxCap=0 ,d=0,node_size=0,num_nodes=0,max_num_nodes=0;


Node fetch(int id,FileHandler& fh){

    int pagenum = floor(id/max_num_nodes);
    int offset = (id%max_num_nodes)*node_size;

    PageHandler ph = fh.PageAt(pagenum);

    char *data = ph.GetData ();
    Node node(0,-1);

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
    int offset = (node.id%max_num_nodes)*node_size;
    
    int n = fh.LastPage().GetPageNum();
    
    PageHandler ph;
    
    if(pagenum == (n+1)){
        ph = fh.NewPage();
    }
    else if(pagenum <= n){
        ph = fh.PageAt(pagenum);
    }
    else{
        // cout<<"page error in save"<<endl;
    }

    char *data = ph.GetData ();

    memcpy (&data[offset],&node.id, sizeof(int));

    memcpy (&data[offset+4], &node.parent_id, sizeof(int));

    int temp=8+offset;
    for(int i=0;i<d;i++){
        // cout<< node.id<<" "<<i <<" minmbrs "<< node.minmbr[i]<<endl;
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
    fh.MarkDirty(pagenum);
	fh.UnpinPage(pagenum);
	fh.FlushPage(pagenum);
    return ;    
}

long double calculateVolume(const vector<int> &minmbr, const vector<int>& maxmbr){
    long double vol =1;
    // cout<< " min and max "<<minmbr.size()<<" "<<maxmbr.size()<<endl;
    for (int i = 0; i < minmbr.size(); i++)
    {
        int lb = minmbr[i];
        int ub = maxmbr[i];
        if(lb==INT_MAX){
            break;
        }
        vol = vol*(ub-lb);
    }
    return vol;
}


void updateParentOnDisk(int nodeID, int new_parentID, FileHandler& fh ){
    // cout<<"p_n "<< nodeID<< " new_p "<< new_parentID<<endl;
    if(nodeID=-1){
        return;
    }  
    Node node = fetch(nodeID,fh);
    node.parent_id = new_parentID;
    save(node,fh);
}

void generateMinimum(const vector<int>& v1,const vector<int>& v2, vector<int>& result){
    result.clear();
    for(int i=0; i<v1.size(); i++){
        result.push_back(min(v1[i], v2[i]));
    }
}
void generateMaximum(const vector<int>& v1, const vector<int>& v2, vector<int>& result){
    result.clear();
    for(int i=0; i<v1.size(); i++){
        result.push_back(max(v1[i], v2[i]));
    }
}

void simpleBubbleUpToTheRoot(Node currNode, FileHandler& fh){
    // cout<<"id :"<<currNode.id<<endl;
    if(currNode.parent_id==-1){
        // cout<<"id1 :"<<currNode.id<<endl;
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

void incorporate_child_mbr(Node& node, Entry child){
    for(int i=0;i<d;i++){
        node.minmbr[i] = min(node.minmbr[i], child.minmbr[i]);
        node.maxmbr[i] = max(node.maxmbr[i], child.maxmbr[i]);
    }
}
bool entry_eq(Entry e1,Entry e2){
   
    if(e1.minmbr.size()!=e2.minmbr.size() || e1.maxmbr.size()!=e2.maxmbr.size()){
        return false;
    }
    if(e1.id!=e2.id){
        return false;
    }
    for(int i=0;i<e1.minmbr.size();i++){
        if(e1.minmbr[i]!=e2.minmbr[i] || e1.maxmbr[i]!= e2.maxmbr[i]){
            return false;
        }
    }
    return true;
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
        // cout<< " e_s "<< E_set.size()<<" ch "<< currNode.children.size() <<endl;
        long double maxDistance = -1;
        Entry e1;
        Entry e2;
        for(int i=0; i< E_set.size(); i++){
            for(int j=i+1; j<E_set.size(); j++){
                Entry child1 = E_set[i];
                Entry child2 = E_set[j];

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
        // cout<< "came"<<endl;

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
       
        for(Entry child: E_set){
            if(entry_eq(child,e1) || entry_eq(child,e2)){
                continue; //already added these two
            }
            long double childVol = calculateVolume(child.minmbr, child.maxmbr);
            vector<int> r1,r2;
            generateMinimum(child.minmbr, L1.minmbr,r1);
            generateMaximum(child.maxmbr, L1.maxmbr,r2);
            long double newVol1 = calculateVolume( r1,r2 );

            generateMinimum(child.minmbr, L2.minmbr,r1);
            generateMaximum(child.maxmbr, L2.maxmbr,r2);
            long double newVol2 = calculateVolume( r1, r2 );

            long double increase1 = abs(newVol1-childVol);
            long double increase2 = abs(newVol2-childVol);
            if(increase1<increase2){
                group1.push_back(child);
                incorporate_child_mbr(L1,child);
            }
            else if(increase1>increase2){
                group2.push_back(child);
                incorporate_child_mbr(L2,child);
            }
            else{
                //tie breaker case 1
                long double L1Vol = calculateVolume(L1.minmbr, L1.maxmbr);
                long double L2Vol = calculateVolume(L2.minmbr, L2.maxmbr);
                if(L1Vol<L2Vol){
                    group1.push_back(child);
                    incorporate_child_mbr(L1,child);
                }
                else if(L2Vol<L1Vol){
                    group2.push_back(child);
                    incorporate_child_mbr(L2,child);
                }
                else{
                    //tie breaker case 2
                    if(group1.size()<group2.size()){
                        group1.push_back(child);
                        incorporate_child_mbr(L1,child);
                    }
                    else{
                        group2.push_back(child);
                        incorporate_child_mbr(L2,child);
                    }
                }
            }
        }
    //     //update the children of L1 and L2

    for(int i=0;i<group1.size();i++){
        L1.children[i] = group1[i];
    }
    for(int i=0;i<group2.size();i++){
        L2.children[i] = group2[i];
    }
    // L1.children = group1;
    // L2.children = group2;

        save(L1,fh);
        save(L2,fh);


    //     //for entries in group1 and group2 and update their parents
        for(Entry e : group1){
            updateParentOnDisk(e.id,L1.id,fh); //updates parent id of node having id = e.id 
        }
        for(Entry e : group2){ updateParentOnDisk(e.id,L2.id,fh);}


        //since we have created a new child L2
        //we call addChild function to the parent of currNode/L1
        int parent_id = L1.parent_id;
        if(parent_id==-1){
            //means this is root
            //so new root creation happens
            Node newRoot(num_nodes, -1);
            num_nodes = num_nodes+1;

            //create two entries for newRoot
            Entry ent1(L1.id);
            Entry ent2(L2.id);
            ent1.minmbr = L1.minmbr;
            ent1.maxmbr = L1.maxmbr;
            ent2.minmbr = L2.minmbr;
            ent2.maxmbr = L2.maxmbr;

            //update MBR of newRoot
            for(int i=0;i<d;i++){
                newRoot.minmbr[i] = min(newRoot.minmbr[i], min(L1.minmbr[i], L2.minmbr[i]));
                newRoot.maxmbr[i] = max(newRoot.maxmbr[i], max(L1.maxmbr[i], L2.maxmbr[i]));
            }

            //add the two children to newRoot
            newRoot.children[0] = ent1;
            newRoot.children[1] = ent2;
            
            updateParentOnDisk(ent1.id, newRoot.id,fh);
            updateParentOnDisk(ent2.id, newRoot.id,fh);
            root_id = newRoot.id;
            // cout<<"eh 3"<<endl; 
            save(newRoot,fh);
            
        }
        else{
            Node parentNode = fetch(parent_id,fh);
            //update MBR of parent first by incorporating L1 and L2 and then call addChild
            for(int i=0;i<d;i++){
                parentNode.minmbr[i] = min(parentNode.minmbr[i], min(L1.minmbr[i], L2.minmbr[i]));
                parentNode.maxmbr[i] = max(parentNode.maxmbr[i], max(L1.maxmbr[i], L2.maxmbr[i]));
            }

            addChild(parentNode, L2.id, L2.minmbr, L2.maxmbr,fh);
        }
    }

}
void insert_and_update(Node& currNode,const vector<int>& P, FileHandler& fh){
    // cout<<"entered "<<endl;
    if(currNode.children[0].id==-1){
        for(int i=0;i<d;i++){
            currNode.minmbr[i] = min(currNode.minmbr[i],P[i]);
            currNode.minmbr[i] = max(currNode.minmbr[i],P[i]);
        }
        addChild(currNode,-1,P,P,fh);
    }
    else{
        Entry min_MBR_child;
        long double volumeIncrease = LDBL_MAX; //long double because volume will be very large
        long double tiebreaker1Vol = LDBL_MAX; //long double because volume will be very large

        for(Entry child : currNode.children){
            //if we choose this child then how much volume do we have to increase
            //basically the difference of change in MBR's volume
            
            //get the old volume and new volume and calculate diff of this child using its mbr
            long double oldVol = calculateVolume(child.minmbr, child.maxmbr);
            vector<int> r1,r2;
            generateMinimum(child.minmbr, P,r1);
            generateMaximum(child.maxmbr, P,r2);
            long double newVol = calculateVolume( r1, r2);
            long double diff = newVol-oldVol;

            if(diff<volumeIncrease){
                min_MBR_child = child;
                volumeIncrease = diff;
            }
            else if(diff==volumeIncrease){
                //tie breaker 1, i.e. volumeIncrease is same so we need will choose that child which has lower volume
                if(oldVol<tiebreaker1Vol){
                    min_MBR_child = child;
                    tiebreaker1Vol = oldVol;
                }
                else if (oldVol==tiebreaker1Vol){
                    int index1=-1,index2=-1;
                    for(int i=0;i<currNode.children.size();i++){
                        if(entry_eq(currNode.children[i],min_MBR_child)){
                            index1=i;
                            break;
                        }
                    }
                    for(int i=0;i<currNode.children.size();i++){
                        if(entry_eq(currNode.children[i],child)){
                            index2=i;
                            break;
                        }
                    }
                    if(index1==-1 && index2==-1){
                        // cout<<"error in finding index L446"<<endl;
                    }
                    //tie breaker 2, i.e volumeIncrease is same , child volumes are also same, choose the one coming before in the children list
                    // int index1 = find(currNode.children.begin(),currNode.children.begin(),min_MBR_child) - currNode.children.begin();
                    // int index2 = find(currNode.children.begin(),currNode.children.begin(),child) - currNode.children.begin();
                    if(index2<index1){
                        min_MBR_child = child;
                    }
                    
                }
            }
        }
        //use the id of this child to traverse down the tree
        int child_id = min_MBR_child.id;
        Node nextNode = fetch(child_id,fh); //fetchNodeFromDisk is some fetch function which you have implemented
        insert_and_update(nextNode,P,fh); 

        // Entry min_MBR_child;

    }
}



void RTree::insert(const vector<int>& p, FileHandler& fh){
    //root node
    if(num_nodes==0){

        PageHandler ph = fh.NewPage();
        char *data = ph.GetData ();
        Node num(0,-1);

        root_id=0;
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
        insert_and_update(rootNode, p,fh);
    }
}


bool search(int nodeID, const vector<int>& P, FileHandler& fh){
	//fetch the node node from disk using nodeID
    Node currNode = fetch(nodeID,fh);
    
	if(currNode.children[0].id==-1){
        // cout << " CUR "<<currNode.id<<endl;
		//means the currNode is a leaf and its children are all points

		//go over these children points and check whether our search point is among one of them or not
		for(Entry child : currNode.children){
			if(child.minmbr[0]==INT_MAX && child.maxmbr[0]==INT_MIN){
				//means this is just a place filler child
				//and the upcoming ones will also be place fillers
				return false;
			}
			else{
				//point is valid
				//check whether this point is equal to P
				bool equals = true;
				for(int i=0;i<d;i++){
					if(child.minmbr[i]!=P[i] || child.maxmbr[i]!=P[i]){
						equals = false;
						break;
					}
				}

				if(equals){
					return true;
				}
			}
		}

		return false;
	}

	else{
		//means currNode is not a leaf so we have to check children of this currNode

		for(Entry child : currNode.children){

			//check whether child is valid or just a place filler
			if(child.minmbr[0]==INT_MAX && child.maxmbr[0]==INT_MIN){
				//means this is just a place filler child
				//and the upcoming ones will also be place fillers
				break;
			}
			else{
				//if child is valid check if P lies inside this child's MBR in each dimension
				//if it does then we recurse on that child
				bool liesInside = true;
				for(int i=0;i<d;i++){
					if(P[i] < child.minmbr[i] || P[i] > child.maxmbr[i]){
						liesInside = false;
						break;
					}
				}

				if(liesInside){
					//P lies inside so recurse on this child
					if( search(child.id, P,fh) ){
						return true;
					}
				}
			}
		}

		return false;
	}


}

bool RTree::query(const vector<int>& P, FileHandler& fh){
	//fetch the node node from disk using nodeID
    return search(root_id,P,fh);
}

// need to edit
// void buildRecursiveTree(int startNodeID, int endNodeID, int& nodes_count,FileHandler &fh){

// 	if(endNodeID-startNodeID == 1){
// 		//we have reached root so stop
// 		return;
// 	}

// 	vector<Node> nodes_collected; 
// 	for(int currNodeID=startNodeID; currNodeID<endNodeID; currNodeID++){
// 		Node childNode = fetch(currNodeID,fh);
// 		nodes_collected.push_back(childNode);

// 		if(nodes_collected.size()== min(maxCap, endNodeID-currNodeID)){ // min(maxCap, remaining nodes)

// 			//create a parent for this block of nodes_collected
// 			Node parentNode(num_nodes, -1);
// 			num_nodes++; //update the variable num_nodes using ref, check for syntax I may have done mistake

// 			//assign the children to this parent node as nodes_collected
// 			for(int i=0;i<nodes_collected.size();i++){
// 				Entry newChild(-1);
// 				newChild.minmbr = nodes_collected[i];
// 				newChild.maxmbr = nodes_collected[i];
// 				parentNode.children[i] = newChild;

// 				//also update the parentID inside the nodes in nodes_collected
// 				nodes_collected[i].parent_id = parentNode.id;
// 				save(nodes_collected[i],fh); //since we changed parent so save this node to page
// 			}

// 			//update the MBR of this parent node by iterating the children
// 			for(Entry child: parentNode.children){
// 				for(int i=0; i<d; i++){
// 					parentNode.minmbr[i] = min(parentNode.minmbr[i], child.minmbr[i]);
// 					parentNode.maxmbr[i] = max(parentNode.maxmbr[i], child.maxmbr[i]);
// 				}
// 			}

// 			save(parentNode,fh);

// 		} 
// 	}

// 	start = endNodeID+1;
// 	end = num_nodes;
// 	buildRecursiveTree(start,end,num_nodes);
// }




// void RTree::bulkload(int numPoints,FileHandler& fo,FileHandler& fh){
//     int m = floor(PAGE_CONTENT_SIZE/sizeof(int));
//     int nodes_count=0;
//     int numPointsRead =0;
//     vector< vector<int>> points_collected;
//     vector<int> ongoingPoint;
// 	int ongoingPointIndex=0;

// 	PageHandler ph = fo.PageAt(0);
// 	int last_page_read = 0;
// 	int locationInPage=0;
// 	char *data = ph.GetData ();

// 	while(numPointsRead<numPoints){

// 		if (locationInPage<m){

// 			//fetch 1 int and store it in ongoingPoint vector at location ongoingPointIndex
// 			memcpy (&data[locationInPage], &ongoingPoint[ongoingPointIndex], sizeof(int));
// 			locationInPage+=4;
// 			ongoingPointIndex++;

// 			if(ongoingPointIndex==d){
// 				//means completed one d dimensional point
// 				numPointsRead++;
// 				points_collected.push_back(ongoingPoint);
// 				// ongoingPoint.clear();  //create  new point  doubt 1
// 				ongoingPointIndex = 0;

// 				//check whether we have formed a block
// 				if(points_collected.size() == min(maxCap, numPoints-numPointsRead)){ // min(maxCap, remaining points)
// 					//create a new leaf node here
// 					Node leafNode (num_nodes, -1);

// 					//assign the children to this leaf node as points_collected
// 					for(int i=0;i<points_collected.size();i++){
// 						Entry newChild(-1);
// 						newChild.minmbr = points_collected[i];
// 						newChild.maxmbr = points_collected[i];
// 						leafNode.children[i] = newChild;
// 					}

// 					//update the MBR of this leaf node by iterating the children
// 					for(Entry child: leafNode.children){
// 						for(int i=0; i<d; i++){
// 							leafNode.minmbr[i] = min(leafNode.minmbr[i], child.minmbr[i]);
// 							leafNode.maxmbr[i] = max(leafNode.maxmbr[i], child.maxmbr[i]);
// 						}
// 					}

// 					save(leafNode,fh);
// 					points_collected.clear();
// 				}
// 			}

			
// 		}


// 		else{
// 			//we have completed a page need to start reading from next page
// 			ph = fo.NextPage(last_page_read);
// 			data = ph.GetData();
// 			locationInPage = 0; //update the location in page
// 		}
// 	} 


// 	//at this point we have stored all the points in the leaf nodes
// 	int start = 0;
// 	int end = num_nodes;


// 	buildRecursiveTree(start,end, &num_nodes);
// }
