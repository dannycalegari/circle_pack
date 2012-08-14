/*	file potato.cc 	

	Definitions of combinatorial data structures associated to a potato packing, and simple
	functions to produce one packing from another. */


class ppacking {
		vector< vector<int> > adj;		// list of adjacency lists.
										// if adj[i][e]=j then edge e from vertex i lands at vertex j
		vector< vector<double> > rad;	// list of half-radius lists.
										// rad[i][e]=r is the length from i to the "mid"point of edge e
		char geometry;	// can be either E for Euclidean or S for Spherical
	public:
		int which_edge(int,int);
		double fitness();
		double angle(int);
		double wedge(int,int);
		void adjust_vertex(int);			// adjust single vertex
		vector<int> determine_bad_vertices(double);	// which vertices have bad angles?
		void adjust_radii(vector<int> &);	// adjust radii at specific vertices
		void solve_radii(double);		// determine correct placement, up to specified accuracy
};

int ppacking::which_edge(int i, int j){	// returns e if adj[i][e]=j and returns -1 if there is no e.
	int e;
	bool found_yet;
	found_yet=false;
	for(e=0;e<(int) adj[i].size();e++){
		if(adj[i][e]==j){
			found_yet=true;
			return(e);
		};
	};
	if(found_yet==false){
		return(-1);
	};
};

double angle_at_C(double a, double b, double c, char geometry){	
	// in a triangle with side lengths a,b,c, what is the angle at C (i.e. opposite c)?
	double t,u;
	if(geometry=='E'){	// Euclidean geometry
		// Euclidean cosine rule: c*c = a*a + b*b - 2*a*b*cos(C)
		// so C = acos((a*a + b*b - c*c)/(2*a*b));
		t=(a*a+b*b-c*c)/(2.0*a*b);
		if(t>1.0){
			return(0.0);
		} else if(t<-1.0){
			return(3.14159265358979);
		} else {
			return(acos(t));
		};
	} else if(geometry=='S'){	// Spherical geometry
		// Spherical cosine rule: cos(c) = cos(a)*cos(b)+sin(a)*sin(b)*cos(C)
		// so C=acos((cos(c)-cos(a)*cos(b))/(sin(a)*sin(b)))
		u=sin(a)*sin(b);
		if(u==0.0){		// degenerate case
			return(0.0);
		} else {
			t=(cos(c)-cos(a)*cos(b))/u;
			if(t>1.0){
				return(0.0);
			} else if(t<-1.0){
				return(3.14159265358979);
			} else {
				return(acos(t));
			};
		};
	};
};

double ppacking::wedge(int i,int e){	// angle at vertex i between edge e and e+1
	int j,k;	// i -e-> j, i -(e+1)-> k
	double ijrad,jirad,ikrad,kirad,jkrad,kjrad;
	j=adj[i][e];
	k=adj[i][(e+1)%adj[i].size()];	
	ijrad=rad[i][e];
	ikrad=rad[i][(e+1)%adj[i].size()];
	jirad=rad[j][which_edge(j,i)];
	jkrad=rad[j][which_edge(j,k)];
	kjrad=rad[k][which_edge(k,j)];
	kirad=rad[k][which_edge(k,i)];
	
	return(angle_at_C(ijrad+jirad,ikrad+kirad,jkrad+kjrad,geometry));
};

double ppacking::angle(int i){	// angle at vertex i; for correct packing, this should be 2pi
	int e;
	double angle;
	angle=0.0;	// initialize
	for(e=0;e<(int) adj[i].size();e++){
		angle=angle+wedge(i,e);
	};
	return(angle);
};

double ppacking::fitness(){
	int i;
	double fitness;
	fitness=0.0;
	for(i=0;i<(int) adj.size();i++){
		fitness=fitness+fabs(6.28318530717959-angle(i));
	};
	return(fitness);
};

vector<int> ppacking::determine_bad_vertices(double accuracy){
	int i;
	vector<int> L;
	for(i=0;i<(int) adj.size();i++){
		if(fabs(6.28318530717959-angle(i))>accuracy){
			L.push_back(i);
		};
	};
	return(L);
};

void ppacking::adjust_vertex(int i){
	double scale;
	int j;
	scale=6.28318530717959/angle(i);
	for(j=0;j<(int) adj[i].size();j++){	// not a great way to adjust, but easy to implement
		rad[i][j]=rad[i][j]*scale;
	};
};

void ppacking::adjust_radii(vector<int> &L){		// adjust radii at specific list of vertices, and modify list
	int j,k;
	int initial_list_size;
	initial_list_size=L.size();
	vector<int> A;	// list of additions
	for(j=0;j<initial_list_size;j++){
		adjust_vertex(j);
		for(k=0;k<(int) adj[j].size();k++){
			A.push_back(k);
		};
	};
	L.insert(L.end(), A.begin(), A.end());
	sort(L.begin(),L.end());
	L.erase( unique( L.begin(), L.end() ), L.end() );
};

void ppacking::solve_radii(double accuracy){	// adjust until fitness <= accuracy
	vector<int> bad_list;
	double current_fitness;
	int i;
	while(current_fitness>accuracy){
		bad_list = determine_bad_vertices(accuracy);
		for(i=0;i<10;i++){
			adjust_radii(bad_list);	// bad list is locally added to
		};
	};
};
