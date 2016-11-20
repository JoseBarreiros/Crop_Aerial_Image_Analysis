/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "of2=",    0,   " output file2 "},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  OVAL2   par[2].val
void set_label(int, int, int, bool);
void calculate_orientation(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (tmf);                      /* temp image structure         */

struct Crops{
	unsigned short area;//pixel count
	unsigned short COM_x;//x coordinate of center of mass
	unsigned short COM_y;//y coordinate of center of mass
	unsigned short u_20;
	unsigned short u_02;
	unsigned short u_11;
	double orientation; 	
	unsigned char row; //row
};


struct Crops MetaDataArray[255];


/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x, i;                    	  /* index counters               */
		int L=1;			  /* Label weight		  */
		VXparse(&argc, &argv, par);       /* parse the command line       */

		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
		Vfembed(&tmf, &im, 0,0,0,0);       /* image structure with border  */
		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
				tmf.u[y][x]=0;//clear other output
			}
		}
		for (i = 0 ; i < 255 ; i++) {
			MetaDataArray[i].area=0;
			MetaDataArray[i].COM_x=0;
			MetaDataArray[i].COM_y=0;
			MetaDataArray[i].row=0;
			MetaDataArray[i].u_20=0;
			MetaDataArray[i].u_02=0;
			MetaDataArray[i].u_11=0;	
			MetaDataArray[i].orientation=0.0;
		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] == 255 && im.u[y][x]==0){ //object label found and current output label not set
					set_label(x, y, L, false);
					if(MetaDataArray[L-1].area<10){
						set_label(x,y,L,true); //use the same label again to connect components if it's less than 10
					} else {
						MetaDataArray[L-1].COM_x = MetaDataArray[L-1].COM_x/MetaDataArray[L-1].area;
						MetaDataArray[L-1].COM_y = MetaDataArray[L-1].COM_y/MetaDataArray[L-1].area;
						printf("Index: %d\t COM_x: %d\t COM_y: %d\n", L-1, MetaDataArray[L-1].COM_x, MetaDataArray[L-1].COM_y);
						L++;//increment label after finishing a connected component
					}
				}
			}
		}
		L=1;//reset Label (index) to 1 to calculate orientation of each crop

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] == 255 && im.u[y][x]==0){ //object label found and current output label not set
					calculate_orientation(x, y, L);
					double cen_moment_ratio = MetaDataArray[L-1].u_11/(MetaDataArray[L-1].u_20 + MetaDataArray[L-1].u_02); 
					MetaDataArray[L-1].orientation = atan(cen_moment_ratio);
					L++;//increment label after finishing a connected component

				}
			}
		}
		
		for (i = 0; i < 1000; i++) {
			if(MetaDataArray[i].area!=0){
				y = MetaDataArray[i].COM_y;
				x = MetaDataArray[i].COM_x;
				tmf.u[y][x]=255;
			}
		}
	
	Vfwrite(&tmf, OVAL2);
	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
	}

void set_label(int x, int y, int L, bool redo)
{
	//printf("IN Method Set_Label\n");
	//printf("IMx: %d\t IMy: %d\t IMval: %d\n", x,y,im.u[y][x]);
	//printf("TMx: %d\t TMy: %d\t TMval: %d\n", x,y,tm.u[y][x]);

	im.u[y][x] = L;
	if(redo==true){//if redo is set to true
		//erase all data
		MetaDataArray[L-1].area=0;
		MetaDataArray[L-1].COM_x=0;
		MetaDataArray[L-1].COM_y=0;
		MetaDataArray[L-1].row=0;
	}
	MetaDataArray[L-1].area = MetaDataArray[L-1].area + 1;
	MetaDataArray[L-1].COM_x = MetaDataArray[L-1].COM_x + x;
	MetaDataArray[L-1].COM_y = MetaDataArray[L-1].COM_y + y;
	
	/*check if the neighboring pixels are object pixels and if
	output label for corresponding pixels have been set yet*/
	if(tm.u[y+1][x]==255 && im.u[y+1][x]==0){
		set_label(x, y+1, L,false);
	}
	if(tm.u[y-1][x]==255 && im.u[y-1][x]==0){
		set_label(x, y-1, L,false);
	}
	if(tm.u[y][x+1]==255 && im.u[y][x+1]==0){
		set_label(x+1, y, L,false);
	}
	if(tm.u[y][x-1]==255 && im.u[y][x-1]==0){
		set_label(x-1, y, L,false);
	}
	
}

void calculate_orientation(int x, int y, int L)
{
	MetaDataArray[L-1].u_11 = MetaDataArray[L-1].u_11 + (x-MetaDataArray[L-1].COM_x)*(y-MetaDataArray[L-1].COM_y);
	MetaDataArray[L-1].u_20 = MetaDataArray[L-1].u_20 + (x-MetaDataArray[L-1].COM_x)*(x-MetaDataArray[L-1].COM_x); 
	MetaDataArray[L-1].u_02 = MetaDataArray[L-1].u_02 + (y-MetaDataArray[L-1].COM_y)*(y-MetaDataArray[L-1].COM_y);
	if(tm.u[y+1][x]==255){
		calculate_orientation(x, y+1, L);
	}
	if(tm.u[y-1][x]==255){
		calculate_orientation(x, y-1, L);
	}
	if(tm.u[y][x+1]==255){
		calculate_orientation(x+1, y, L);
	}
	if(tm.u[y][x-1]==255){
		calculate_orientation(x-1, y, L);
	}
	
}
