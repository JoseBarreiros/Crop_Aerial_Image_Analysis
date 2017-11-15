/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */
#include <stdbool.h>

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "t=",    0,   " area threshold val "},
//	{    "of2=",    0,   " output file2 "},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  THRESH par[2].val

void set_label(int, int, int);
void reset_label(int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (im2);                      /* temp image structure         */

/*
struct Crops{
	unsigned short area;//pixel count
	unsigned char row; //row
	unsigned short x_coord[1000];
	unsigned short y_coord[1000];
};
*/



//global variables to hold metadata for crops
int area=0;
unsigned short x_coord[10000];
unsigned short y_coord[10000];

/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int crop_regs = 0;
		int y,x,i,j,yin,xin;              /* index counters               */
		int area_count;
		int region_area;
		int old_area=0;
		area = 0;


		int L=1;			  /* Label weight		  */
		int Area_T;
		bool redo_label = false;	  /* set true if we want to redo set label with same L */
		VXparse(&argc, &argv, par);       /* parse the command line       */
		
		if (THRESH){//get area threshold vaue
			Area_T = atoi(THRESH);
		}

		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&im2, &im, 1,1,1,1);       /* image structure with border  */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
		//Vfembed(&tmf, &im, 0,0,0,0);       /* image structure with border  */
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
				if(tm.u[y][x] != 0 && im.u[y][x]==0){ //object label found and current output label not set
					crop_regs++;	
					printf("crop_regs: %d\t", crop_regs);
					set_label(x, y, tm.u[y][x]);
					reset_label(Area_T);
				}
			}
		}
		//printf("area count: %d\n", accumulator);
		
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x =im.xlo; x <= im.xhi; x++) {
				if(im.u[y][x]==1)im.u[y][x]=0;	
			}
			
		}

	
	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
}

void set_label(int x, int y, int L)
{
	area++;//increment global variable that counts size of grown region
	x_coord[area-1]=x;//record pixels of grown region
	y_coord[area-1]=y;

	im.u[y][x] = L;



	
	/*check if the neighboring pixels are object pixels and if
	output label for corresponding pixels have been set yet*/
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

	/*Everything below is added for 8connectivity in region growing */
	if(tm.u[y+1][x-1]!=0 && im.u[y+1][x-1]==0){//up left
		set_label(x-1, y+1, L);
	}

	if(tm.u[y+1][x+1]!=0 && im.u[y+1][x+1]==0){//up right
		set_label(x+1, y+1, L);
	}

	if(tm.u[y-1][x-1]!=0 && im.u[y-1][x-1]==0){//down left
		set_label(x-1, y-1, L);
	}

	if(tm.u[y-1][x+1]!=0 && im.u[y-1][x+1]==0){//down right
		set_label(x+1, y-1, L);
	}

	
}

void reset_label(int thresh){
	//printf("resetting label\n");
	int i;
	int yin, xin;
	printf("area: %d\n", area);
	if(area<thresh){
		printf("thresholding small area out\n");
		for(i=0; i < area; i++){
			xin = x_coord[i];
			yin = y_coord[i];
			im.u[yin][xin]=1;//mark as junk pixel
		}
		
	}
	area = 0; //reset global variable
	printf("reset! area:%d\n", area);
}
	

