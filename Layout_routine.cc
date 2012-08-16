/* Layout.cc layout functions for Packing */

//struct Edge {
//	int i,j;	// edge from vertex i to vertex j
//};

//	vector<Edge> layout_order;		// layout data

//int min(int i, int j){
//	
//};

void Packing::compute_distances_to_vertex_0(){
	bool all_found, one_not_done;
	int i,j,nearest;
	if(verbose){
		cout << "computing distances to vertex 0.\n";
	};
	dist.clear();				// clear distance list
	maximum_distance=0;					// reset maximum
	dist.push_back(0);
	for(i=1;i<(int) adj.size();i++){	// initialize distances
		dist.push_back(-1);
	};
	all_found=false;
	while(all_found==false){
		one_not_done=false;
		for(i=0;i<(int) adj.size();i++){	// for each vertex i
			if(dist[i]==-1){		// if we don't yet know the distance to 0
				one_not_done=true;			// note this fact
				nearest=1000000;			// initialize guess
				for(j=0;j<(int) adj[i].size();j++){	// for all neighbors of i
					if(dist[adj[i][j]]!=-1){
						nearest=nearest < dist[adj[i][j]] ? nearest : dist[adj[i][j]];
					};
				};
				if(nearest<1000000){		// if we found a neighbor
					dist[i]=nearest+1;
					maximum_distance= maximum_distance > nearest+1 ? maximum_distance : nearest+1;	// let's remember this while we're at it
				};
			};
		};
		if(one_not_done==false){
			all_found=true;
		};
	};
};

void Packing::write_distances_to_vertex_0(){
	int i;
	for(i=0;i<(int) adj.size();i++){
		cout << "vertex " << i << " is distance " << dist[i] << " to vertex 0.\n";
	};
	cout << "maximum distance is " << maximum_distance << "\n";
};

bool Packing::already_laid_out(int i){
	int j;
	bool found_index;
	found_index=false;
	for(j=0;j<(int) lay.size();j++){
		if(lay[j][0]==i){	// did we lay it out?
			found_index=true;
		};
	};
	return(found_index);
};

void Packing::determine_layout(){			// figure out layout order of vertices
	int first,second;
	int i,j,k,e;
	vector<int> triangle;
	if(verbose){
		cout << "determining layout order.\n";
	};
	if(geometry=='H'){	// don't lay out circle 0
		lay.clear();	// initialize layout vector
		compute_distances_to_vertex_0();
		for(i=0;i<(int) adj.size();i++){
			if(dist[i]==maximum_distance){
				first=i;					
			};
		};
		triangle.clear();
		triangle.push_back(first);
		triangle.push_back(-1);
		triangle.push_back(-1);
		lay.push_back(triangle);	// add first triangle (special case)
		second=adj[first][0];
		triangle.clear();
		triangle.push_back(second);
		triangle.push_back(first);
		triangle.push_back(-1);
		lay.push_back(triangle);	// add second triangle (special case)
		while((int) lay.size() < (int) adj.size()-1){	// hyperbolic geometry: don't lay out 0!
			for(i=1;i<(int) adj.size();i++){	
				if(already_laid_out(i)==false){		// not yet laid out
					for(e=0;e<(int) adj[i].size();e++){
						j=adj[i][e];
						k=adj[i][(e+1)%(int) adj[i].size()];
						if(already_laid_out(j) && already_laid_out(k)){	// neighbors are both laid out
							triangle.clear();
							triangle.push_back(i);
							triangle.push_back(j);
							triangle.push_back(k);
							lay.push_back(triangle);
							e=(int) adj[i].size();	// don't add it twice!
						};
					};
				};
			};
			if(verbose){
				cout << "total laid out: " << lay.size() << " out of " << adj.size()-1 << "\n";
			};
		};
	};
};

Point layout_point(double RI, Point J, double RJ, Point K, double RK, char geometry){
	// triangle JKI; i.e. JK is bottom edge. 
	// given location of J and K, and side lengths (implicitly), compute location of I
	Point I;
	Point J0,K0,I0,K1;
	Matrix M;
	double a,b,c,angle;
	a=RJ+RK;
	b=RJ+RI;
	c=RI+RK;
	angle=angle_at_C(a,b,c,geometry);
	J0.x=0.0;
	J0.y=0.0;
	J0.z=1.0;
	K0=HTR(a)(J0);
	I0=(ROT(angle)*HTR(b))(J0);	// (J0,K0,I0) have correct relative position. 
		// need to compute M in SO(2,1) with M(J0,K0,I0) = (J,K,I);
	M=HTR(-hyp_dist(J))*ROT(-hyp_ang(J));	// M moves J to origin
	K1=M(K);
	M=ROT(-hyp_ang(K1))*M;
	I=M.hyp_inv()(I0);	// apply inverse of M to I0
	return(I);
};

void Packing::initialize_centers(){
	if(verbose){
		cout << "initializing centers to 0.\n";
	};
	Point P;
	int i;
	P.x=0.0;
	P.y=0.0;
	P.z=1.0;
	for(i=0;i<(int) adj.size();i++){
		center.push_back(P);	// initialize center to origin
	};
};

void Packing::determine_centers(){			// figure out centers of vertices
	Point P;
	int i,j,k,l;
	double d;
	initialize_centers();
	if(verbose){
		cout << "determining centers.\n";
	};
	if(geometry=='H'){
		cout << "hyperbolic geometry.\n";
	};
	i=lay[0][0];	// special case
	j=lay[1][0];	// special case
	P.x=0.0;
	P.y=0.0;
	P.z=1.0;
	// P=ORIGIN;
	center[i]=P;
//	cout << "center " << 0 << " index " << i << " laid out at " << P.x << " " << P.y << " " << P.z << "\n";
	d=rad[i]+rad[j];
	P=HTR(d)(P);
	center[j]=P;
//	cout << "center " << 1 << " index " << j << " laid out at " << P.x << " " << P.y << " " << P.z << "\n";
	for(l=2;l<(int) lay.size();l++){
		i=lay[l][0];
		k=lay[l][1];
		j=lay[l][2];
		P=layout_point(rad[i], center[j], rad[j], center[k], rad[k], geometry);
		center[i]=P;
//		cout << "center " << l << " index " << i << " laid out on " << j << " and " << k << " at " << P.x << " " << P.y << " " << P.z << "\n";
	};
};

void Packing::write_centers(){
	cout << "geometry is " << geometry << "\n";
	int i;
	for(i=0;i<(int) adj.size();i++){
		cout << "vertex " << i << " has center " << center[i].x << " " << center[i].y << " " << center[i].z << "\n";
	};
};

void Packing::change_geometry(char new_geometry){
	Point Pcent,Pin,Pout,ORIGIN;
	double d,theta;
	double R;
	int i;
	if(verbose){
		cout << "changing geometry from " << geometry << " to " << new_geometry << "\n";
	};
	ORIGIN.x=0.0;
	ORIGIN.y=0.0;
	ORIGIN.z=1.0;
	if(new_geometry=='E' && geometry=='H'){
		for(i=1;i<(int) adj.size();i++){	// for each point
			Pcent=center[i];	// hyperbolic center
			d=hyp_dist(Pcent);		// hyperbolic distance to 0
			theta=hyp_ang(Pcent);	// hyperbolic angle from 0
			R=rad[i];		// hyperbolic radius
			Pin=(ROT(theta)*HTR(d-R))(ORIGIN);
			Pin=hyperboloid_to_Poincare(Pin);
			Pout=(ROT(theta)*HTR(d+R))(ORIGIN);
			Pout=hyperboloid_to_Poincare(Pout);
			Pcent=midpoint(Pin,Pout);
			R=Euc_dist(Pin,Pout)/2.0;
//				sqrt((Pin.x-Pout.x)*(Pin.x-Pout.x)+(Pin.y-Pout.y)*(Pin.y-Pout.y))/2.0;
			center[i]=Pcent;	// Euclidean center
			rad[i]=R;			// Euclidean radius
		};
		geometry='E';
	} else if(new_geometry=='S' && geometry=='H'){
		rad[0]=PI/2.0;
		for(i=1;i<(int) adj.size();i++){	// for each point
			Pcent=center[i];	// hyperbolic center
			d=hyp_dist(Pcent);		// hyperbolic distance to 0
			theta=hyp_ang(Pcent);	// hyperbolic angle from 0
			R=rad[i];		// hyperbolic radius
			Pin=(ROT(theta)*HTR(d-R))(ORIGIN);
			Pin=hyperboloid_to_sphere(Pin);
			Pout=(ROT(theta)*HTR(d+R))(ORIGIN);
			Pout=hyperboloid_to_sphere(Pout);
			Pcent=midpoint(Pin,Pout);
			Pcent=Pcent/norm(Pcent);
			R=sph_dist(Pin,Pout)/2.0;
			center[i]=Pcent;	// Spherical center
			rad[i]=R;			// Spherical radius
		};
		geometry='S';	
	};	
};