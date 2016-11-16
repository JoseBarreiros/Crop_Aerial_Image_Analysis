	/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "r=",     0,   "region pixel range"},
	{    "-p",     0,   "label w/ first pixel of region"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  RVAL	par[2].val
#define  PFLAG  par[3].val

/*struct Crops{
	int area = 0;
	int center_of_mass_x=0;
	int center_of_mass_y=0;
	int avg_radius=0;
	int row=0; 
} crop;*/

void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (tmf);                      /* temp image structure         */
int region_range;
//struct Crops MetaDataArray[100]; 

/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x, xx, yy;                     	  /* index counters               */
		int L=1;			  /* Label weight		  */
		int first;			  /* variable for pflag block	  */
		int accumulator;

		VXparse(&argc, &argv, par);       /* parse the command line       */
		region_range = (RVAL ? atoi(RVAL) : 5);//grab region_range from command line, set to 5 default

		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
		Vfembed(&tmf, &im,1,1,1,1);

		if ( im.type != VX_PSHORT ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}
/*
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
				accumulator = 0;
				for(yy=-1; yy<=1; yy++){
					for(xx=-1; xx<=1; xx++){
						if(yy==-1){//bottom row
							if(xx==-1){
								accumulator = accumulator + 0*tm.u[y+yy][x+xx];

							}
							if(xx==0){
								accumulator = accumulator + -1*tm.u[y+yy][x+xx];
							}
							if(xx==1){
								accumulator = accumulator + 0*tm.u[y+yy][x+xx];
							}
						}
						if(yy==0){//middle row
							if(xx==-1){
								accumulator = accumulator + -1*tm.u[y+yy][x+xx];
							}
							if(xx==0){
								accumulator = accumulator + 5*tm.u[y+yy][x+xx];
							}
							if(xx==1){
								accumulator = accumulator + -1*tm.u[y+yy][x+xx];
							}	
						}
						if(yy==1){//top row
							if(xx==-1){
								accumulator = accumulator + 0*tm.u[y+yy][x+xx];
							}
							if(xx==0){
								accumulator = accumulator + -1*tm.u[y+yy][x+xx];
							}
							if(xx==1){
								accumulator = accumulator + 0*tm.u[y+yy][x+xx];
							}						}
					}
				}
				tmf.u[y][x]= accumulator;
			}
		}
	
*/
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] != 0 && im.u[y][x]==0){ //object label found and current output label not set
					//check if -p was input as part of arguments
					if(PFLAG){
						first=tm.u[y][x];
						set_label(x, y, first);
					} else { 
						set_label(x, y, L);
						if(L==255){//roll over to 1
							L=1;
						} else {//increment label after finishing a connected component
							L++;
						}
					}
				}
			}
		}
		
		


	Vfwrite(&tmf, OVAL);             /* write image file                */
	exit(0);
	}


void set_label(int x, int y, int L)
{
	//set output pixel to label value
	im.u[y][x] = L;
	
	/*check if the neighboring pixels are object pixels and if
	output label for corresponding pixels have been set yet*/
	if(tm.u[y+1][x]!=0 && im.u[y+1][x]==0){//Up neighbor

		if(PFLAG){//pflag set
			if( abs(tm.u[y+1][x] - L) < region_range){
				set_label(x, y+1, L);
			}

		}else{
			if( abs(tm.u[y+1][x] - tm.u[y][x]) < region_range){
				set_label(x, y+1, L);
			}
		}
	}

	if(tm.u[y+1][x+1]!=0 && im.u[y+1][x+1]==0){//up right neighbor
		if(PFLAG){//pflag set
			if( abs(tm.u[y+1][x+1] - L) < region_range){
				set_label(x+1, y+1, L);
			}

		}else{
			if( abs(tm.u[y+1][x+1] - tm.u[y][x]) < region_range){
				set_label(x+1, y+1, L);
			}
		}
	}

	if(tm.u[y][x+1]!=0 && im.u[y][x+1]==0){//right neighbor
		if(PFLAG){//pflag set
			if( abs(tm.u[y][x+1] - L) < region_range){
				set_label(x+1, y, L);
			}

		}else{
			if( abs(tm.u[y][x+1] - tm.u[y][x]) < region_range){
				set_label(x+1, y, L);
			}
		}
	}

	if(tm.u[y-1][x+1]!=0 && im.u[y-1][x+1]==0){//down right neighbor
		if(PFLAG){//pflag set
			if( abs(tm.u[y-1][x+1] - L) < region_range){
				set_label(x+1, y-1, L);
			}
		}else{	
			if( abs(tm.u[y-1][x+1] - tm.u[y][x]) < region_range){
				set_label(x+1, y-1, L);
			}
		}
	}

	if(tm.u[y-1][x]!=0 && im.u[y][x+1]==0){//down neighbor 
		if(PFLAG){//pflag set
			if( abs(tm.u[y-1][x] - L) < region_range){
				set_label(x, y-1, L);
			}
		}else{
			if( abs(tm.u[y-1][x] - tm.u[y][x]) < region_range){
				set_label(x, y-1, L);
			}
		}
	}

	if(tm.u[y-1][x-1]!=0 && im.u[y][x-1]==0){//down left neighbor
		if(PFLAG){//pflag set
			if( abs(tm.u[y-1][x-1] - L) < region_range){
				set_label(x-1, y-1, L);
			}

		}else{
			if( abs(tm.u[y-1][x-1] - tm.u[y][x]) < region_range){
				set_label(x-1, y-1, L);
			}
		}
	}

	if(tm.u[y][x-1]!=0 && im.u[y+1][x]==0){//left neighbor
	/*Check if the neighboring pixels are within the region_range*/
		if(PFLAG){//pflag set
			if( abs(tm.u[y][x-1] - L) < region_range){
				set_label(x-1, y, L);
			}

		}else{
			if( abs(tm.u[y][x-1] - tm.u[y][x]) < region_range){
				set_label(x-1, y, L);
			}
		}
	}

	if(tm.u[y+1][x-1]!=0 && im.u[y+1][x-1]==0){//up left neighbor
	/*Check if the neighboring pixels are within the region_range*/
		if(PFLAG){//pflag set
			if( abs(tm.u[y+1][x-1] - L) < region_range){
				set_label(x-1, y+1, L);
			}

		}else{
			if( abs(tm.u[y+1][x-1] - tm.u[y][x]) < region_range){
				set_label(x-1, y+1, L);
			}
		}
	}
	
}
