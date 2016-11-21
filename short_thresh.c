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

/*struct Crops{
	int area = 0;
	int center_of_mass_x=0;
	int center_of_mass_y=0;
	int avg_radius=0;
	int row=0; 
} crop;*/

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
		int first;			  /* variable for pflag block	  */
		unsigned int accumulator;
		unsigned int pix_count=0;
		unsigned int hard_thresh; //background pixels usually less than 2000

		VXparse(&argc, &argv, par);       /* parse the command line       */
		
		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */

		if ( im.type != VX_PSHORT ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(im.s[y][x]>2000){
					accumulator = accumulator + im.s[y][x];
					pix_count = pix_count + 1;
				} 
			}
		}
		printf("accumulator: %d\n", accumulator);
		printf("pix_count: %d\n", pix_count);
		hard_thresh = accumulator/pix_count;
		printf("thresh: %d\n", hard_thresh);
		hard_thresh=2500;
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				//if positive do extensive checks for thresholding
				if(im.s[y][x]>0){
					unsigned int region_check = abs(im.s[y][x] - hard_thresh); 
					//above threshold or within threshold region
					if(im.s[y][x] > hard_thresh){
						im.s[y][x]=-1*hard_thresh;//multiply by negative 1
					}
					else if (region_check <= 150){
						im.s[y][x]=-1 * hard_thresh;
					}
					else{
						im.s[y][x]=1000;
					}
	
				} else{
					im.s[y][x]=-1*hard_thresh; //keep negative 
				}
				
			}
		}	

	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}
