/* branched_cover.cc	a bit of a hack right now */

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

vector<int> itinerary(packing P, branch_data B, int v, int sheet){	// monodromy around vertex v on sheet s
	vector<int> I;
	int i,j;
	I.push_back(sheet);
	for(i=0;i<(int) B.v.size();i++){
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

bool is_to_the_left(point p1, point p2, point p3){	// tests if p3 is on the left of the oriented line p1->p2
	if(((p3.x - p1.x)*(p2.y - p1.y) - (p3.y - p1.y)*(p2.x - p1.x)) < 0.0){
		return(true);
	} else {
		return(false);
	};
}

bool is_to_the_right(point p1, point p2, point p3){	// tests if p3 is on the right of the oriented line p1->p2
	if(((p3.x - p1.x)*(p2.y - p1.y) - (p3.y - p1.y)*(p2.x - p1.x)) > 0.0){
		return(true);
	} else {
		return(false);
	};
}

bool crosses_left(point p1, point p2, point p3){	// does the line p1->p2 cross the line p3->(0.1,0.1)?
	point p4;
	p4.x=0.1;
	p4.y=0.1;
	if(is_to_the_left(p1,p2,p3) && is_to_the_left(p2,p1,p4) 
		&& is_to_the_left(p3,p4,p2) && is_to_the_left(p4,p3,p1)){
		return(true);	
	} else {
		return(false);
	};
};

bool crosses_right(point p1, point p2, point p3){	// does the line p1->p2 cross the line p3->(0.1,0.1)?
	point p4;
	p4.x=0.1;
	p4.y=0.1;
	if(is_to_the_right(p1,p2,p3) && is_to_the_right(p2,p1,p4) 
		&& is_to_the_right(p3,p4,p2) && is_to_the_right(p4,p3,p1)){
		return(true);	
	} else {
		return(false);
	};
};
	
int adjust_sheet(packing P, branch_data B, center_list C, int m, int n, int sheet){	// how does sheet change when we move from m to n?
	int i,j,new_sheet;
	new_sheet=sheet;
	if(m*n==0){	// special case; no branching at infinity
		return(sheet);
	} else {
		for(i=0;i<(int) B.v.size();i++){
			j=B.v[i];
//			if(crosses_left(C.x[m],C.y[m],C.x[n],C.y[n],C.x[j],C.y[j])){
			if(crosses_left(C.p[m],C.p[n],C.p[j])){
				cout << "line from " << m << " to " << n << " crosses arc from " << j << " positively \n";
				new_sheet=B.b[i].p[sheet];
//			} else if(crosses_right(C.x[m],C.y[m],C.x[n],C.y[n],C.x[j],C.y[j])){
			} else if(crosses_right(C.p[m],C.p[n],C.p[j])){
				cout << "line from " << m << " to " << n << " crosses arc from " << j << " negatively \n";
				new_sheet=inverse(B.b[i]).p[sheet];
			};
		};
		return(new_sheet);
	};
};

packing branch_cover(packing P, branch_data B, center_list C){	// take a branched cover over P with data B
	packing Q;
	adjacency_list A;
	int j,k,l,m,n,sheet,local_sheet;
	vector<int> I;
	for(sheet=0;sheet<degree(B.b[0]);sheet++){
		for(j=0;j<(int) P.v.size();j++){
			I=itinerary(P,B,j,sheet);
			if(minimal_sheet(P,B,j,sheet)==sheet){
				local_sheet=sheet;
				A.a.clear();
				if(I.size()>1){			// case of branch point
					cout << "branch vertex " << j << "," << sheet << "\n";
					local_sheet=adjust_sheet(P,B,C,j,P.v[j].a[0],sheet);
					for(k=0;k<(int) I.size();k++){
						for(l=0;l<(int) P.v[j].a.size();l++){
							m=P.v[j].a[l];
							n=P.v[j].a[(l+1)%P.v[j].a.size()];
							A.a.push_back(new_index(P,B,P.v[j].a[l],local_sheet));
							cout << P.v[j].a[l] << "," << local_sheet << " ";
							local_sheet=adjust_sheet(P,B,C,m,n,local_sheet);
						};
					};
					cout << "\n";
				} else {
					cout << "ordinary vertex " << j << "," << sheet << "\n";
					for(l=0;l<(int) P.v[j].a.size();l++){
						m=P.v[j].a[l];
						cout << P.v[j].a[l] << "," << sheet << " ";
						local_sheet=adjust_sheet(P,B,C,j,m,sheet);
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

