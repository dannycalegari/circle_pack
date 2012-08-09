/* branched_cover.cc	a bit of a hack right now */

struct permutation{	// a permutation is an ordering on {0,1, . . . , d-1}
	vector<int> p;	
};

permutation inverse(permutation P){
	permutation Q;
	Q=P;
	int i,j;
	for(i=0;i<P.p.size();i++){
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
	vector<permutation> b;	// monodromy around branch point i is permutation b[i]
};

vector<int> itinerary(packing P, branch_data B, int v, int sheet){	// monodromy around vertex v on sheet s
	vector<int> I;
	int i,j;
	I.push_back(sheet);
	for(i=0;i<B.v.size();i++){
//		cout << "testing branch vertex " << i << " ";
		if(v==B.v[i]){		// if v is a branch vertex
//			cout << "found a match! computing itinerary. ";
			j=B.b[i].p[sheet];
			while(j!=sheet){
				I.push_back(j);
				j=B.b[i].p[j];
//				cout << "added " << j << " ";
			};
		};
	};
	return(I);
};

int minimal_sheet(packing P, branch_data B, int v, int sheet){ // is this the smallest sheet on my itinerary?
	vector<int> I;
	int i,least;
	I=itinerary(P,B,v,sheet);
	least=sheet;
	for(i=0;i<I.size();i++){
		if(I[i]<least){
			least=I[i];
		};
	};
	return(least);
};

int new_index(packing P, branch_data B, int v, int sheet){	// index of (v,s) in branched cover
	int i,j,min_sheet,count;
	min_sheet=minimal_sheet(P,B,v,sheet);
	count=-1;
	for(i=0;i<=min_sheet;i++){
		if(i<min_sheet){
			for(j=0;j<P.v.size();j++){
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

bool is_to_the_left(double x1, double y1, double x2, double y2, double x3, double y3){
	if(((x3 - x1)*(y2 - y1) - (y3 - y1)*(x2 - x1)) < 0.0){
		return(true);
	} else {
		return(false);
	};
}

bool is_to_the_right(double x1, double y1, double x2, double y2, double x3, double y3){
	if(((x3 - x1)*(y2 - y1) - (y3 - y1)*(x2 - x1)) > 0.0){
		return(true);
	} else {
		return(false);
	};
}

bool crosses_left(double x1, double y1, double x2, double y2, double x3, double y3){	// does the line (x1,y1)->(x2,y2) cross the line (x3,y3)->(0.1,0.1)?
	double x4, y4;
	x4=0.1;
	y4=0.1;
	if(is_to_the_left(x1,y1,x2,y2,x3,y3) 
		&& is_to_the_left(x2,y2,x1,y1,x4,y4)
		&& is_to_the_left(x3,y3,x4,y4,x2,y2)
		&& is_to_the_left(x4,y4,x3,y3,x1,y1)){
		return(true);	
	} else {
		return(false);
	};
};

bool crosses_right(double x1, double y1, double x2, double y2, double x3, double y3){	// does the line (x1,y1)->(x2,y2) cross the line (x3,y3)->(0.1,0.1)?
	double x4, y4;
	x4=0.1;
	y4=0.1;
	if(is_to_the_right(x1,y1,x2,y2,x3,y3) 
		&& is_to_the_right(x2,y2,x1,y1,x4,y4)
		&& is_to_the_right(x3,y3,x4,y4,x2,y2)
		&& is_to_the_right(x4,y4,x3,y3,x1,y1)){
		return(true);	
	} else {
		return(false);
	};
};
	
int adjust_sheet(packing P, branch_data B, layout_data L, center_list C, int m, int n, int sheet){	// how does sheet change when we move from m to n?
	int i,j,new_sheet;
	vector<int> l;
	l=invert(L);
	new_sheet=sheet;
	if(m*n==0){	// special case; no branching at infinity
		return(sheet);
	} else {
		for(i=0;i<B.v.size();i++){
			j=B.v[i];
			if(crosses_left(C.x[l[m]],C.y[l[m]],C.x[l[n]],C.y[l[n]],C.x[l[j]],C.y[l[j]])){
				cout << "line from " << m << " to " << n << " crosses arc from " << j << " positively \n";
				new_sheet=B.b[i].p[sheet];
			} else if(crosses_right(C.x[l[m]],C.y[l[m]],C.x[l[n]],C.y[l[n]],C.x[l[j]],C.y[l[j]])){
				cout << "line from " << m << " to " << n << " crosses arc from " << j << " negatively \n";
				new_sheet=inverse(B.b[i]).p[sheet];
			};
		};
		return(new_sheet);
	};
};

packing branch_cover(packing P, branch_data B, layout_data L, center_list C){	// take a branched cover over P with data B
	packing Q;
	adjacency_list A;
	int i,j,k,l,m,n,deg,sheet,local_sheet;
	vector<int> I;
	for(sheet=0;sheet<degree(B.b[0]);sheet++){
		for(j=0;j<P.v.size();j++){
			I=itinerary(P,B,j,sheet);
			if(minimal_sheet(P,B,j,sheet)==sheet){
				local_sheet=sheet;
				A.a.clear();
				if(I.size()>1){			// case of branch point
					cout << "branch vertex " << j << "," << sheet << "\n";
					local_sheet=adjust_sheet(P,B,L,C,j,P.v[j].a[0],sheet);
					for(k=0;k<I.size();k++){
						for(l=0;l<P.v[j].a.size();l++){
							m=P.v[j].a[l];
							n=P.v[j].a[(l+1)%P.v[j].a.size()];
							A.a.push_back(new_index(P,B,P.v[j].a[l],local_sheet));
							cout << P.v[j].a[l] << "," << local_sheet << " ";
							local_sheet=adjust_sheet(P,B,L,C,m,n,local_sheet);
						};
					};
					cout << "\n";
				} else {
					cout << "ordinary vertex " << j << "," << sheet << "\n";
					for(l=0;l<P.v[j].a.size();l++){
						m=P.v[j].a[l];
						cout << P.v[j].a[l] << "," << sheet << " ";
						local_sheet=adjust_sheet(P,B,L,C,j,m,sheet);
						A.a.push_back(new_index(P,B,P.v[j].a[l],local_sheet));
					};
					cout << "\n";
				};
				Q.v.push_back(A);
				Q.r.push_back(0.1);
			};
		};
	};
	Q.r[0]=-1.0;
	
	return(Q);
};

