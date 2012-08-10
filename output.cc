/*	output.cc	postscript output routine	*/

void write_circle_packing(packing P, center_list C, ofstream &outfile){
	int i;
// 	ofstream outfile;
//	outfile.open("output_circles.eps");
	outfile << "%!PS-Adobe-2.0 EPSF-2.0 \n";
	outfile << "%%BoundingBox: 0 0 500 500 \n";
	outfile << "gsave 100 100 scale 2.5 2.5 translate \n";
	outfile << "/min { 2 copy gt { exch } if pop } bind def \n";
	outfile << "/c {3 dict begin /r exch def /y exch def /x exch def \n";
	outfile << "1 50 div r 10 div min setlinewidth \n";
	outfile << "newpath x y r 0 360 arc stroke end} def \n";
	for(i=0;i<(int) C.p.size();i++){
		outfile << C.p[i].x << " " << C.p[i].y << " " << fabs(P.r[i]) << " c\n";
	};
	outfile << "grestore %eof \n";
//	outfile.close();
};
