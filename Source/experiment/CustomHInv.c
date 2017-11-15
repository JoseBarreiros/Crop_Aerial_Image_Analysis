/*******************************************************************/
/* simple_t      Compute an absolut threshold.                     */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */
#include "stdio.h"
#include "math.h"
VXparam_t par[] =             
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
int t_value;
void plot_line(float,float,int);

main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x;                     	  		  

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
	
double teta,ro;
float b_r,slope_r;
		for (y =im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
			if(tm.u[y][x] ==255 ){
teta=((y-127.5)*3.1416/255);
ro=x*360.62/255;
b_r=ro/sin(teta);  //x=>ro_vx y=>teta_vx
slope_r=-tan(teta);

printf("b:%0.2f	slope:%0.2f\n",b_r,slope_r);
				plot_line(b_r,slope_r,255);
			}
		}
}


	Vfwrite(&im, OVAL);             /* write image file */
	exit(0);
	}


/* Function that returns -1,0,1 depending on whether x */
/* is <0, =0, >0 respectively */
#define sign(x) ((x>0)?1:((x<0)?-1:0))
void plot_line(float b0,float slope0,int L0){
int x1,y1,x2,y2;
float t_y2;

//find min ext point
x1=0;
y1=b0;
//find max ext point
x2=255;
t_y2=(slope0*255)+b0;
y2=(int)t_y2;

//

int dx, dy, x, y, d, s1, s2, swap=0, temp;

  dx = abs(x2 - x1);
  dy = abs(y2 - y1);
  s1 = sign(x2-x1);
  s2 = sign(y2-y1);

  /* Check if dx or dy has a greater range */
  /* if dy has a greater range than dx swap dx and dy */
  if(dy > dx){temp = dx; dx = dy; dy = temp; swap = 1;}

  /* Set the initial decision parameter and the initial point */
  d = 2 * dy - dx;
  x = x1;
  y = y1;

  int i;
  for(i = 1; i <= dx; i++)
  {
  if (((x>=0)&&(x<=255))&&((y>=0)&&(y<=255))){
			im.u[y][x]=L0;}//  setPixel(x,y);
    
 while(d >= 0) 
    {
	d = d - 2 * dx;
      	if(swap) x = x + s1;
      	else y = y + s2;
    }
//printf("d:%d\n",d);
    d = d + 2 * dy;  
    if(swap) y = y + s2;
    else x = x + s1;
}


}

