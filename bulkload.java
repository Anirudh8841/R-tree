/*
  	The command to run our code is:
 		./rtree queryfile maxcap d output
	
	Notice that maxCap and d are given as inputs to this command.
	
	In the queryfile we will have a bulkload line which looks like this:
		BULKLOAD filename numPoints
	
	filename is binary file storing the integers in sorted order.
	We have to read total of numPoints from the filename to store in tree.

	The algorithm is from assignment statement:

		We have to read a block of min(maxCap, remainingPoints) number of points at a time and assign them a leaf node.
		In this way we will have created the bottom most layer of the tree composed of leaf nodes.

		Similarly we go through the created leaf nodes and pickup a block of min(maxCap, remainingNodes) nodes, 
		create a parent node for them and assign these block of nodes to this parent.
		In this way we will have created the layer of nodes above the bottom layer.

		We continue this approach till we have only 1 node left. i.e. the root


	In the following code I am assuming that we can hold maxCap number of nodes at a time in memory,
	can be optimized if required
*/



public void bulkload(String bulkload_file_name, int maxCap, int d, int numPoints){
	FileManager fm;

	//create fileHandler for this bulkload_file_name
	FileHandler fh = fm.OpenFile (bulkload_file_name); 

	//we are given PAGE_SIZE so we know how many ints are stored in a page by using PAGE_SIZE/sizeof(int)
	int m = PAGE_SIZE/sizeof(int) // I think in our case m will come out to be 4096/4 = 1024
	int num_nodes = 0;


	int numPointsRead = 0;
	vector< vector<int> > points_collected = new vector([]);

	vector<int> ongoingPoint;
	int ongoingPointIndex=0;

	PageHandler ph = fh.PageAt(0);
	int last_page_read = 0;
	int locationInPage=0;
	char *data = ph.GetData ();

	while(numPointsRead<numPoints){

		if (locationInPage<m){

			//fetch 1 int and store it in ongoingPoint vector at location ongoingPointIndex
			memcpy (&data[locationInPage], &ongoingPoint[ongoingPointIndex], sizeof(int));
			locationInPage+=4;
			ongoingPointIndex++;

			if(ongoingPointIndex==d){
				//means completed one d dimensional point
				numPointsRead++;
				points_collected.push_back(ongoingPoint);
				ongoingPoint = new vector([]); //create new point
				ongoingPointIndex = 0;

				//check whether we have formed a block
				if(points_collected.size() == min(maxCap, numPoints-numPointsRead)){ // min(maxCap, remaining points)
					//create a new leaf node here
					Node leafNode = new Node(num_nodes, -1);

					//assign the children to this leaf node as points_collected
					for(int i=0;i<points_collected.size();i++){
						Entry newChild = new Entry(-1);
						newChild.minmbr = points_collected[i].copy();
						newChild.maxmbr = points_collected[i].copy();
						leafNode.children[i] = newChild;
					}

					//update the MBR of this leaf node by iterating the children
					for(Entry child: leafNode.children){
						for(int i=0; i<d; i++){
							leafNode.minmbr[i] = min(leafNode.minmbr[i], child.minmbr.[i]);
							leafNode.maxmbr[i] = max(leafNode.maxmbr[i], child.maxmbr.[i]);
						}
					}

					saveNodeInPage(leafNode);
					points_collected = new vector([]);
				}
			}

			
		}


		else{
			//we have completed a page need to start reading from next page
			ph = fh.NextPage(last_page_read);
			*data = ph.GetData();
			locationInPage = 0; //update the location in page
		}
	} 


	//at this point we have stored all the points in the leaf nodes
	int start = 0;
	int end = num_nodes;


	buildRecursiveTree(start,end, &num_nodes);
}


//fetch blocks i.e. min(maxCap, remaining number of nodes) from the disk and create a parent of those nodes
//update the parentID inside these nodes and re-save them on the disk
public void buildRecursiveTree(int startNodeID, int endNodeID, int* num_nodes){

	if(endNodeID-startNodeID == 1){
		//we have reached root so stop
		return;
	}

	vector<Node> nodes_collected = new vector([]); //empty vector
	for(int currNodeID=startNodeID; currNodeID<endNodeID; currNodeID++){
		Node childNode = fetchNodeFromDisk(currNodeID);
		nodes_collected.push_back(childNode);

		if(nodes_collected.size()== min(maxCap, endNodeID-currNodeID)){ // min(maxCap, remaining nodes)

			//create a parent for this block of nodes_collected
			Node parentNode = new Node(num_nodes, -1);
			num_nodes++; //update the variable num_nodes using ref, check for syntax I may have done mistake

			//assign the children to this parent node as nodes_collected
			for(int i=0;i<nodes_collected.size();i++){
				Entry newChild = new Entry(-1);
				newChild.minmbr = nodes_collected[i].copy();
				newChild.maxmbr = nodes_collected[i].copy();
				parentNode.children[i] = newChild;

				//also update the parentID inside the nodes in nodes_collected
				nodes_collected[i].parent_id = parentNode.id;
				saveNodeInPage(nodes_collected[i]); //since we changed parent so save this node to page
			}

			//update the MBR of this parent node by iterating the children
			for(Entry child: parentNode.children){
				for(int i=0; i<d; i++){
					parentNode.minmbr[i] = min(parentNode.minmbr[i], child.minmbr.[i]);
					parentNode.maxmbr[i] = max(parentNode.maxmbr[i], child.maxmbr.[i]);
				}
			}

			saveNodeInPage(parentNode);

		} 
	}

	start = endNodeID+1;
	end = num_nodes;
	buildRecursiveTree(start,end,num_nodes);
}



public void saveNodeInPage(Node nodeToSave){
	// save this node to our custom file which is storing all the nodes
	// use this node's id to store it in the corresponding page
}



