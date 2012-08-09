/*	input_routine.cc	*/

void input_routine(packing &P, layout_data &L, center_list &C, bool &finished){
	char c;
	string s;
	ifstream packing_input_file;
	ofstream packing_output_file, eps_output_file;
	int i,j;
	double step_size;

	cout << "Current packing has " << P.v.size() << " circles. \n";
	cout << "Enter command ([h] for help):";
	cin >> c;
	switch (c){
		case 'h':
			cout << "[h] for help (i.e. return to this list) \n";
			cout << "[r] to read packing from a file \n";
			cout << "[w] to write packing to a file \n";
			cout << "[l] to determine layout order \n";
			cout << "[p] to determine packing radii \n";
			cout << "[c] to center circles \n";
			cout << "[e] to write .eps output \n";
			cout << "[s] to subdivide packing \n";
			cout << "[a] to amalgamate neighbors of specified vertex \n";
			cout << "[q] to quit to graphics display \n";
			break;
		case 'r':
			cout << "enter name of packing file to read from:";
			cin >> s;
			packing_input_file.open(s.c_str());
			read_packing(packing_input_file,P);
			packing_input_file.close();
			cout << "read packing from file " << s << "\n";
			break;
		case 'w':
			cout << "enter name of packing file to write to:";
			cin >> s;
			packing_output_file.open(s.c_str());
			write_packing(packing_output_file,P);
			packing_output_file.close();
			cout << "wrote packing to file " << s << "\n";
			break;
		case 's':
			P=subdivide(P);
			cout << "subdivided \n";
			break;
		case 'a':
			cout << "amalgamate at which vertex (0 recommended):";
			cin >> i;
			P=amalgamate(P,i);
			cout << "amalgamated at vertex " << i << "\n";
			break;
		case 'l':
			cout << "determining layout order.\n";
			layout_order(P,L);
			cout << "determined layout order.\n";
			break;
		case 'p':
			if(L.a.size()<P.v.size()){
				cout << "need to determine layout order first!\n";
			} else {
			cout << "enter step size (a real number between 0.0 and 1.0; [d]efault 0.7):";
			cin >> step_size;
			cout << "determining packing radii.\n";
		 	i=0;
            	while(fitness(P)>0.001){
           			if(i%100==0){
	           			cout << "fitness " << fitness(P) << " after " << i << " steps. \n";
	           		};
					i++;
					for(j=0;j<P.v.size();j++){
						adjust_angle(P,j,step_size);
					};
					rescale(P);
				};
			cout << "fitness " << fitness(P) << " after " << i << " steps. \n";
			cout << "determined packing radii.\n";
			};
			break;
		case 'c':
			if(L.a.size()<P.v.size()){
				cout << "need to determine layout order first!\n";
			} else {
			cout << "centering circles.\n";
			determine_centers(P,L,C);
			cout << "centers determined.\n";
			};
			break;
		case 'e':
			if(C.x.size()<P.v.size()){
				cout << "need to determine centers first!\n";
			} else {
			cout << "enter name of .eps file to write to:";
			cin >> s;
			eps_output_file.open(s.c_str());
			write_circle_packing(P,L,C,eps_output_file);
			eps_output_file.close();
			cout << "wrote .eps to file " << s << "\n";
			};
			break;
		case 'q':
			cout << "quit to graphics display; focus in graphics window and press [q] again to exit.\n";
			finished=true;
			break;
		default:
			break;
	};
};