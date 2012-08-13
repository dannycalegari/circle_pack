/* layout.cc layout algorithms 

   There are *two* orderings on the set of circles. Let P be a packing, and L an associated layout_data.
   
   vertex order: P.v indexes the circles in the order in which they are read in from a file.
   layout order: L.a indexes the circles in the order in which they are laid out.
   
   center_list is first computed using layout order, since centers are determined inductively
   from the list of radii and where earlier circles (in the layout order) have been placed.
   
   After doing this, I *should* reorder center_list so that its entries are in vertex order.
*/

struct layout_data{		// a[i] rests on b[i] and c[i] (for i>1) where all three form a triangle.
	vector<int> a;		// a[i] is vertex order for layout order i
	vector<int>	b;		// b[i] is vertex order of circle that vertex order a[i] rests on
	vector<int>	c;		// c[i] is vertex order of second circle that vertex order a[i] rests on
	vector<int> Beta;	// Beta[i] is the index of b[i] in the layout order; i.e. L.a[L.Beta[i]]=P.v[L.b[i]]
	vector<int> Gamma;	// Gamma[i] is the index of c[i] in the layout order; i.e. L.a[L.Gamma[i]]=P.v[L.c[i]]
};

vector<int> invert(layout_data L){	// L.a is a permutation of 0 . . P.v.size(); this returns the inverse
	vector<int> v;
	int i,j,k;
	i=L.a.size();
	for(j=0;j<i;j++){				// for each integer j
		for(k=0;k<i;k++){			// go through the indices of L
			if(L.a[k]==j){			// until we find the one whose value is j
				v.push_back(k);		// and push it on the vector v.
			};
		};
	};
	return(v);
};

struct point {	// a point has x and y coordinates
	double x;
	double y;
};

point operator+(point p, point q){
	point r;
	r.x=p.x+q.x;
	r.y=p.y+q.y;
	return(r);
};

point operator-(point p, point q){
	point r;
	r.x=p.x-q.x;
	r.y=p.y-q.y;
	return(r);
};


double radius_squared(point p){
	return(p.x*p.x+p.y*p.y);
};

struct center_list {	// list of centers, presumably arising from the data of a packing.
	vector<point> p;	
};

bool already_laid_out(layout_data L, int i){	// has i already been laid out?
	int j;
	bool is_it_here;
	is_it_here=false;	// initial value; if returned, it means false
	for(j=0;j<(int) L.a.size();j++){
		if(L.a[j]==i){
			is_it_here=true;
		};
	};
	return(is_it_here);
};

int layout_index(layout_data L, int i){
	int j,k;
	k=-1;
	if(already_laid_out(L,i)==false){
		k=-1;
	} else {
		for(j=0;j<(int) L.a.size();j++){
			if(L.a[j]==i){
				k=j;
			};
		};	
	};
	return(k);
};

bool try_to_add(packing P, layout_data &L, int i){	
	int j,valence;
	bool successfully_added;
	successfully_added=false;	// initial value
	valence=P.v[i].a.size();
	for(j=0;j<valence;j++){
		if(already_laid_out(L,P.v[i].a[j]) && already_laid_out(L,P.v[i].a[(j+1)%valence])){
			L.a.push_back(i);	
			L.b.push_back(P.v[i].a[j]);
			L.c.push_back(P.v[i].a[(j+1)%valence]);
	//		cout << "layout index " << layout_index(L,P.v[i].a[j]) << " " << layout_index(L,P.v[i].a[(j+1)%valence]) << "   ";
			L.Beta.push_back( layout_index(L,P.v[i].a[j]) );
			L.Gamma.push_back( layout_index(L,P.v[i].a[(j+1)%valence]) );
			successfully_added=true;
			break;
		};
	};
	return(successfully_added);
};

void layout_order(packing P, layout_data &L){
	int number_of_vertices,i;
	number_of_vertices=P.v.size();
	bool tried_to_add;
	
	L.a.clear();	// initialize data
	L.b.clear();
	L.c.clear();
	L.Beta.clear();
	L.Gamma.clear();
	
	L.a.push_back(0);	// add first vertex by hand
	L.b.push_back(-1);	// dummy indicating vertex is a special case
	L.c.push_back(-1);	// another dummy
	L.Beta.push_back(-1);
	L.Gamma.push_back(-1);

	L.a.push_back(P.v[0].a[0]);	// add second vertex by hand
	L.b.push_back(0);
	L.c.push_back(-1);	// another dummy
	L.Beta.push_back(-1);
	L.Gamma.push_back(-1);

	while((int) L.a.size()<number_of_vertices){
		for(i=0;i<number_of_vertices;i++){	// try to add them in order
			if(already_laid_out(L,i)==false){	// we only try to add vertices that haven't yet been added
				tried_to_add=try_to_add(P,L,i);
			};
		};
	};
};

double slope(point p1, point p2){
	if(p2.y-p1.y==0.0 && p2.x-p1.x==0.0){
		return(0.0);
	} else {
		return(atan2(p2.y-p1.y,p2.x-p1.x));
	};
};

center_list reorder(layout_data L, center_list C){	// takes a center_list in layout order to vertex order
	vector<int> l;	
	int i;
	center_list D;
	
	l=invert(L);
	for(i=0;i<(int) L.a.size();i++){
		D.p.push_back(C.p[l[i]]);
	};
	return(D);
};


center_list determine_centers(packing P, layout_data L){
	int i,a,b,c,Beta,Gamma;
	double theta,phi,S,R1,R2,R3;
	center_list C;
	point p;
	
	p.x=0.0;
	p.y=0.0;
	C.p.push_back(p);		// first circle is centered at the origin
	
	p.x=P.r[L.a[1]]+P.r[L.b[1]];
	p.y=0.0;
	C.p.push_back(p);		// second circle is centered to the right of the first circle
	
	for(i=2;i<(int) L.a.size();i++){	// determine C in layout order
		a=L.a[i];
		b=L.b[i];
		c=L.c[i];
		Beta=L.Beta[i];
		Gamma=L.Gamma[i];


		R1=P.r[a]+P.r[b];
		R2=P.r[a]+P.r[c];
		R3=P.r[b]+P.r[c];
		S=(R1*R1+R3*R3-R2*R2)/(2.0*R1*R3);

		if(S>1.0){				// acos is very strict about domain
			theta=0.0;			// so if numerical error puts S outside [-1,1]
		} else if(S<-1.0){		// we have to push it back in
			theta=3.1415926;
		} else {
			theta=acos(S);	// Euclidean law
		};
			
		if(b==0){
			theta=-theta;
		}; 
			
		phi=slope(C.p[Beta],C.p[Gamma])+theta;
			
	//	cout << slope(C.x[Beta],C.y[Beta],C.x[Gamma],C.y[Gamma]) << " " << theta << " \n";

		if(b*c==0){
			phi=phi+3.1415926;
		}; 
		p.x=C.p[Beta].x+R1*cos(phi);
		p.y=C.p[Beta].y+R1*sin(phi);
		C.p.push_back(p);
		
	};
	
	C=reorder(L,C);	// now put C in vertex order
	return(C);
};

int closest_center(center_list C, point p){	// returns index of center closest to p
	int i, current_closest;
	current_closest=1;	// initial value
	for(i=1;i<(int) C.p.size();i++){
		if(radius_squared(p-C.p[i])<radius_squared(p-C.p[current_closest])){
			cout << "testing index " << i << "\n";
			cout << radius_squared(p-C.p[i]) << " is smaller than " <<
			radius_squared(p-C.p[current_closest]) << "\n";
			current_closest=i;
		};
	};
	return(current_closest);
};

