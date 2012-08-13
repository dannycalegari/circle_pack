/*	branched_cover.cc (formerly edge_path.cc) algorithm to compute branched cover */

/* as it stands, this file is a complete hack; it can be easily made much better.
	To do: 
	
	1. the function new_index should be replaced by an approximation 
			new_index(i,s) = s*P.v.size() + i 
	which is then corrected at the end after all adjacency data is computed. 
	
	2. the function compute_sheet tests *every* vertex. instead the new packing should
	be written as the trivial branched covering (i.e. (i,s) is adjacent to (j,s) if
	and only if i is adjacent to j) and then adjusted *only* on indices i which are
	on a branch cut, or next to one. I.e. in effect we should build a trivial cover,
	then cut it and reglue it. Probably the easy thing to do is to write these
	functions from scratch. */
   
struct path{	// a path is an ordered list of integers
	vector<int> v;
};

struct branch_paths{	// an ordered list of paths
	vector<path> X;
};

path path_to_zero(packing P, center_list C, int v){	// determines canonical path from v to 0
	path X;
	int current_vertex,i,j;
	current_vertex=v;
	X.v.push_back(v);
	while(current_vertex!=0){	// if v=0 this is skipped
	//	cout << "testing to see if adjacent to 0 " << which_index(P,current_vertex,0) << "\n";
		if(which_index(P,current_vertex,0)!=-1){	// if 0 is adjacent to v
			X.v.push_back(0);
			current_vertex=0;
		} else {
			i=0;
			while(i<(int) P.v[current_vertex].a.size()){
				j=P.v[current_vertex].a[i];	// get neighbor j
	//			cout << "is vertex " << j << " further out than vertex " << current_vertex << "?\n";
	//			cout << "rsqr of " << j << " is " << radius_squared(C.p[j]) << "; rsqr of " << current_vertex 
	//				<< " is " << radius_squared(C.p[current_vertex]) << "\n";
				if(radius_squared(C.p[j])>radius_squared(C.p[current_vertex])){	// if j is further out
					X.v.push_back(j);
					current_vertex=j;
					i=(int) P.v[current_vertex].a.size();
				};
				i++;
			};
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

vector<int> itinerary(packing P, branch_data B, int v, int sheet){	// monodromy around vertex v on sheet s
	vector<int> I;
	int i,j;
	I.push_back(sheet);
	for(i=0;i<(int) B.v.size();i++){
		if(v==B.v[i]){		// if v is a branch vertex
			j=B.b[i].p[sheet];
			while(j!=sheet){
				I.push_back(j);
				j=B.b[i].p[j];
			};
		};
	};
	return(I);
};

int minimal_sheet(packing P, branch_data B, int v, int sheet){ 
	/* vertex in branched cover given by a pair (v,s). If v
	is not a branched point, this is unique, and minimal_sheet = s.
	Otherwise minimal_sheet is the minimal value over the itinerary
	I(P,B,v,s) which is a vector of the set of s' with (v,s)=(v,s') */
	vector<int> I;
	int i,least;
	I=itinerary(P,B,v,sheet);
	least=sheet;
	for(i=0;i<(int) I.size();i++){
		if(I[i]<least){
			least=I[i];
		};
	};
	return(least);
};

int new_index(packing P, branch_data B, int v, int sheet){	
	/* vertices in branched cover are given by a pair (v,s)
	where v is a vertex in the original packing, and s is a sheet,
	where s runs from 0 to degree-1. If v is a branch point, the
	sheet is not unique, but runs over the itinerary I(P,B,v,s).
	new_index(P,B,v,s) returns the index in the branched cover
	associated to (v,s). */
	int i,j,min_sheet,count;
	min_sheet=minimal_sheet(P,B,v,sheet);
	count=-1;
	for(i=0;i<=min_sheet;i++){
		if(i<min_sheet){
			for(j=0;j<(int) P.v.size();j++){
				if(minimal_sheet(P,B,j,i)==i){
					count++;
				};
			};
		} else {
			for(j=0;j<=v;j++){
				if(minimal_sheet(P,B,j,i)==i){
					count++;
				};
			};
		};
	};
	return(count);
};

bool lies_on(int i,path X){		// is index i on path X?
	int j;
	bool found_yet;
	found_yet=false;
	for(j=0;j<(int) X.v.size();j++){
		if(X.v[j]==i){
			found_yet=true;
		};
	};
	return(found_yet);
};

int previous(int i,path X){		// what lies before i on path X?
	int j;
	j=0;
	if(X.v[0]==i){	// is this the initial point of X?
		return(-1);
	} else {
		while(j<(int) X.v.size()){
			if(X.v[j]==i){		// when we find it,
				return(X.v[j-1]);	// return the previous one.
			};
			j++;
		};
	};
	return(-1);
};

int next(int i,path X){		// what lies after i on path X?
	int j;
	j=0;
	if(X.v[X.v.size()-1]==i){	// is this the terminal point of X?
		return(-1);
	} else {
		while(j<(int) X.v.size()){
			if(X.v[j]==i){		// when we find it,
				return(X.v[j+1]);	// return the previous one.
			};
			j++;
		};
	};
	return(-1);
};

bool circular_order(packing P, int i, int j, int l, int k){	// are (j,l,k) circularly ordered in link(i)?
	int valence,a,b,c,d,e,f;
	valence=(int) P.v[i].a.size();
	a=which_index(P,i,j);
	b=which_index(P,i,l);
	c=which_index(P,i,k);
	d=b-a;
	e=c-b;
	f=a-c;
	if(d<0){
		d=d+valence;
	};
	if(e<0){
		e=e+valence;
	};
	if(f<0){
		f=f+valence;
	};
	if(d+e+f==valence && d>0 && e>0 && f>0){	// points must be distinct and in the right order
		return(true);
	} else {
		return(false);
	};
};

int compute_sheet(packing P, branch_data B, branch_paths Y, int current_sheet, int this_neighbor, int next_neighbor){
	// when we move from this_neighbor to next_neighbor, how does current_sheet change?
	int i,j,k,S;
	int kk,c_s,t_n,n_n;
	int computed_sheet;
	cout << "computing how sheet changes when we move from " << this_neighbor << " to " << next_neighbor << " on sheet " << current_sheet << "\n";
	computed_sheet = current_sheet;	// initial value
	if(next_neighbor==0){	// special case
		/* need to find S so that compute_sheet(S, 0, this_neighbor) = current_sheet and return S */
		for(S=0;S<degree(B.b[0]);S++){		// test S sheet for 0
			kk=which_index(P,0,this_neighbor);

			c_s=S;
			for(k=0;k<kk;k++){		
				t_n=P.v[0].a[k];
				n_n=P.v[0].a[(k+1)%P.v[0].a.size()];
				c_s = compute_sheet(P, B, Y, c_s, t_n, n_n);
			};
			if(c_s==current_sheet){		// do we have a match?
				computed_sheet=S;
			};
		};		
	} else {	
		for(i=0;i<(int) Y.X.size();i++){
			if(lies_on(this_neighbor,Y.X[i])){	// only adjust sheet if we lie on a branch cut
				j=previous(this_neighbor,Y.X[i]);	// path Y.X[i] contains segment j -> this_neighbor -> k
				k=next(this_neighbor,Y.X[i]);	
				if(circular_order(P,this_neighbor,j,next_neighbor,k)){	
					// are j -> next_neighbor -> k in correct circular order in link(this_neighbor)?
					cout << "braiding positively around " << B.v[i] << "\n";
					computed_sheet=B.b[i].p[current_sheet];
					cout << "new current sheet is " << computed_sheet << "\n";
				};
			} else if(lies_on(next_neighbor,Y.X[i])){
				j=previous(next_neighbor,Y.X[i]);	// path Y.X[i] contains segment j -> next_neighbor -> k
				k=next(next_neighbor,Y.X[i]);	
				if(circular_order(P,next_neighbor,j,this_neighbor,k)){	
					// are j -> this_neighbor -> k in correct circular order in link(next_neighbor)?
					cout << "braiding negatively around " << B.v[i] << "\n";
					computed_sheet=inverse(B.b[i]).p[current_sheet];
					cout << "new current sheet is " << computed_sheet << "\n";
				};		
			};
		};
	};
	return(computed_sheet);
};

packing branched_cover(packing P, branch_data B, center_list C){
	packing Q;
	path X;
	branch_paths Y;
	int i,j,k;
	int branch_degree;
	int sheet, current_sheet;
	int this_neighbor, next_neighbor;
	vector<int> I;
	adjacency_list A;

	for(i=0;i<(int) B.v.size();i++){	// for each branch point
		cout << "for branch point " << B.v[i] << " determining path to zero. \n";
		X=path_to_zero(P,C,B.v[i]);		// determine the path to zero
		write_path(X);
		Y.X.push_back(X);				// and add it to the list of branch paths.
	};

	for(sheet=0;sheet<degree(B.b[0]);sheet++){	// for each sheet
		for(i=0;i<(int) P.v.size();i++){		// for each vertex in the packing P
			cout << "considering (" << i << "," << sheet << ")\n"; 	// consider vertex (i,sheet). 
			
			if(i==0){	// special case!!! treat it sort of like a branch point
				A.a.clear();
				current_sheet=sheet;
				for(k=0;k<(int) P.v[0].a.size();k++){
					this_neighbor=P.v[0].a[k];
					next_neighbor=P.v[0].a[(k+1)%P.v[0].a.size()];
					A.a.push_back(new_index(P,B,this_neighbor,current_sheet));
					current_sheet = compute_sheet(P, B, Y, current_sheet, this_neighbor, next_neighbor);
				};
				Q.v.push_back(A);
				Q.r.push_back(0.1);
			} else if (branch_point(B,i)){	// a branch point (other than 0)
				if(minimal_sheet(P,B,i,sheet)==sheet){	// If it is minimal in its itinerary, 
					cout << "minimal in its itinerary.\n";	// we need to compute its adjacency list and then add it.
					A.a.clear();	// clear the temporary adjacency list.
					I=itinerary(P,B,i,sheet);
					branch_degree=(int) I.size();
					cout << "itinerary has size " << branch_degree << "\n";

					current_sheet=sheet;			// initialize current sheet
					for(j=0;j<branch_degree;j++){	// loop branch_degree times
						cout << "on loop " << j << "\n";
						for(k=0;k<(int) P.v[i].a.size();k++){	// for each adjacent neighbor,
							cout << "looking at " << k << "th neighbor, which is " << this_neighbor << "\n";
							this_neighbor=P.v[i].a[k];
							A.a.push_back(new_index(P,B,this_neighbor,current_sheet));	// add to adjacency list
							next_neighbor=P.v[i].a[(k+1)%P.v[i].a.size()];
							current_sheet = compute_sheet(P, B, Y, current_sheet, this_neighbor, next_neighbor);	// how does sheet change?
						};
					};
					Q.v.push_back(A);
					Q.r.push_back(0.1);	
				};
			} else {	// an ordinary point, not 0
				A.a.clear();	// clear the temporary adjacency list.
				for(k=0;k<(int) P.v[i].a.size();k++){	// for each adjacent neighbor,
					this_neighbor=P.v[i].a[k];
					current_sheet=compute_sheet(P, B, Y, sheet, i, this_neighbor);	// need to modify this if this_neighbor is a branch point or 0
					A.a.push_back(new_index(P,B,this_neighbor,current_sheet));
				};		
				Q.v.push_back(A);
				Q.r.push_back(0.1);		
			};
		};
	};
	Q.r[0]=-1.0;
	return(Q);
};
