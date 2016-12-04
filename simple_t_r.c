	/*******************************************************************/
/* simple_t_r Threshold the image. Pixels over T are pass to the output  */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "t=",     0,   "Threshold"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  RVAL	par[2].val


void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
int t_value;


main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  /* index counters               */
		int L=1;			  /* Label weight		  */
		int first;			  /* variable for pflag block	  */

		VXparse(&argc, &argv, par);       /* parse the command line       */
		if (RVAL){ //grab region_range from command line
			t_value = atoi(RVAL);
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
			if(tm.u[y][x] > t_value){
						im.u[y][x]=tm.u[y][x];//multiply by negative 1
					}

					else{
						im.u[y][x]=0;
					}	
			}
		}
		
		


	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}

