/*******************************************************************/
/* HighRangeT      Threshold the image in T equal to max pixel     */
/* value less a range set in commandline                           */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "r=",     0,   "Range"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  RVAL	par[2].val

Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */


main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  /* index counters  */
	
int T=0; //threshold
int range;
	
		int lowest, highest;		/*variables that hold low and high pixel values of image*/	
		float accumulator;
		int test;
		VXparse(&argc, &argv, par);       /* parse the command line       */
				if (RVAL){ //grab region_range from command line
			range = atoi(RVAL);
		}
		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */

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

		//printf("low: %d\t high: %d\n", lowest, highest);

T=highest-range;

		for (y =im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
			if(tm.u[y][x] > T){
						im.u[y][x]=tm.u[y][x];
					}

					else{
						im.u[y][x]=0;
					}	
			}
		}	

	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}
