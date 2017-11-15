	/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val

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
//struct Crops MetaDataArray[100]; 

/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x, xx, yy;                     	  /* index counters               */
				  /* variable for pflag block	  */
		float accumulator;
accumulator = 0;
		VXparse(&argc, &argv, par);       /* parse the command line       */
		
		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */

		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				
				//convolve kernel with image
				for(yy=-1; yy<=1; yy++){
					for(xx=-1; xx<=1; xx++){
						if(yy==-1){//bottom row
							if(xx==-1){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}
							if(xx==0){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}
							if(xx==1){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}
						}
						if(yy==0){//middle row
							if(xx==-1){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}
							if(xx==0){
								accumulator = accumulator + (8/9)*tm.u[y+yy][x+xx];
							}
							if(xx==1){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}	
						}
						if(yy==1){//top row
							if(xx==-1){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}
							if(xx==0){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
							}
							if(xx==1){
								accumulator = accumulator + (-1/9)*tm.u[y+yy][x+xx];
// printf("%d,     ",tm.u[y+yy][x+xx]);
							}						}
					}
				}


				im.u[y][x]= accumulator;
           printf("%.2f,     ",accumulator);
accumulator = 0;
			}
		}	

	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}
