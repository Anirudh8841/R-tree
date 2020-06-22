//insertPoint pseudo-code-java mix

//in the code below d = dimension of points


public void insertPoint(vector<int> P, Tree T){
    //fetch the rootNode using the file_manager and rootID
    //here I am assuming that by default a root node exists

    //get root node from disk using rootID
    Node rootNode = fetchNodeFromDisk(T.rootID)

    insert_and_update(rootNode, P); //a recursive function

}


public void insert_and_update(Node currNode, vector<int> P){
    
    if( currNode.children[0].id == -1 ){
        //means currNode is leaf so insertion happens here and updates till the root

        //update MBR of currNode by incorporating P and then call addChild
        for(int i=0;i<d;i++){
            currNode.minmbr[i] = min(currNode.minmbr[i], P.minmbr[i]);
            currNode.maxmbr[i] = min(currNode.maxmbr[i], P.maxmbr[i]);
        }
        addChild(currNode, -1, P, P);
    }


    else{
        //means this node is not leaf 
        //so we have to go along the branches till we reach leaf
        
        //In the children list of this node we choose that child which causes least increase in MBR volume
        //In case of ties there are corresponding actions

        Entry min_MBR_child = null;
        long double volumeIncrease = LONG_DOUBLE_MAX; //long double because volume will be very large
        long double tiebreaker1Vol = LONG_DOUBLE_MAX; //long double because volume will be very large

        for(Entry child : currNode.children){
            //if we choose this child then how much volume do we have to increase
            //basically the difference of change in MBR's volume
            
            //get the old volume and new volume and calculate diff of this child using its mbr
            long double oldVol = calculateVolume(child.minmbr, child.maxmbr);
            long double newVol = calculateVolume( generateMinimum(child.minmbr, P) , generateMaximum(child.maxmbr, P));
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
                    //tie breaker 2, i.e volumeIncrease is same , child volumes are also same, choose the one coming before in the children list
                    int index1 = currNode.children.indexOf(min_MBR_child)
                    int index2 = currNode.children.indexOf(child)
                    if(index2<index1){
                        min_MBR_child = child;
                    }
                }
            }
        }

        //use the id of this child to traverse down the tree
        int child_id = min_MBR_child.id;
        Node nextNode = fetchNodeFromDisk(child_id) //fetchNodeFromDisk is some fetch function which you have implemented
        insert_and_update(nextNode, P); 
    }
}



public void addChild(Node currNode, int childID, vector<int> child_minmbr, vector<int> child_maxmbr){
    //calculate the count of valid children in currNode
    int valid_children_count = 0
    int invalid_childIndex= -1;
    for(int i=0;i<currNode.children.size();i++){
        Entry child = currNode.children[i];
        if (child.minmbr[0] == INT_MAX && child.maxmbr[0]]==INT_MIN) {
            //this is not a valid child he's just a place filler
            invalid_childIndex = i;
            break;
        }
        valid_children_count++;
    }

    //create newChild_entry
    Entry newChild_entry = new Entry(childID); 
    newChild_entry.minmbr  = child_minmbr.copy();
    newChild_entry.maxmbr = child_maxmbr.copy();

    if(valid_children_count<maxCap){
        //so no splitting happens simple insert happens

        //insert the newChild_entry
        currNode.children[invalid_childIndex] = newChild_entry;

        //since currNode is changed so update it in the page
        saveNodeOnDisk(currNode); //creation a  function which fetches the page using currNode's id and replaces it with updated currNode
        simpleBubbleUpToTheRoot(currNode); //start from this node and update till the root

    }
    else if(valid_children_count==maxCap){
        //splitting happens here
        Node L1 = new Node(currNode.id, currNode.parent_id); //L1 is basically updated currentNode
        Node L2 = new Node(T.num_nodes, currNode.parent_id); //L2 is the new created split node
        T.num_nodes = T.num_nodes + 1; //since a new node is created
        vector<Entry> E_set = currNode.children.copy();
        E_set.push_back(newChildEntry);
        //E_set vector is overflowing because of maxCap breach

        //PICKSEED algo.
        //In the E_set select those two entries which are farthest in distance 
        //lets calculate euclidean distance using center points in each dimension
        long double maxDistance = -1;
        Entry e1 = null;
        Entry e2 = null;
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

        vector<Entry> group1 = new vector([]); //empty vector
        vector<Entry> group2 = new vector([]); //empty vector

        group1.push_back(e1);
        group2.push_back(e2);

        //update MBR of L1 and L2
        incorporate_child_mbr(L1,e1);
        incorporate_child_mbr(L2,e2);

        //go over to entries of the E_set and assign them to the group1/L1 or group2/L2
        //on the basis of which group causes least increase in volume
        for(Entry child: E_set){
            if(child==e1 || child==e2){
                continue; //already added these two
            }

            long double childVol = calculateVolume(child.minmbr, child.maxmbr);
            long double newVol1 = calculateVolume( generateMinimum(child.minmbr, L1.minmbr), 
                                                   generateMaximum(child.maxmbr, L1.maxmbr) );

            long double newVol2 = calculateVolume( generateMinimum(child.minmbr, L2.minmbr), 
                                                   generateMaximum(child.maxmbr, L2.maxmbr) );

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

        //update the children of L1 and L2
        L1.children = group1;
        L2.children = group2;

        //save nodes L1 and L2 to page
        saveNodeOnDisk(L1);
        saveNodeOnDisk(L2);

        //for entries in group1 abd group2 and update their parents
        for(Entry e : group1){
            updateParentOnDisk(e.id,L1.id); //updates parent id of node having id = e.id 
        }
        for(Entry e : group2){ updateParentOnDisk(e.id,L2.id);}


        //since we have created a new child L2
        //we call addChild function to the parent of currNode/L1
        int parent_id = L1.parent_id;
        if(parent_id==-1){
            //means this is root
            //so new root creation happens
            Node newRoot = new Node(T.num_nodes, -1);
            T.num_nodes = T.num_nodes+1;

            //create two entries for newRoot
            Entry ent1 = new Entry(L1.id);
            Entry ent2 = new Entry(L2.id);
            ent1.minmbr = L1.minmbr.copy();
            ent1.maxmbr = L1.maxmbr.copy();
            ent2.minmbr = L2.minmbr.copy();
            ent2.maxmbr = L2.maxmbr.copy();

            //update MBR of newRoot
            for(int i=0;i<d;i++){
                newRoot.minmbr[i] = min(newRoot.minmbr[i], L1.minmbr[i], L2.minmbr[i]);
                newRoot.maxmbr[i] = min(newRoot.maxmbr[i], L1.maxmbr[i], L2.maxmbr[i]);
            }

            //add the two children to newRoot
            newRoot.children[0] = ent1;
            newRoot.children[1] = ent2;
            updateParentOnDisk(ent1.id, newRoot.id);
            updateParentOnDisk(ent2.id, newRoot.id);

            saveNodeOnDisk(newRoot);
        }
        else{
            Node parentNode = fetchNodeFromDisk(parent_id);

            //update MBR of parent first by incorporating L1 and L2 and then call addChild
            for(int i=0;i<d;i++){
                parentNode.minmbr[i] = min(parentNode.minmbr[i], L1.minmbr[i], L2.minmbr[i]);
                parentNode.maxmbr[i] = min(parentNode.maxmbr[i], L1.maxmbr[i], L2.maxmbr[i]);
            }

            addChild(parentNode, L2.id, L2.minmbr, L2.maxmbr);
        }

    }

}




public void updateParentOnDisk(int nodeID, int new_parentID){
    if(nodeID==-1){
        return; //its a point in leaf node
    }

    //fetch the node from the page
    Node node = fetchNodeFromDisk(nodeID);
    node.parentID = new_parentID;
    saveNodeOnDisk(node); //save back to page
}

public void simpleBubbleUpToTheRoot(Node currNode){
    if(currNode.parentID==-1){
        //no parent so no need to do anything
        return;
    }

    Node parentNode = fetchNodeFromDisk(currNode.parentID);

    //update this parentNode's MBR
    for(int i=0;i<d;i++){
        parentNode.minmbr[i] = min(parentNode.minmbr[i], currNode.minmbr[i])
        parentNode.maxmbr[i] = max(parentNode.maxmbr[i], currNode.maxmbr[i])
    }

    //update the MBR of that child in parent children list which matches currNode's id
    for(Entry child : parentNode.children){
        if(child.id==currNode.id){

            for(int i=0;i<d;i++){
                child.minmbr[i] = min(child.minmbr[i], currNode.minmbr[i])
                child.maxmbr[i] = max(child.maxmbr[i], currNode.maxmbr[i])
            }

            break;
        }
    }


    saveNodeOnDisk(parentNode); //since parentNode is changed so update it in page
    simpleBubbleUpToTheRoot(parentNode); //recurse
}



//calculates the volume using minmbr and maxmbr
public void calculateVolume(vector<int> minmbr, vector<int> maxmbr){
    long double vol = 1;
    for(int i=0; i<minmbr.size(); i++){
        int lb = minmbr[i];
        int ub = maxmbr[i];
        if(lb==INT_MIN){
            break;
        }
        vol *= (ub-lb);
    }
    return vol;
}

//generates a vector by taking minimum in each entry
public vector<int> generateMinimum(vector<int> v1, vector<int> v2){
    vector<int> result = new vector<int>(v1.size());
    for(int i=0; i<v1.size(); i++){
        result[i] = min(v1[i], v2[i]);
    }
    return result;
}

//generates a vector by taking maximum in each entry
public vector<int> generateMaximum(vector<int> v1, vector<int> v2){
    vector<int> result = new vector<int>(v1.size());
    for(int i=0; i<v1.size(); i++){
        result[i] = max(v1[i], v2[i]);
    }
    return result;
}

//updates MBR of node by incorporating the child
public void incorporate_child_mbr(Node node, Entry child){
    for(int i=0;i<d;i++){
        node.minmbr[i] = min(node.minmbr[i], child.minmbr[i]);
        node.maxmbr[i] = max(node.maxmbr[i], child.maxmbr[i]);
    }
}


public void fetchNodeFromDisk(int nodeID){

    //implement this function which generates node from disk page by taking nodeID
}