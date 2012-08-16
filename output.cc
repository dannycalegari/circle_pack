/*	output.cc	postscript output routine	*/

void Packing::write_packing_eps(ofstream &outfile){
	int i;
	if(geometry=='E'){
		outfile << "%!PS-Adobe-2.0 EPSF-2.0 \n";
		outfile << "%%BoundingBox: 0 0 500 500 \n";
		outfile << "gsave 100 100 scale 2.5 2.5 translate \n";
		outfile << "/min { 2 copy gt { exch } if pop } bind def \n";
		outfile << "/c {3 dict begin /r exch def /y exch def /x exch def \n";
		outfile << "1 50 div r 10 div min setlinewidth \n";
		outfile << "newpath x y r 0 360 arc stroke end} def \n";
		for(i=0;i< size();i++){
			outfile << center[i].x << " " << center[i].y << " " << fabs(rad[i]) << " c\n";
		};
		outfile << "grestore %eof \n";
	} else if(geometry=='S'){
		outfile << "%!PS-Adobe-2.0 EPSF-2.0 \n";
		outfile << "%%BoundingBox: 0 0 500 500 \n";
		outfile << "gsave 200 200 scale 1.25 1.25 translate 1 200 div setlinewidth \n";
		outfile << "/arccos { 2 dict begin /base exch def /opp 1 base base mul sub \n";
		outfile << "sqrt def opp base atan end } def \n";
		outfile << "/c { 3 dict begin /R exch 360 mul " << TWOPI <<" div def \n";
		outfile << "/beta exch 360 mul " << TWOPI <<" div def /gamma exch 360 mul " << TWOPI <<" div \n";
		outfile << "def gsave beta rotate 0 gamma sin R cos mul translate 1 gamma \n";
		outfile << "cos scale newpath 0 0 R sin 0 360 arc stroke fill grestore end } def \n";
		for(i=0;i< size();i++){
			outfile << acos(center[i].z) << " " << atan2(center[i].x,center[i].y) 
				<< " " << rad[i] << " c\n";
		};
		outfile << "newpath 0 0 1 0 360 arc stroke grestore %eof \n";
	};
};

void Packing::write_packing(ofstream &outfile){
	int i,j;
	
	outfile << size() << "\n";
	for(i=0;i<size();i++){
		outfile << (int) adj[i].size() << "  ";
		for(j=0;j<(int) adj[i].size();j++){
			outfile << adj[i][j] << " ";
		};
		outfile << "\n";
	};
	for(i=0;i<size();i++){
		outfile << rad[i] << "\n";
	};
	outfile << geometry << "\n";
	return;
};