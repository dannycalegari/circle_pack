/* file trigonometry.cc	*/


double subtend(double r1, double r2, double r3){		
	double a,b,c,d,phi;
	a=r1+r2;
	b=r1+r3;
	c=r2+r3;
	d=((a*a)+(b*b)-(c*c))/(2.0*a*b);
	if(d>1.0){				// acos is very strict about domain
		phi=0.0;			// so if numerical error puts d outside [-1,1]
	} else if(d<-1.0){		// we have to push it back in
		phi=3.1415926;
	} else {
		phi=acos(((a*a)+(b*b)-(c*c))/(2.0*a*b));	// Euclidean law
	};
//	phi=acos((cos(a)*cos(b)-cos(c))/(sin(a)*sin(b)));	// Spherical law
	return(phi);
};

double angle(packing P, int i){
	/* this function adds up the angles subtended at vertex i by neighborhing
	circles, given current values of radii. vertex 0 is special. */
	
	double theta, r1, r2, r3;
	int j,k,l,m;
	
	theta=0.0;	// initial value
	j=P.v[i].a.size();
	r1=P.r[i];	// my radius
	for(k=0;k<j;k++){	// for each adjacent vertex,
		l=P.v[i].a[k];	// find name of vertex
		r2=P.r[l];		// find radius of circle at vertex l
		if(k==j-1){		// m is next adjacent vertex in the circular order
			m=P.v[i].a[0];
		} else {
			m=P.v[i].a[k+1];
		};
		r3=P.r[m];	// find radius of circle at vertex m

	/*	if(i==1){
			cout << "vertices " << l << " and " << m << "\n";
			cout << "radii " << r2 << " and " << r3 << "\n";
			cout << "subtends " << subtend(r1,r2,r3) << "\n";
		}; */

		theta=theta+subtend(r1,r2,r3);	// add angle subtended by circle l to total
	};
	return(theta);
};

double fitness(packing P){
	double d;
	d=0.0;
	int i,j;
	i=P.v.size();
	for(j=0;j<i;j++){
		d=d+fabs(angle(P,j)-6.28318530717959);
	};
	return(d);
};

double correct_ratio(double A, int V){
	/* if a vertex of valence V has radius R and angle A
		then if all neighbors had the same radius, we would need a
		new radius R' to give angle 2pi. This function returns
		the ratio R'/R.	*/
	double current, desired;
	current=sqrt(1.0/(2.0*(1.0-cos(A/(double) V))))-0.5;
	desired=sqrt(1.0/(2.0*(1.0-cos(6.28318530717959/(double) V))))-0.5;
	return(desired/current);
};


void adjust_angles(packing &P, double step_size){
	vector<double> r;
	double adj;
	r.clear();
	int i;
	for(i=0;i<(int) P.v.size();i++){
		adj=correct_ratio(angle(P,i),P.v[i].a.size());
		adj=1.0+((adj-1.0)*step_size);
		r.push_back(P.r[i]*adj);
	};
	P.r=r;
};

void rescale(packing &P){
	int i,j;
	double d;
	i=P.v.size();
	d=-P.r[0];
	for(j=0;j<i;j++){
		P.r[j]=2.0*P.r[j]/d;
	};
};


void packing_angles(packing P){
	int i,j;
	double d;
	
	cout << "testing angles with current radii \n";
	
	i=P.v.size();
	for(j=0;j<i;j++){
		d=angle(P,j);
		cout << "angle at vertex " << j << " is " << d << "\n";
	};
	
	return;
};
