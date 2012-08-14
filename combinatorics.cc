/*	file combinatorics.cc 	

	Definitions of combinatorial data structures associated to a packing, and simple
	functions to produce one packing from another.
	
	Packings are stored in a file as a list of integers followed by a list of doubles

	Format is:
		V = number of vertices
		valence of vertex 0; adjacency list of vertex 0
		valence of vertex 1; adjacency list of vertex 1
			. . .
		valence of vertex V-1; adjacency list of vertex V-1
		radius of vertex 0
		radius of vertex 1
			. . .
		radius of vertex V-1
		(everything after this is ignored.)

	Main operations to perform on packing; (*) indicates that the function is implemented:
		(*) subdivide (add a new 6-valent vertex at the center of each edge in an obvious way)
		(*) amalgamate (remove the vertices adjacent to some specific vertex)
		(*) branch_cover (branch over specified vertices with specified monodromy); in file branched_cover.cc
	
	contains definitions of packing, adjacency_list, edge_list
	contains functions which_index, read_packing, write_packing, edge_num, subdivide 	*/

struct adjacency_list { 
	/* cyclic list of vertices adjacent to a given vertex */
	vector<int> a; // names of adjacent vertices are integers. 0 means the vertex at infinity.
};

struct packing {	// packing
	/* A list of adjacency lists, one for each vertex in order. This should be
	subject to certain compatibility conditions, but we don't check. The number of
	radii should also be equal to the number of vertices; again, we don't check. */
	
	vector<adjacency_list> v;	// list of adjacency lists of vertices in order.
	vector<double> r;			// list of radii
};



int which_index(packing P, int i, int j){	// if j is adjacent to i in P, what is k so that P.v[i].a[k]=j?
	int k,l,m;
	k=P.v[i].a.size();	// valence of vertex i
	m=-1;				// default value
	for(l=0;l<k;l++){	// cycle through adjacency list
		if(P.v[i].a[l]==j){		// have we found vertex j?
			m=l;				// if so, set m to its index
		};
	};
	return(m);
};

void read_packing(ifstream &packing_file, packing &P){	// read packing from a file
	int vertices, valence, i, j, k;
	adjacency_list L;
	double d;
	
	packing_file >> vertices;
	for(i=0;i<vertices;i++){
		packing_file >> valence;
		for(j=0;j<valence;j++){
			packing_file >> k;
			L.a.push_back(k);
		};
		P.v.push_back(L);
		L.a.clear();
	};
	for(i=0;i<vertices;i++){
		packing_file >> d;
		P.r.push_back(d);
	};
	return;
};

void write_packing(ofstream &packing_file, packing P){	// write packing to a file
	int i,j;
	packing_file << P.v.size() << "\n";
	for(i=0;i<(int) P.v.size();i++){
		packing_file << P.v[i].a.size() << "   ";
		for(j=0;j<(int) P.v[i].a.size();j++){
			packing_file << P.v[i].a[j] << " ";
		};
		packing_file << "\n";
	};
	for(i=0;i<(int) P.v.size();i++){
		packing_file << P.r[i] << "\n";
	};
};

void write_packing(packing P){	// write packing to cout (should I give this a different name?)
	int vertices, valence, i, j, k;
	double d;
	vertices=P.v.size();
	cout << vertices << " vertices \n";
	for(i=0;i<vertices;i++){
		cout << "vertex " << i;
		valence=P.v[i].a.size();
		cout << " valence " << valence << " adjacent to ";
		for(j=0;j<valence;j++){
			k=P.v[i].a[j];
			cout << k << " ";
		};
		d=P.r[i];
		cout << "  radius " << d << "\n";
	};
	return;
};

bool valid_packing(packing P){	// determines whether P is an honest triangulation
	int i,j,k,l,m,n,o;
	int vertices,edges;
	bool valid;
	valid=true;		// initialize variable
	vertices=P.v.size();
	edges=0;		// initialize variable
	for(i=0;i<(int) P.v.size();i++){	// for each vertex i
		edges=edges+P.v[i].a.size();
		for(j=0;j<(int) P.v[i].a.size();j++){	// for each edge starting at i
			k=P.v[i].a[j];		// what is the endpoint of that edge?
			l=which_index(P,k,i);	// does k point to i? in which direction l?
			if(l==-1){
				valid=false;
				cout << "vertex " << i << " is adjacent to " << k << " but " << k <<
					" is not adjacent to " << i << ".\n";
			};
			m=P.v[k].a[(l+1)%P.v[k].a.size()];	
			n=which_index(P,m,k);
			o=P.v[m].a[(n+1)%P.v[m].a.size()];
			if(o!=i){
				valid=false;
				cout << "triangle " << i << " -> " << k << " -> " << m << " -> " << o << "\n";
			};
		};
	};
	edges=edges/2;
	cout << vertices << " vertices, " << edges << " edges; Euler characteristic is " << vertices - edges/3 << "\n";
	return(valid);
};

struct edge_list{	// data structure used by function subdivide
	vector<int> initial;
	vector<int> terminal;
	vector<int> left;
	vector<int> right;
};

int edge_num(packing P, int i, int j, edge_list E){	// what is the edge from i to j?
	int k,l;
	l=0;
	for(k=0;k<(int) E.initial.size();k++){
		if(((E.initial[k]==i)&&(E.terminal[k]==j))||((E.initial[k]==j)&&(E.terminal[k]==i))){
			l=k;
		};
	};
	return(l);
};

packing subdivide(packing P){	// produces new packing by subdividing each triangle into 4 subtriangles
	packing Q;
	edge_list E;
	adjacency_list L;
	int i,j,k,l,m;
	for(i=0;i<(int) P.v.size();i++){
		for(j=0;j<(int) P.v[i].a.size();j++){
			if(i<P.v[i].a[j]){
				E.initial.push_back(i);
				E.terminal.push_back(P.v[i].a[j]);
		//		E.left.push_back((i+1)%P.v[i].a.size());
		//		E.right.push_back((P.v[i].a.size()-1)%P.v[i].a.size());
			};
		};
	};
	for(i=0;i<(int) P.v.size();i++){		// Q has vertices coming from vertices of P
		for(j=0;j<(int) P.v[i].a.size();j++){
			L.a.push_back(P.v.size()+edge_num(P,i,P.v[i].a[j],E));
		};
		Q.v.push_back(L);
		if(i==0){
			Q.r.push_back(-1.0);
		} else {
			Q.r.push_back(0.1);
		};
		L.a.clear();
	};
	for(i=0;i<(int) E.initial.size();i++){	// Q has vertices coming from edges of P
		j=E.initial[i];
		k=E.terminal[i];
		l=P.v[j].a[(which_index(P,j,k)+1)%P.v[j].a.size()];
		m=P.v[k].a[(which_index(P,k,j)+1)%P.v[k].a.size()];
	
	//	cout << "edge " << i << " j " << j << " k " << k << " l " << l << " m " << m << "\n";
		L.a.push_back(j);
		L.a.push_back(P.v.size()+edge_num(P,j,m,E));
		L.a.push_back(P.v.size()+edge_num(P,m,k,E));
		L.a.push_back(k);
		L.a.push_back(P.v.size()+edge_num(P,k,l,E));
		L.a.push_back(P.v.size()+edge_num(P,l,j,E));

		Q.v.push_back(L);
		Q.r.push_back(0.1);
		L.a.clear();
	};
	
	return(Q);
};

packing amalgamate(packing P, int V){	// produces new packing by removing the immediate neighbors of a specified vertex
	packing Q;
	Q=P;
	
	Q.v[V].a.clear();
	int i,j,k,l,m,n,o,p,q,r,s;
	bool moved_edge;
	for(i=0;i<(int) P.v[V].a.size();i++){		// for each vertex adjacent to V
		j=P.v[V].a[i];					// let j be its index
		k=P.v[j].a.size();				// let k be the valence of j
		l=which_index(P,j,V);			// which index is V from j?
		for(m=l+2;m<l+k-2;m++){
			n=m%k;
			o=P.v[j].a[n];				// o is adjacent to j but not V
			Q.v[V].a.push_back(o);		// vertex adjacent to j in P is adjacent to V in Q

			p=P.v[o].a.size();			// p is valence of o
			moved_edge=false;			// reset for each o
			for(q=0;q<p;q++){
				r=P.v[o].a[q];			// r is adjacent to o
				s=which_index(P,r,V);		// is V adjacent to r?
				if(s!=-1){					// if it is,
					if(moved_edge==false){		// and this is the first time,
						Q.v[o].a[q]=V;			// make it point to V in Q instead
						moved_edge=true;
					} else {
						Q.v[o].a.erase(Q.v[o].a.begin()+q);			// prune superfluous edge
					};
				};
			};
			
			
			
		};
		Q.v[j].a.clear();				// make vertex j 0-valent in Q
	};
	
	bool pruned=false;
	bool found_0_valent;
	while(pruned==false){				// routine to prune 0-valent vertices
		found_0_valent=false;			// initialize
		for(i=0;i<(int) Q.v.size();i++){		// look for 0-valent vertex
			if(Q.v[i].a.size()==0){
				found_0_valent=true;
				q=i;					// q will be the biggest index 0-valent vertex
			};
		};
		if(found_0_valent==false){
			pruned=true;	
		} else {
			Q.v.erase(Q.v.begin()+q);				// remove vertex q;
			for(i=0;i<(int) Q.v.size();i++){	
				for(j=0;j<(int) Q.v[i].a.size();j++){
					k=Q.v[i].a[j];
					if(k>q){
						Q.v[i].a[j]--;	// change labels in adjacency lists
					};
				};
			};
		};
	};
	
	return(Q);
};