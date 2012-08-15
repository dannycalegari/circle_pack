/*	file Packing.cc 	

	Definitions of combinatorial data structures associated to an ordinary packing, and simple
	functions to produce one packing from another. */


class Packing {
		vector< vector<int> > adj;		// list of adjacency lists.
										// if adj[i][e]=j then edge e from vertex i lands at vertex j
		vector< double > rad;			// list of radii.
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
		void read_packing(ifstream &);	// read data from file
};

int Packing::which_edge(int i, int j){	// returns e if adj[i][e]=j and returns -1 if there is no e.
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
	return(-1);
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
	return(0.0);
};

double Packing::wedge(int i,int e){	// angle at vertex i between edge e and e+1
	int j,k;	// i -e-> j, i -(e+1)-> k
	j=adj[i][e];
	k=adj[i][(e+1)%adj[i].size()];	
	return(angle_at_C(rad[i]+rad[j],rad[i]+rad[k],rad[j]+rad[k],geometry));
};


double Packing::angle(int i){	// angle at vertex i; for correct packing, this should be 2pi
	int e;
	double angle;
	angle=0.0;	// initialize
	for(e=0;e<(int) adj[i].size();e++){
		angle=angle+wedge(i,e);
	};
	return(angle);
};


double Packing::fitness(){
	int i;
	double fitness;
	fitness=0.0;
	for(i=0;i<(int) adj.size();i++){
		fitness=fitness+fabs(6.28318530717959-angle(i));
	};
	return(fitness);
};

vector<int> Packing::determine_bad_vertices(double accuracy){
	int i;
	vector<int> L;
	for(i=0;i<(int) adj.size();i++){
		if(fabs(6.28318530717959-angle(i))>accuracy){
			L.push_back(i);
		};
	};
	return(L);
};

/* copy of function "correct_ratio" from trigonometry.cc
double correct_ratio(double A, int V){
	double current, desired;
	current=sqrt(1.0/(2.0*(1.0-cos(A/(double) V))))-0.5;
	desired=sqrt(1.0/(2.0*(1.0-cos(6.28318530717959/(double) V))))-0.5;
	return(desired/current);
};
*/

void Packing::adjust_vertex(int i){
	double t;
	t=correct_ratio(angle(i),adj[i].size());
	t=1.0+((t-1.0)*0.5);	// step_size hardcoded as 0.5
	rad[i]=rad[i]*t;
};

void Packing::adjust_radii(vector<int> &L){		// adjust radii at specific list of vertices, and modify list
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

void Packing::solve_radii(double accuracy){	// adjust until fitness <= accuracy
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

void Packing::read_packing(ifstream &packing_file){	// read packing from a file
	int vertices, valence, i, j, k;
	vector<int> L;	// adjacency_list
	double d;
	
	packing_file >> vertices;
	for(i=0;i<vertices;i++){
		packing_file >> valence;
		for(j=0;j<valence;j++){
			packing_file >> k;
			L.push_back(k);
		};
		adj.push_back(L);
		L.clear();
	};
	for(i=0;i<vertices;i++){
		packing_file >> d;
		rad.push_back(d);
	};
	return;
};