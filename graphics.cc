/*******************************************************
 * graphics.cc - sets up window stuff for main program *
 *                                                     *
 * uses standard Xlib stuff, because I'm too perverse  *
 * to use a GUI toolkit. the main reason is that       *
 * they keep upgrading to newer and newer libraries    *
 * which are not backward compatible, so that my       *
 * programs always start to break. boohoohoo.          *
 *                                                     *
 * Danny Calegari 12/17/2000                           *
 *******************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

Display *display;
int screen_num;
//Screen *screen_ptr;
// char *display_name; // = NULL;
unsigned int display_width, display_height;
XEvent report;
GC gc;
Window win;
int border_width = 4;
unsigned int width, height;

void setup_graphics(void){

  display=XOpenDisplay(NULL);
  display_width = DisplayWidth(display, screen_num);
  display_height = DisplayHeight(display, screen_num);
  
  screen_num = DefaultScreen(display);
//  screen_ptr = DefaultScreenOfDisplay(display);
  
  width = 600;
  height = 600;
  
  win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
			    0, 0, width, height, border_width, 
			    BlackPixel(display, screen_num),
			    WhitePixel(display, screen_num));
 
  XSelectInput(display, win, ExposureMask | KeyPressMask);
  
  		//	| ButtonPressMask | StructureNotifyMask ); 


//  gc = XCreateGC(display, RootWindow(display, screen_num), 0, NULL);
  gc = DefaultGC(display, screen_num);
  XSetForeground(display, gc, BlackPixel(display, screen_num));
  XSetBackground(display, gc, WhitePixel(display, screen_num));
  
  XMapWindow(display, win);
}


void erase_field(void){
	XClearWindow(display, win);
}

void draw_line(int x1, int y1, int x2, int y2){
        XDrawLine(display, win, gc, x1, 600-y1, x2, 600-y2);
};

void draw_circle(int x, int y, int r, double col){
        XSetForeground(display, gc, (long) ((256.0*65536.0)*col)+((256.0*256.0)*0.5)+(256.0*(1.0-col)));
        XSetLineAttributes(display, gc, 2, LineSolid, 1, 1);
        XDrawArc(display, win, gc, x-r, 600-y-r, 2*r, 2*r, 0, 23040);
};

void draw_3d_circle(Point P, double R, bool front){
	double t;
	double theta,r;
	double fraction;
	fraction=0.003/R;	// the bigger the radius, the smaller fraction should be
	Matrix M;
	Point Q;
	int x1,y1,x2,y2;
    theta=sph_ang(P);
    r=acos(P.z);
    M=ROT(theta)*XZ_ROT(-r);
    Q.x=0.0;
    Q.y=0.0;
    Q.z=1.0;
  //  write_Point(M(Q));
	// write_Point(P);
	if(front){
	    for(t=0.0;t<=TWOPI*(1.0+fraction);t=t+TWOPI*fraction){
    		Q.x=sin(R)*cos(t);
    		Q.y=sin(R)*sin(t);
    		Q.z=cos(R);
    		Q=M(Q);
    		x2=x1;
    		y2=y1;
    		x1=(int) (300.0+Q.x*200.0);
    		y1=(int) (300.0+Q.y*200.0);
    		if(t!=0.0 && Q.z>=0.0){
    			draw_line(x1,y1,x2,y2);
    		};
    	};
	} else {
	    for(t=0.0;t<=TWOPI*(1.0+fraction);t=t+TWOPI*fraction){
    		Q.x=sin(R)*cos(t);
    		Q.y=sin(R)*sin(t);
    		Q.z=cos(R);
    		Q=M(Q);
    		x2=x1;
    		y2=y1;
    		x1=(int) (300.0+Q.x*200.0);
    		y1=(int) (300.0+Q.y*200.0);
    		if(t!=0.0 && Q.z<=0.0){
    			draw_line(x1,y1,x2,y2);
    		};
    	};
	};
	

};

void Packing::draw_circles(){
	int X,Y,R;
	int SIZE;
	SIZE=(int) adj.size();
	erase_field();
	if(geometry=='E'){	
		int i;
		for(i=0;i<SIZE;i++){
			X=(int) (300.0+VIEWMATRIX(center[i]).x*200.0);
			Y=(int) (300.0+VIEWMATRIX(center[i]).y*200.0);
			R=(int) (VIEWMATRIX.e[0][0]*rad[i]*200.0);	// will this work?
			R=abs(R);	//	infinite circle drawn in ordinary way
			draw_circle(X,Y,R,(double) i/(double) SIZE);
		};
	} else if(geometry=='S'){
		int i;
		draw_circle(300,300,200,0.5);	// draw sphere outline
		XSetForeground(display, gc,(long) 0xF0F0F5);	// wonder what color this is?
		XSetLineAttributes(display, gc, 1, LineDoubleDash, 1, 1);	
		for(i=0;i<SIZE;i++){	// first pass; draw part of circles on the "dark side"
			draw_3d_circle(VIEWMATRIX(center[i]),rad[i],false);
		};
		XSetForeground(display, gc,(long) 0x001000);
		XSetLineAttributes(display, gc, 1, LineSolid, 1, 1);
		for(i=0;i<SIZE;i++){	// second pass; draw part of circles on the "front side"	
			draw_3d_circle(VIEWMATRIX(center[i]),rad[i],true);
		};
	};
};

void Packing::XY_rotate(double theta){
	VIEWMATRIX=ROT(theta)*VIEWMATRIX;
};
void Packing::XZ_rotate(double theta){
	VIEWMATRIX=XZ_ROT(theta)*VIEWMATRIX;
};
void Packing::YZ_rotate(double theta){
	VIEWMATRIX=YZ_ROT(theta)*VIEWMATRIX;
};
void Packing::dilate(double t){
	VIEWMATRIX=DIL(t)*VIEWMATRIX;
};
void Packing::reset_VIEWMATRIX(){
	VIEWMATRIX=ROT(0.0);
};

void graphics_routine(Packing &P, bool &finished){
	P.draw_circles();
	XFlush(display);
			
	XNextEvent(display, &report);
    	switch (report.type) {
      		case KeyPress:
  				if(XLookupKeysym(&report.xkey, 0) == XK_q){
  					finished=true;
                    XCloseDisplay(display);
                	exit(0);
                   	break;
               	};
               	if(P.write_geometry()=='S'){
           	    	if(XLookupKeysym(&report.xkey, 0) == XK_Right){
            	   		P.XZ_rotate(-TWOPI/50.0);
            	   	};
            	   	if(XLookupKeysym(&report.xkey, 0) == XK_Left){
               			P.XZ_rotate(TWOPI/50.0);
               		};
               		if(XLookupKeysym(&report.xkey, 0) == XK_Up){
               			P.YZ_rotate(-TWOPI/50.0);
               		};
               		if(XLookupKeysym(&report.xkey, 0) == XK_Down){
               			P.YZ_rotate(TWOPI/50.0);
               		};
               	} else if(P.write_geometry()=='E'){
               		if(XLookupKeysym(&report.xkey, 0) == XK_Up){
               			P.dilate(1.02);
               		};
               		if(XLookupKeysym(&report.xkey, 0) == XK_Down){
               			P.dilate(1.0/1.02);
               		};               		          
               		if(XLookupKeysym(&report.xkey, 0) == XK_g){
	               		P.reset_VIEWMATRIX();
               			P.change_geometry('S');
               		};
               	};
          //     	break;
            default:
               	break;
        };
};