/*	branched_cover.cc (formerly edge_path.cc) algorithm to compute branched cover 
	new version 0.3 August 13, 2012*/
   
struct path{	// a path is an ordered list of integers
	vector<int> v;
};

struct branch_paths{	// an ordered list of paths
	vector<path> X;
};

path path_to_zero(packing P, center_list C, int v){	// determines canonical path from v to 0
	path X;
	int current_vertex,i,j,k;
	current_vertex=v;
	X.v.push_back(v);
	while(current_vertex!=0){	// if v=0 this is skipped
	//	cout << "testing to see if adjacent to 0 " << which_index(P,current_vertex,0) << "\n";
		if(which_index(P,current_vertex,0)!=-1){	// if 0 is adjacent to v
			X.v.push_back(0);
			current_vertex=0;
		} else {
			i=0;
			k=P.v[current_vertex].a[0];	// candidate biggest neighbor
			for(i=0;i<(int) P.v[current_vertex].a.size();i++){
				j=P.v[current_vertex].a[i];	// get neighbor j

				if(radius_squared(C.p[j])>radius_squared(C.p[k])){	// if j is further out than k
					k=j;	// update candidate biggest neighbor 
				};
			};
			X.v.push_back(k);
			current_vertex=k;
		};
	};
	return(X);
};

void write_path(path X){
	int i;
	for(i=0;i<(int) X.v.size();i++){
		cout << X.v[i] << " ";
	};
	cout << "\n";
};

struct permutation{	// a permutation is an ordering on {0,1, . . . , d-1}
	vector<int> p;	
};

permutation inverse(permutation P){	// returns the inverse of a permutation
	permutation Q;
	Q=P;
	int i,j;
	for(i=0;i<(int) P.p.size();i++){
		j=P.p[i];
		Q.p[j]=i;
	};
	return(Q);
};

int orbit_size(permutation P, int i){
	int j,k;
	j=1;
	k=P.p[i];
	while(k!=i){
		j++;
		k=P.p[k];
	};
	return(j);
};

int degree(permutation S){
	return(S.p.size());
};

struct branch_data{
	vector<int>	v;			// location of branch point i is v[i]
	vector<permutation> b;	// monodromy of positive loop around branch point i is permutation b[i]
};

bool branch_point(branch_data B, int i){
	int j;
	bool found_match;
	found_match=false;
	for(j=0;j<(int) B.v.size();j++){
		if(B.v[j]==i){
			found_match=true;
		};
	};
	return(found_match);
};

int naive_new_index(int size_of_P, int v, int sheet){	// index in branched cover
	return(sheet*size_of_P+v);
};

	/* note: naive_new_index will be adjusted later. We will remove new indices
	corresponding to pairs (v,s) where v is a branch point, 
	and itinerary(v,s) contains s' < s. */
	
void compute_branch_cuts(branch_paths &Y, packing P, branch_data B, center_list C){
	int i;
	path X;
	for(i=0;i<(int) B.v.size();i++){	// for each branch point
	//	cout << "for branch point " << B.v[i] << " determining path to zero. \n";
		X=path_to_zero(P,C,B.v[i]);		// determine the path to zero
		write_path(X);
		Y.X.push_back(X);				// and add it to the list of branch paths.
	};
};

void initialize_trivial_cover(packing &Q, packing P, int degree){
	int size_of_P, i, j, s;
	size_of_P = (int) P.v.size();
	adjacency_list A;
	for(s=0;s<degree;s++){
		for(i=0;i<size_of_P;i++){
			A.a.clear();
			for(j=0;j<(int) P.v[i].a.size();j++){
				A.a.push_back(naive_new_index(size_of_P,P.v[i].a[j],s));	// should be inline?
			};
			Q.v.push_back(A);
			Q.r.push_back(1.0);	// should we make an educated guess about new radius?
		};
	};
	Q.r[0]=-1.0;
};


void generate_elimination_list(packing P, branch_data B, vector<int> &E, vector<int> &F){
	vector<int> L,M;
	int i,j,size_of_P,sheet,new_sheet,minimum_sheet;
	bool found_smaller;
	size_of_P = (int) P.v.size();

	for(i=0;i<(int) B.v.size();i++){
		j=B.v[i];								// j is ith branch point
		for(sheet=0;sheet<degree(B.b[0]);sheet++){	// test sheet to eliminate
			found_smaller=false;
			new_sheet=B.b[i].p[sheet];	// compute
			minimum_sheet=sheet;
			while(new_sheet!=sheet){
				if(new_sheet<sheet){
					found_smaller=true;
					if(new_sheet<minimum_sheet){
						minimum_sheet=new_sheet;
					};
				};
				new_sheet=B.b[i].p[new_sheet];
			};
			if(found_smaller==true){
				L.push_back(naive_new_index(size_of_P,j,sheet));	// mark (j,sheet) for elimination
				M.push_back(naive_new_index(size_of_P,j,minimum_sheet));	// remember where to redirect links to this vertex
			};
		};
	};
	E=L;
	F=M;
};

void remove_spurious_points(vector<int> E, vector<int> F, packing &Q){
	int i,j,k,e;
	for(i=0;i<(int) E.size();i++){	// first adjust pointers to replacements
		for(j=0;j<(int) Q.v.size();j++){
			for(k=0;k<(int) Q.v[j].a.size();k++){
				if(Q.v[j].a[k]==E[i]){
					Q.v[j].a[k]=F[i];
				};
			};
		};
	};
	for(i=0;i<(int) E.size();i++){	// then eliminate vertices and adjust pointers
		e=E[i];
		Q.v.erase(Q.v.begin()+e);	// remove entry e
		for(j=0;j<(int) Q.v.size();j++){
			for(k=0;k<(int) Q.v[j].a.size();k++){
				if(Q.v[j].a[k]>=e){
					Q.v[j].a[k]--;
				};
			};
		};
	};	
};
	

packing branched_cover(packing P, branch_data B, center_list C){
	packing Q;
	path X;
	branch_paths Y;
	int i,j,k,l,m,n,o,r,sheet,new_sheet,current_sheet;
	int branch_degree;
	permutation sigma;
	int size_of_P;
	adjacency_list A;
	int valence, next_m, prev_m;
	
	branch_degree=degree(B.b[0]);	// don't want to keep computing this.
	size_of_P = (int) P.v.size();
	
	/* first step: compute the set of branch cuts. This uses the center_list C to
	compute canonical paths to 0; an alternative would be to choose some combinatorial
	geodesic path (this would be slower) 
	
	for(i=0;i<(int) B.v.size();i++){	// for each branch point
		cout << "for branch point " << B.v[i] << " determining path to zero. \n";
		X=path_to_zero(P,C,B.v[i]);		// determine the path to zero
		write_path(X);
		Y.X.push_back(X);				// and add it to the list of branch paths.
	};	*/
	
	compute_branch_cuts(Y,P,B,C);
	
	/* second step: initialize Q as the product cover P x {0, . . , d-1} */
	
	initialize_trivial_cover(Q,P,branch_degree);
	
	/* third step: for each vertex on a branch cut, adjust value of neighbor */
	
	// generic case
	for(i=0;i<(int) B.v.size();i++){	// for each branch point
		sigma=B.b[i];	// read in monodromy permutation about branch point i
		X=Y.X[i];		// read in branch path
		for(sheet=0;sheet<branch_degree;sheet++){
			if(sigma.p[sheet]!=sheet){	// if monodromy around branch point i does not fix sheet sheet,
				new_sheet=sigma.p[sheet];	// let new_sheet be the new sheet
				for(l=1;l<(int) X.v.size()-1;l++){	// for each point on *interior* of branch cut
					m=X.v[l];	// m is vertex on branch cut i
					cout << "(" << m << "," << sheet << ")\n";
	
					/*
					for all neighbors o of m on the positive side of the branch, replace
					the edge (m,sheet) -> (o,sheet) with (m,sheet) -> (o,sigma(sheet)) and replace the
					edge (o,sigma(sheet)) -> (m,sigma(sheet)) with (o,sigma(sheet)) -> (m,sheet).
					*/
					
					valence=P.v[m].a.size();	// valence of m
					next_m=which_index(P,m,X.v[l+1]);	// P.v[m].a[next_m]=X.v[l+1]
					prev_m=which_index(P,m,X.v[l-1]);	// P.v[m].a[prev_m]=X.v[l-1]
					if(prev_m<next_m){
						prev_m=prev_m+valence;
					};
					for(n=next_m+1;n<prev_m;n++){
						o=P.v[m].a[n%valence];
						r=which_index(P,o,m);
						cout << "adjacency index " << r << " has index " << o << "\n";
						Q.v[naive_new_index(size_of_P,m,sheet)].a[n%valence]=naive_new_index(size_of_P,o,new_sheet);	
						Q.v[naive_new_index(size_of_P,o,new_sheet)].a[r]=naive_new_index(size_of_P,m,sheet);
					};
				};
			};
		};
	};
	
	// special case for vertex 0 
	for(sheet=0;sheet<branch_degree;sheet++){
		current_sheet=sheet;
		cout << "(0," << sheet << ")\n";
		valence=P.v[0].a.size();	// valence of 0
		for(i=0;i<valence;i++){
			j=P.v[0].a[i];
			k=which_index(P,j,0);
			cout << "adjacency index " << k << " has index " << j << "\n";
			Q.v[naive_new_index(size_of_P,0,sheet)].a[i]=naive_new_index(size_of_P,j,current_sheet);
			Q.v[naive_new_index(size_of_P,j,current_sheet)].a[k]=naive_new_index(size_of_P,0,sheet);
			for(l=0;l<(int) Y.X.size();l++){
				if(j==Y.X[l].v[Y.X[l].v.size()-2]){	// is j on the branch cut?
					current_sheet = B.b[l].p[current_sheet];	// apply monodromy
				};
			};
		};
	};
	
	// special case for each branch vertex 
	for(sheet=0;sheet<branch_degree;sheet++){
		for(i=0;i<(int) B.v.size();i++){	// for each branch point 
			j=B.v[i];						// which we call j
			valence=P.v[j].a.size();
			current_sheet=sheet;
			cout << "(" << j << "," << sheet << ")\n";
			k=orbit_size(B.b[i],sheet);
			if(k>1){			// if this is a genuine branch point
	//			Q.v[naive_new_index(size_of_P,j,sheet)].a.clear();
				A.a.clear();	// initialize adjacency list
				for(l=0;l<k*valence;l++){	
					m=P.v[j].a[l%valence];
					cout << "adjacency index " << l << " has index " << m << "\n";
					A.a.push_back(naive_new_index(size_of_P,m,current_sheet));
					if(m==Y.X[i].v[1]){	// if a[l] is on the branch cut,
						current_sheet=B.b[i].p[current_sheet];	// apply monodromy
					};
				};
				Q.v[naive_new_index(size_of_P,j,sheet)]=A;	// copy modified adjacency list
			};
		};
	};
	
	/* fourth step: prune the "extraneous" elements of Q by removing redundant
	vertices (those corresponding to (v,s) where v is a branch point and s > min_s)
	and adjusting indices of things pointing to them. */
	
	
	vector<int> E,F;
	generate_elimination_list(P,B,E,F);
	remove_spurious_points(E,F,Q);	

	return(Q);
};
