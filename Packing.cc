/*	file Packing.cc 	

	New attempt to write a better class for a packing, and a better algorithm.
	For the moment, we distinguish the new Packing from the old packing by using a capital
	initial letter.
	
	Function "find_radii" implements the algorithm described in 
		C. Collins, K. Stephenson "A circle packing algorithm",
		Comp. Geom. 25 (2003) 233--256
		
	To do: write method to lay out inner hyperbolic circles, and then
	transform to a Euclidean or Spherical packing.
	
	*/


class Packing {
		vector< vector<int> > adj;		// list of adjacency lists.
										// if adj[i][e]=j then edge e from vertex i lands at vertex j
	//	vector< vector<int> > edj;		// if edj[i][e]=f then edge f from vertex adj[i][e] lands at vertex i
		vector< double > rad;			// list of radii.
		vector< double > lab;			// list of labels. lab[i]=e^{-2rad[i]} (used for hyperbolic calculations)
		vector<int>	inner;				// list of vertices not adjacent to 0
		char geometry;	// can be E for Euclidean, S for Spherical or H for hyperbolic
	//	vector<int> layout;
	public:
		int which_edge(int,int);
		void determine_inner_vertices();
		void determine_labels();	// work with labels
		void determine_radii();
		void rescale();
		double fitness();
		double interior_fitness();
		double angle(int);
		double wedge(int,int);
		double hyperbolic_wedge(int,int);	// hyperbolic wedge is computed with labels
		void hyperbolic_correct_radius(int);
		void Euclidean_correct_radius(int);
		void find_radii(double);	// adjust until fitness <= accuracy
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

void Packing::determine_inner_vertices(){
	int i;
	inner.clear();
	for(i=1;i<(int) adj.size();i++){
		if(which_edge(i,0)==-1){
			inner.push_back(i);
		};
	};
};

void Packing::determine_labels(){
	int i;
	lab.clear();
//	cout << "cleared labels \n";
	lab.push_back(0.0);
	for(i=1;i<(int) adj.size();i++){
		lab.push_back(exp(-2.0*rad[i]));
//		cout << rad[i] << " " << lab[i] << "\n";
	};
};

void Packing::determine_radii(){
	int i,j;
	for(i=0;i<(int) inner.size();i++){
		j=inner[i];
		rad[j]=-log(lab[j])/2.0;
	};
};

void Packing::rescale(){	// rescales radii so outer circle has radius -2.0
	double R;
	int i;
	R=-2.0/rad[0];
	for(i=0;i<(int) rad.size();i++){
		rad[i]=rad[i]*R;
	};
};

double angle_at_C(double a, double b, double c, char geometry){	
	// in a triangle with side lengths a,b,c, what is the angle at C (i.e. opposite c)?
	double t,u;
	if(geometry=='E'){	// Euclidean geometry
		// Euclidean cosine rule: c*c = a*a + b*b - 2*a*b*cos(C)
		// so C = acos((a*a + b*b - c*c)/(2*a*b));
		t=(a*a+b*b-c*c)/(2.0*a*b);
		return(acos(t));
	} else if(geometry=='S'){	// Spherical geometry
		// Spherical cosine rule: cos(c) = cos(a)*cos(b)+sin(a)*sin(b)*cos(C)
		// so C=acos((cos(c)-cos(a)*cos(b))/(sin(a)*sin(b)))
		u=sin(a)*sin(b);
		t=(cos(c)-(cos(a)*cos(b)))/u;
		return(acos(t));
	} else if(geometry=='H'){	// Hyperbolic geometry
		// Hyperbolic cosine rule: cosh(c) = cosh(a)*cosh(b)-sinh(a)*sinh(b)*cos(C)
		// so C=acos((cosh(c)-cosh(a)*cosh(b))/(sinh(a)*sinh(b)))
		u=-sinh(a)*sinh(b);
		t=(cosh(c)-(cosh(a)*cosh(b)))/u;
		return(acos(t));
	};
	return(0.0);
};

double Packing::wedge(int i,int e){	// angle at vertex i between edge e and e+1
	int j,k;	// i -e-> j, i -(e+1)-> k
	j=adj[i][e];
	k=adj[i][(e+1)%adj[i].size()];	
	return(angle_at_C(rad[i]+rad[j],rad[i]+rad[k],rad[j]+rad[k],geometry));
};

double Packing::hyperbolic_wedge(int i,int e){	// angle at vertex i between e and e+1
	int j,k;	// i -e-> j, i -(e+1)-> k
	j=adj[i][e];
	k=adj[i][(e+1)%adj[i].size()];	
	double v,u,w;
	v=lab[i];
	u=lab[j];
	w=lab[k];
	return(2.0*asin(sqrt(v*(1.0-u)*(1.0-w)/((1.0-v*u)*(1.0-v*w)))));
};

double Packing::angle(int i){	// angle at vertex i; for correct packing, this should be 2pi
	int e;
	double angle;
	angle=0.0;	// initialize
	for(e=0;e<(int) adj[i].size();e++){
		if(geometry=='E' || geometry=='S'){
			angle=angle+wedge(i,e);
		} else {
			angle=angle+hyperbolic_wedge(i,e);
		};
	};
	return(angle);
};


double Packing::fitness(){
	int i;
	double fitness;
	fitness=0.0;
	for(i=0;i<(int) adj.size();i++){
		fitness=fitness+fabs(TWOPI-angle(i));
	};
	return(fitness);
};

double Packing::interior_fitness(){
	int i;
	double fitness;
	fitness=0.0;
	for(i=1;i<(int) adj.size();i++){
		if(which_edge(i,0)==-1){
			fitness=fitness+fabs(TWOPI-angle(i));
		};
	};
	return(fitness);
};	

void Packing::hyperbolic_correct_radius(int j){
	double beta, delta;
	double dk;
	double vhat;
	double t,u;
	double v;
	dk = (double) adj[j].size();
	beta=sin(angle(j)/(2.0*dk));
	delta=sin(TWOPI/(2.0*dk));
	v=lab[j];
//	cout << "beta " << beta << " delta " << delta << " v " << v << "\n";
	vhat=(beta-sqrt(v))/((beta*v)-sqrt(v));
//	cout << "vhat " << vhat << "\n";
	if(vhat<0){
//		cout << "negative vhat ";
		vhat=0.0;
	};
	t=2.0*delta/(sqrt((1.0-vhat)*(1.0-vhat)+4.0*delta*delta*vhat) + (1.0-vhat));
//	cout << "t " << t << "\n";
	u=t*t;
	lab[j]=u;
};

void Packing::Euclidean_correct_radius(int j){
	double beta, delta;
	double dk;
	double vhat;
	dk = (double) adj[j].size();
	beta=sin(angle(j)/(2.0*dk));
	delta=sin(TWOPI/(2.0*dk));	
	vhat=rad[j]*beta/(1.0-beta);
	rad[j]=vhat*(1.0-delta)/delta;
};

void Packing::find_radii(double accuracy){	// adjust until fitness <= accuracy
	int i,j,step;
	step=0;
	if(geometry=='H'){
		cout << "Hyperbolic geometry.\n";
		determine_inner_vertices();
		determine_labels();
		cout << "initial fitness is " << interior_fitness() << "\n";
		while(interior_fitness()>accuracy){
			for(i=0;i<(int) inner.size();i++){
				j=inner[i];
				hyperbolic_correct_radius(j);
			};
			for(i=0;i<(int) adj[0].size();i++){
				j=adj[0][i];
				if(lab[j]>0.0000000001){
					lab[j]=lab[j]/2.0;
		//			lab[j]=lab[j]*lab[j];
				};
		//		cout << lab[j] << " ";
		//		cout << "new label " << j << " is " << lab[j] << "\n";
			};
			step++;
			if(step%100==0){
				cout << "fitness is " << interior_fitness() << " after " << step << " steps. \n";
			};
		//	usleep(1000000);
		};
		cout << "fitness is " << interior_fitness() << " after " << step << " steps. \n";
	} else if (geometry=='E'){
		cout << "Euclidean geometry.\n";
		cout << "initial fitness is " << fitness() << "\n";
		while(fitness()>accuracy){	
			for(i=0;i<(int) adj.size();i++){
				Euclidean_correct_radius(i);
			};	
			step++;
			if(step%100==0){
				cout << "fitness is " << fitness() << " after " << step << " steps. \n";
			};
		};
		cout << "fitness is " << fitness() << " after " << step << " steps. \n";
	} else {
		cout << "Spherical packing not implemented yet (move a vertex to infinity)\n";
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
		rad.push_back(0.1);	// setting all initial radii to 0.1
	};
//	rescale();
	geometry='H';
	return;
};