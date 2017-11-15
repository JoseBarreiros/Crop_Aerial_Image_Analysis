/*******************************************************************/
/* simple_t      Compute an absolut threshold.                     */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},

{    "if2=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  IVAL2   par[1].val
#define  OVAL   par[2].val




Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm); 
Vfstruct (im2); 
Vfstruct (tm2);                      /* temp image structure         */



main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  		  

		VXparse(&argc, &argv, par);       /* parse the command line       */
	

		Vfread(&im, IVAL);                /* read image file              */
	Vfread(&im2, IVAL2); 
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
Vfembed(&tm2, &im2, 1,1,1,1); 
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
			if((tm2.u[y][x]==0)&&(tm.u[y][x]!=0)){
						im.u[y][x]=tm.u[y][x];//apply T
					}

					else{
						im.u[y][x]=0;
					}	
			}
		}


	Vfwrite(&im, OVAL);             /* write image file */
	exit(0);
	}

