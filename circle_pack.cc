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

#include "combinatorics.cc"		// basic combinatorial data structure of a packing
#include "trigonometry.cc"		// basic functions on packings

#include "Trigonometry_routine.cc"	// alternate version of trigonometry
#include "Packing.cc"			// alternate version of packing
#include "Layout_routine.cc"	// alternate version of layout

#include "potato.cc"			// experimental: for "potato packings"

#include "layout.cc"
#include "branched_cover.cc"
#include "graphics.cc"
#include "output.cc"
#include "input_routine.cc"


int main(int argc, char *argv[]){

	
	packing p;
	layout_data L;
	center_list C;
	bool finished=false;
	Packing P;
	ifstream packing_input_file;
	
	if(argc>1){
		packing_input_file.open(argv[1]);
		P.read_packing(packing_input_file);
		packing_input_file.close();
		P.set_verbose(false);
		P.find_radii(0.000000001);
//		P.write_radii();
		P.determine_layout();
		P.determine_centers();
//		P.write_centers();
		P.change_geometry('E');
//		P.write_radii();
//		P.write_centers();
		
		setup_graphics();
		XFlush(display);
		usleep(100000);
		XSetInputFocus(display, win, RevertToNone, CurrentTime);
		while(1){
			erase_field();
			P.draw_circles();
			XFlush(display);
			
			XNextEvent(display, &report);
    			switch (report.type) {
           			case KeyPress:
  						if(XLookupKeysym(&report.xkey, 0) == XK_q){
                        	XCloseDisplay(display);
                        	exit(0);
                    		break;
                		};
            			default:
                			break;
        	};
		};
	} else {
	
	while(finished==false){
		input_routine(p,L,C,finished);
	};
	
	setup_graphics();
	XFlush(display);
	usleep(100000);
	XSetInputFocus(display, win, RevertToNone, CurrentTime);

	
	while(1){
		erase_field();
		draw_circles(p,C);
		XFlush(display);

		XNextEvent(display, &report);
    		switch (report.type) {
           		case KeyPress:
  					if(XLookupKeysym(&report.xkey, 0) == XK_q){
                        XCloseDisplay(display);
                        exit(0);
                    	break;
                	};
            	default:
                	break;
        };
	};

	};
	
	return 0;
}
