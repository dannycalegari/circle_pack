/* Trigonometry_routine.cc */


struct Point {
	double x,y,z;	
};

void write_Point(Point P){
	cout << P.x << " " << P.y << " " << P.z << "\n";
};

Point midpoint(Point P, Point Q){
	Point R;
	R.x=(P.x+Q.x)/2.0;
	R.y=(P.y+Q.y)/2.0;
	R.z=(P.z+Q.z)/2.0;
	return(R);
};

class Matrix {
	public:
		// do I want entries to be public?
		vector< vector<double> > e;	// implicitly 3x3; e[i] = row i, e[i][j] = row i, column j
		Point operator()(Point P){
			Point Q;
			Q.x = P.x*e[0][0]+P.y*e[0][1]+P.z*e[0][2];
			Q.y = P.x*e[1][0]+P.y*e[1][1]+P.z*e[1][2];
			Q.z = P.x*e[2][0]+P.y*e[2][1]+P.z*e[2][2];
			return(Q);
		};	
		Matrix operator*(Matrix M){
			Matrix N;
			int i,j;
			vector<double> row;
			for(i=0;i<3;i++){
				row.clear();
				for(j=0;j<3;j++){
					row.push_back(e[i][0]*M.e[0][j]+e[i][1]*M.e[1][j]+e[i][2]*M.e[2][j]);
				};
				N.e.push_back(row);
			};
			return(N);
		};
		Matrix transpose(){
			Matrix N;
			int i,j;
			vector<double> row;
			for(i=0;i<3;i++){
				row.clear();
				for(j=0;j<3;j++){	// ith column of matrix
					row.push_back(e[j][i]);
				};
				N.e.push_back(row);	// is made into ith row of result
			};
			return(N);
		};			
		Matrix hyp_inv(){	// inverse of M is JM^TJ
			Matrix N;
			int i;
			N=transpose();
			for(i=0;i<3;i++){
				N.e[i][2]=-N.e[i][2];
			};
			for(i=0;i<3;i++){
				N.e[2][i]=-N.e[2][i];
			};
			return(N);
		};
};

void write_Matrix(Matrix M){
	int i,j;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			cout << M.e[i][j] << " ";
		};
		cout << "\n";
	};
};


Matrix ROT(double angle){	// rotation in xy-plane about origin
	// returns matrix C=cos(angle), S=sin(angle)
	// (C -S  0)
	// (S  C  0)
	// (0  0  1)
	Matrix M;
	M.e.clear();
	vector<double> row;
	row.clear();
	row.push_back(cos(angle));
	row.push_back(-sin(angle));
	row.push_back(0.0);
	M.e.push_back(row);
	row.clear();
	row.push_back(sin(angle));
	row.push_back(cos(angle));
	row.push_back(0.0);
	M.e.push_back(row);
	row.clear();
	row.push_back(0.0);
	row.push_back(0.0);
	row.push_back(1.0);
	M.e.push_back(row);
	return(M);
};

Matrix HTR(double dist){	// translation in hyperbolic plane along x axis
	// returns matrix c=cosh(dist), s=sinh(dist)
	// (c  0  s)
	// (0  1  0)
	// (s  0  c)
	Matrix M;
	vector<double> row;
	row.clear();
	row.push_back(cosh(dist));
	row.push_back(0.0);
	row.push_back(sinh(dist));
	M.e.push_back(row);
	row.clear();
	row.push_back(0.0);
	row.push_back(1.0);
	row.push_back(0.0);
	M.e.push_back(row);
	row.clear();
	row.push_back(sinh(dist));
	row.push_back(0.0);
	row.push_back(cosh(dist));
	M.e.push_back(row);
	return(M);
};

double hyp_dist(Point P){	// hyperbolic distance from origin
	if(P.z<1.0){
		return(0.0);
	};
	return(acosh(P.z));
};

double ang(Point P){	// hyperbolic angle from origin
	double d;
	d=hyp_dist(P);
	if(d==0.0){
		return(0.0);
	};
	return(atan2(P.y,P.x));
};

Point hyperboloid_to_Poincare(Point P){	// change coordinates from hyperboloid to Poincare model
	// this can be used to change geometry from hyperbolic to Euclidean
	Point Q;
	Q.x=P.x/(P.z+1.0);
	Q.y=P.y/(P.z+1.0);
	Q.z=1.0;
	return(Q);
};
