/*	Input_handler.cc	*/

void input_routine(Packing &P, bool &finished){
	char c;
	string s;
	ifstream packing_input_file;
	ofstream packing_output_file, eps_output_file;
	int i;
	Point Q;
	
	cout << "Current packing has " << P.size() << " circles. \n";
	cout << "Enter command ([h] for help):";
	cin >> c;
	switch (c){
		case 'h':
			cout << "[h] for help (i.e. return to this list) \n";
			cout << "[v] to toggle verbose \n";
			cout << "[r] to read packing from a file \n";
			cout << "[g] to change geometry \n";
			cout << "[n] to normalize Euclidean coordinates \n";
			cout << "[w] to write packing to a file \n";
			cout << "[e] to write .eps output \n";
			cout << "[f] to find closest center \n";
			cout << "[q] to quit to graphics display \n";
			break;
		case 'v':
			P.toggle_verbose();
			break;
		case 'r':
			cout << "enter name of packing file to read from:";
			cin >> s;
			packing_input_file.open(s.c_str());
			P.read_packing(packing_input_file);
			packing_input_file.close();
			cout << "read packing from file " << s << "\n";
			cout << "finding radii.\n";
			P.find_radii(0.000000001);
			cout << "radii found.\n";
			cout << "determining layout.\n";
			P.determine_layout();
			cout << "layout determined.\n";
			cout << "determining centers.\n";
			P.determine_centers();
			cout << "centers determined.\n";
			break;
		case 'g':
			cout << "change to what geometry (S/E/H):";
			cin >> c;
			P.change_geometry(c);
			cout << "geometry changed.\n";
			break;
		case 'n':
			cout << "normalizing coordinates.\n";
			P.rescale();
			break;
		case 'w':
			cout << "enter name of packing file to write to:";
			cin >> s;
			packing_output_file.open(s.c_str());
			P.write_packing(packing_output_file);
			packing_output_file.close();
			cout << "wrote packing to file " << s << "\n";
			break;
		case 'e':
			cout << "enter name of .eps file to write to:";
			cin >> s;
			eps_output_file.open(s.c_str());
			P.write_packing_eps(eps_output_file);
			eps_output_file.close();
			cout << "wrote .eps to file " << s << "\n";
			break;
		case 'f':
			cout << "current geometry is " << P.write_geometry() << "\n";
			cout << "enter coordinates of point: ";
			cin >> Q.x >> Q.y >> Q.z;
			i=P.find_closest_vertex(Q);
			cout << "closest point is " << i << "\n";
			break;
		case 'q':
			cout << "quit to graphics display.\n" 
				<< "focus in graphics window and press [q] again to exit.\n";
			finished=true;
			break;
		default:
			break;
	};
};