/*********************************************************************/
/* vssum   Compute local 1x1xn mean using the buffer method          */
/*********************************************************************/

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */

VXparam_t par[] =            /* command line structure               */
{
{    "if=",    0,   " input file vssum: compute temporal mean"},
{    "of=",    0,   " output file "},
{    "n=",     0,   " number of frames "},
{     0,       0,    0}
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  NVAL   par[2].val

int
main(argc, argv)
int argc;
char *argv[];
{
V3fstruct (im);
V3fstruct (tm);
int        x,y,z;           /* index counters                 */
int        n;               /* Number of frames to average    */
int        median;
int	   pix1,pix2,pix3;

    VXparse(&argc, &argv, par); /* parse the command line    */

    //n = (NVAL ? atoi(NVAL) : 1); /* read n, default is n=1   */

    while (Vbfread( &im, IVAL, 3)) {
	if ( im.type != VX_PBYTE || im.chan != 1) { /* check format  */
           fprintf (stderr, "image not byte type\n");
           exit (1);
        }
        for (y = im.ylo; y <= im.yhi; y++) {
           for (x = im.xlo; x <= im.xhi; x++) {
              //sum = 0;
              for (z = im.zlo; z <= im.zhi; z++) {
		   pix1=im.u[z][y][x];
		   //printf("z:%d\n", z);
		   pix2=im.u[++z][y][x];
		   //printf("z:%d\n", z);
		   pix3=im.u[++z][y][x];
		   //printf("z:%d\n", z);
		   if(pix1-pix2<0){//pix1 is less than pix2
			   if(pix1-pix3<0){//pixl1 is less than pix3 
				   if(pix3-pix2<0){//pix3 is less than pix2
					   median = pix2;
				   }else{
					   median = pix3;
				   }
			   }		//pix1 is greater than pix3
			   else{
				   median = pix1;
			   }
		   }		
		   else if(pix2-pix3<0){//pix1 is greater than px2
			median = pix3;
		   }else{
			median = pix2;
		   }
              }
              im.u[0][y][x] = median;
            }
        }
        V3fwrite (&im, OVAL); /* write the oldest frame */
    }
    exit(0);
}

int get_median(int x, int y, int z)
{
	if(x>y){//x is greater than y
		if(x>z){//x>y,x>z
			if(y>z) return y; //x>y>z
			else return z; //x>z>y
		}else{//x>y, x<z
			return x; //z>x>y
		}

	}else{//x<y
		if(x>z){
			return x;//y>x>z
		}
		else{//y>x z>x
			if(y>z) return z; //y>z>x
			else return y;//z>y>x
		}

	}
	
}
