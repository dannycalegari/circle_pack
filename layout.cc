/* layout.cc layout algorithms 

   There are *two* orderings on the set of circles. Let P be a packing, and L an associated layout_data.
   P.v indexes the circles in the order in which they are read in from a file.
   L.a indexes the circles in the order in which they are laid out.
   
   Is there a good reason for this? Probably it would be faster to rename the circles in the layout order
   after that order is determined.
*/

struct layout_data{		// a[i] rests on b[i] and c[i] (for i>1) where all three form a triangle.
	vector<int> a;		// index for layout order. Circles are laid out in order P.v[L.a[0]], P.v[L.a[1]], . . .
	vector<int>	b;
	vector<int>	c;
	vector<int> Beta;	// Beta[i] is the index of b[i] in the layout order; i.e. L.a[L.Beta[i]]=P.v[L.b[i]]
	vector<int> Gamma;	// Gamma[i] is the index of c[i] in the layout order; i.e. L.a[L.Gamma[i]]=P.v[L.c[i]]
};

vector<int> invert(layout_data L){	// L.a is a permutation of 0 . . P.v.size(); this returns the inverse
	vector<int> v;
	int i,j,k;
	i=L.a.size();
	for(j=0;j<i;j++){
		for(k=0;k<i;k++){
			if(L.a[k]==j){
				v.push_back(k);
			};
		};
	};
	return(v);
};

struct center_list {	// list of centers, presumably arising from the data of a packing.
	vector<double> x;	// note that centers are listed *in the order in which they are laid out*
	vector<double> y;
};

bool already_laid_out(layout_data L, int i){	// has i already been laid out?
	int j;
	bool is_it_here;
	is_it_here=false;	// initial value; if returned, it means false
	for(j=0;j<L.a.size();j++){
		if(L.a[j]==i){
			is_it_here=true;
		};
	};
	return(is_it_here);
};

int layout_index(layout_data L, int i){
	int j;
	if(already_laid_out(L,i)==false){
		return(-1);
	} else {
		for(j=0;j<L.a.size();j++){
			if(L.a[j]==i){
				return(j);
			};
		};	
	};
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
	int number_of_vertices,i,j;
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

	while(L.a.size()<number_of_vertices){
		for(i=0;i<number_of_vertices;i++){	// try to add them in order
			if(already_laid_out(L,i)==false){	// we only try to add vertices that haven't yet been added
				tried_to_add=try_to_add(P,L,i);
			};
		};
	};
};

double slope(double x1, double y1, double x2, double y2){
	if(y2-y1==0.0 && x2-x1==0.0){
		return(0.0);
	} else {
		return(atan2(y2-y1,x2-x1));
	};
};

void determine_centers(packing P, layout_data L, center_list &C){
	int i,a,b,c,Beta,Gamma;
	double theta,phi,S,R1,R2,R3;
	C.x.clear();
	C.y.clear();
	
	C.x.push_back(0.0);		// first circle is centered at the origin
	C.y.push_back(0.0);	
	
	C.x.push_back(P.r[L.a[1]]+P.r[L.b[1]]);
	C.y.push_back(0.0);		// second circle is centered to the right of the first circle
	
	for(i=2;i<L.a.size();i++){
		a=L.a[i];
		b=L.b[i];
		c=L.c[i];
		Beta=L.Beta[i];
		Gamma=L.Gamma[i];
	//	Beta=layout_index(L,b);
	//	Gamma=layout_index(L,c);
	//	cout << a << " " << b << " " << c << " " << Beta << " " << Gamma << "\n";


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
			
		phi=slope(C.x[Beta],C.y[Beta],C.x[Gamma],C.y[Gamma])+theta;
	//	cout << slope(C.x[Beta],C.y[Beta],C.x[Gamma],C.y[Gamma]) << " " << theta << " \n";

		if(b*c==0){
			phi=phi+3.1415926;
		}; 
		C.x.push_back(C.x[Beta]+R1*cos(phi));
		C.y.push_back(C.y[Beta]+R1*sin(phi));
	};
};

