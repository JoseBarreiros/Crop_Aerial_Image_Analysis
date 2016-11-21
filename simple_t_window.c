	/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "tl=",     0,   "Threshold"},
	{    "th=",     0,   "Threshold"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  RVAL	par[2].val
#define  HVAL	par[3].val



Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
int tl_value;
int th_value;



/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  /* index counters               */
		int L=1;			  /* Label weight		  */
		int first;			  /* variable for pflag block	  */

		VXparse(&argc, &argv, par);       /* parse the command line       */
		if (RVAL){ //grab region_range from command line
			tl_value = atoi(RVAL);
		}
		if (HVAL){ //grab region_range from command line
			th_value = atoi(HVAL);
		}
		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
			}
		}
	

		for (y =im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {


			if((tm.u[y][x] < tl_value)||(tm.u[y][x] > th_value)) {
				im.u[y][x]=0;		

					}
else {
			im.u[y][x]=tm.u[y][x];

}	
			}
		}
		
		


	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}

