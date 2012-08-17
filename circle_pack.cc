/************************************
*                                   *
*	circle_pack.cc version 0.01		*
*	July 31 2012					*
*									*
*									*
*	Copyright Danny Calegari 2012	*
*	Released under the GPL license	*
*									*
************************************/
	
#define PI 3.141592653589793238462643383279
#define TWOPI PI*2.0
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

#include "Trigonometry_routine.cc"	// alternate version of trigonometry
#include "Packing.cc"			// alternate version of packing
#include "Layout_routine.cc"	// alternate version of layout

//#include "potato.cc"			// experimental: for "potato packings"

#include "graphics.cc"
#include "output.cc"
#include "Input_handler.cc"

int main(int argc, char *argv[]){

	bool finished=false;
	Packing P;
	ifstream packing_input_file;
	
	if(argc>1){
		packing_input_file.open(argv[1]);
		P.read_packing(packing_input_file);
		packing_input_file.close();
		P.find_radii(0.000000001);
		P.determine_layout();
		P.determine_centers();
		P.change_geometry('E');
		P.rescale();
		P.change_geometry('S');
		finished=true;
	};

	while(finished==false){
		cout << "Welcome to interactive circle_pack.\n";
		cout << "For a list of commands, type [h] for help.\n";
		cout << "To run circle_pack non-interactively,\n";
		cout << "Type .\circle_pack <filename.txt> \n";
		input_routine(P,finished);
	};
	
	setup_graphics();
	XFlush(display);
	usleep(100000);
	XSetInputFocus(display, win, RevertToNone, CurrentTime);
	finished=false;
	while(finished==false){
		graphics_routine(P,finished);
	};
		
	return 0;
}
