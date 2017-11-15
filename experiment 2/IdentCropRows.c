/*******************************************************************/
/* simple_t      Compute an absolut threshold.                     */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file crop thresholded image "},
	{    "ig=",    0,   " row image "},
	{    "of=",     0,   "output for visualization"},
	{    "of2=",     0,   "output for gap recognition"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  IVAL2   par[1].val
#define  OVAL   par[2].val
#define  OVAL2   par[3].val



Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (rm);  
Vfstruct (im2);

main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  		  

		VXparse(&argc, &argv, par);       /* parse the command line       */


		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
		Vfread(&im2, IVAL2);                /* read image file              */
		Vfembed(&rm, &im2, 1,1,1,1);       /* image structure with border  */
		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
im2.u[y][x]=0;
			}
		}
	

		for (y =im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
			if((tm.u[y][x] !=0 )&&(rm.u[y][x]!=0)){
				im.u[y][x]=rm.u[y][x];
im2.u[y][x]=rm.u[y][x];
			}
if((tm.u[y][x]==0)&&(rm.u[y][x]!=0)){im2.u[y][x]=(rm.u[y][x]-30);//to visualize
}	
			}
		}


	Vfwrite(&im, OVAL);             /* write image file */
	Vfwrite(&im2, OVAL2); 	
exit(0);
	}

