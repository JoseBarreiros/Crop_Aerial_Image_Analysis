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


Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
//struct Crops MetaDataArray[100]; 

/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x, xx, yy;                     	  /* index counters               */
		int L=1;			  /* Label weight		  */
		int first=0;			  /* variable for pflag block	  */
		int lowest, highest;		/*variables that hold low and high pixel values of image*/	
		float accumulator;
		int test;
		VXparse(&argc, &argv, par);       /* parse the command line       */
		
		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
		//Vfembed(&im2, &im, 0,0,0,0);       /* image structure with border  */

		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}
		
		printf("calculating low and high\n");
		for (y = im.ylo; y<=im.yhi; y++){
			for (x=im.xlo; x<=im.xhi; x++) {
				if(first==0){
					lowest = im.u[y][x];
					highest = im.u[y][x];
					first = 1;
				}
				if(lowest>=im.u[y][x]) lowest = im.u[y][x];
				if(highest<=im.u[y][x]) highest = im.u[y][x];
			}
		}

		printf("low: %d\t high: %d\n", lowest, highest);
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				accumulator = (float) (tm.u[y][x]-lowest)/(highest-lowest);//*(255);// expand histogram values to 0-255
				test = accumulator * 255;
				im.u[y][x] = test;
				//printf("accum: %.2f\n", accumulator);
				//printf("outcome: %d\n", test);
			}
		}	

	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}
