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
void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */


/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  /* index counters               */
		int L=10;			  /* Label weight		  */
		VXparse(&argc, &argv, par);       /* parse the command line       */

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

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] != 0 && im.u[y][x]==0){ //object label found and current label not set
					set_label(x, y, L);
					L=L+10;
				}
			}
		}
		


	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}

void set_label(int x, int y, int L)
{
	printf("IN Method Set_Label\n");
	printf("IMx: %d\t IMy: %d\t IMval: %d\n", x,y,im.u[y][x]);
	printf("TMx: %d\t TMy: %d\t TMval: %d\n", x,y,tm.u[y][x]);

	im.u[y][x] = L;

	if(tm.u[y+1][x]!=0 && im.u[y+1][x]==0){
		set_label(x, y+1, L);
	}
	if(tm.u[y-1][x]!=0 && im.u[y-1][x]==0){
		set_label(x, y-1, L);
	}
	if(tm.u[y][x+1]!=0 && im.u[y][x+1]==0){
		set_label(x+1, y, L);
	}
	if(tm.u[y][x-1]!=0 && im.u[y][x-1]==0){
		set_label(x-1, y, L);
	}
	
}
