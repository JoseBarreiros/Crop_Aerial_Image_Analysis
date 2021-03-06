/*******************************************************************/
/* bound      Compute local max operation on a single byte image   */
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

main(argc, argv)
int argc;
char *argv[];
{
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
int        y,x;                     /* index counters               */
  VXparse(&argc, &argv, par);       /* parse the command line       */

  Vfread(&im, IVAL);                /* read image file              */
  Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
  if ( im.type != VX_PBYTE ) {      /* check image format           */
     fprintf(stderr, "vtemp: no byte image data in input file\n");
     exit(-1);
  }
  
  for (y = im.ylo ; y <= im.yhi ; y++) {  /* compute the function */
     for (x = im.xlo; x <= im.xhi; x++)  {   
	int current = tm.u[y][x];
	int left = tm.u[y][x-1];
	int above = tm.u[y+1][x];
	int right = tm.u[y][x+1];
	int below = tm.u[y-1][x];
	
	if (left!=0 || right!=0 || above!=0 || below!=0) {
		if (current != 0){
			im.u[y][x]=128;
		}
		else{
			im.u[y][x]=255;
		}
	}
	else{
		im.u[y][x]=0;
	}
     }
   }

   Vfwrite(&im, OVAL);             /* write image file                */
   exit(0);
}
