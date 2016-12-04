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
	{    "of2=",    0,   " output file2 "},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
//#define  OVAL2   par[2].val
void set_label(int, int, int);
//void reset_label(int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (tmf);                      /* temp image structure         */
Vfstruct (im2);                      /* temp image structure         */

struct CropsCOM{
	unsigned short area;//pixel count
	//unsigned short COM_x;//x coordinate of center of mass
	//unsigned short COM_y;//y coordinate of center of mass
	//unsigned char row; //row
	//unsigned short x_coord[1000];
	//unsigned short y_coord[1000];
	unsigned int COM_x;
	unsigned int COM_y;
	unsigned int average;
};


struct CropsCOM MetaDataArray[1000];



/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j,yin,xin;              /* index counters               */
		int L=1;			  /* Label weight		  */
		bool redo_label = false;	  /* set true if we want to redo set label with same L */
		VXparse(&argc, &argv, par);       /* parse the command line       */

		Vfread(&im, IVAL);                /* read image file              */
		//Vfembed(&im2, &im, 1,1,1,1);       /* image structure with border  */
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
		//		im2.u[y][x]=0;
			}
		}
		for (i = 0 ; i < 1000 ; i++) {
			MetaDataArray[i].area=0;
			MetaDataArray[i].COM_x=0;
			MetaDataArray[i].COM_y=0;
		//	MetaDataArray[i].row=0;

		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] != 0 && im.u[y][x]==0){ //object label found and current output label not set
					set_label(x,y,L);		

					MetaDataArray[L-1].COM_x = MetaDataArray[L-1].COM_x/MetaDataArray[L-1].area;
					MetaDataArray[L-1].COM_y = MetaDataArray[L-1].COM_y/MetaDataArray[L-1].area;
					MetaDataArray[L-1].average = MetaDataArray[L-1].average/MetaDataArray[L-1].area;
					printf("Index: %d\t COM_x: %d\t COM_y: %d\n", L-1, MetaDataArray[L-1].COM_x, MetaDataArray[L-1].COM_y);
					L++;//increment label after finishing a connected component
				
				}
			}
		}
		
		for (i = 0; i < L-1; i++) {
			if(MetaDataArray[i].area!=0){
				y = MetaDataArray[i].COM_y;
				x = MetaDataArray[i].COM_x;
				tmf.u[y][x]=MetaDataArray[i].average;
			}
		}

	
//	Vfwrite(&im, OVAL2);
	Vfwrite(&tmf, OVAL);             /* write image file                */
	exit(0);
}

void set_label(int x, int y, int L)
{
	im.u[y][x] = L;
	//MetaDataArray[L-1].x_coord[MetaDataArray[L-1].area] = x;
	//MetaDataArray[L-1].y_coord[MetaDataArray[L-1].area] = y;
	MetaDataArray[L-1].area = MetaDataArray[L-1].area + 1;
	MetaDataArray[L-1].COM_x = MetaDataArray[L-1].COM_x + x;
	MetaDataArray[L-1].COM_y = MetaDataArray[L-1].COM_y + y;
	MetaDataArray[L-1].average += tm.u[y][x];


	
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
/*
void reset_label(int L){
	printf("resetting label\n");
	int k;
	int label_size = MetaDataArray[L-1].area;
	MetaDataArray[L-1].area=0;
	for (k=0; k<label_size; k++) {
		im.u[
		MetaDataArray[L-1].x_coord[k]=0;
		MetaDataArray[L-1].y_coord[k]=0;
	}
	MetaDataArray[L-1].COM_x = 0;
	MetaDataArray[L-1].COM_y = 0;
	
}
*/
