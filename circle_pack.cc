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
	

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>

using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

#include "combinatorics.cc"		// basic combinatorial data structure of a packing
#include "trigonometry.cc"		// basic functions on packings
#include "layout.cc"
#include "graphics.cc"
#include "output.cc"
#include "input_routine.cc"


int main(int argc, char *argv[]){

	packing P;
	layout_data L;
	center_list C;
	bool finished=false;
	
//	ifstream packing_file;
	
	while(finished==false){
		input_routine(P,L,C,finished);
	};

	setup_graphics();
	XFlush(display);
	usleep(100000);
	XSetInputFocus(display, win, RevertToNone, CurrentTime);

	while(1){
		rescale(P);
		erase_field();
		draw_circles(P,L,C);
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
	
	
	return 0;
}
