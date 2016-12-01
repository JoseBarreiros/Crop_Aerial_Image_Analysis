/***************************************************************************/
/* RegionMetaDataBytes  Compute local max operation on a single byte image */
/***************************************************************************/

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
#define  OVAL2   par[2].val
void set_label(int, int, int, bool);
void reset_label(int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (tmf);                      /* temp image structure         */
Vfstruct (im2);                      /* temp image structure         */

struct Crops{
	unsigned short area;//pixel count
	unsigned short COM_x;//x coordinate of center of mass
	unsigned short COM_y;//y coordinate of center of mass
	unsigned char row; //row
	unsigned short x_coord[1000];//hold x and y coordinates
	unsigned short y_coord[1000];
};

struct Crops MetaDataArray[1000];//structure that keeps metadata about crops


main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j,yin,xin;              /* index counters               */
		int L=1;                        /* Label weight */
		bool redo_label = false;	  /* set true if we want to redo set label with same L */
		VXparse(&argc, &argv, par);       /* parse the command line       */

		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&im2, &im, 1,1,1,1);       /* image structure with border  */
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
				im2.u[y][x]=0;
			}
		}
        
		for (i = 0 ; i < 255 ; i++) {
			MetaDataArray[i].area=0;
			MetaDataArray[i].COM_x=0;
			MetaDataArray[i].COM_y=0;
			MetaDataArray[i].row=0;

		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] == 255 && im.u[y][x]==0){ //object label found and current output label not set
					if(redo_label){
						set_label(x,y,L,redo_label);
						redo_label=false;
					}else{
						set_label(x, y, L, false);
					}
					if(MetaDataArray[L-1].area<20){//threshold out pixel areas less than 20
						redo_label = true;
					}
					MetaDataArray[L-1].COM_x = MetaDataArray[L-1].COM_x/MetaDataArray[L-1].area;
					MetaDataArray[L-1].COM_y = MetaDataArray[L-1].COM_y/MetaDataArray[L-1].area;
					printf("Index: %d\t COM_x: %d\t COM_y: %d\n", L-1, MetaDataArray[L-1].COM_x, MetaDataArray[L-1].COM_y);
					if(!redo_label)L++;//increment label after finishing a connected component
				
				}
			}
		}
		
		for (i = 0; i < 255; i++) {
			if(MetaDataArray[i].area!=0){
				y = MetaDataArray[i].COM_y;
				x = MetaDataArray[i].COM_x;
				tmf.u[y][x]=255;
				for(j=0; j<MetaDataArray[i].area;j++){//draw thresholded area output file
					printf("size: %d\t index: %d\n", MetaDataArray[i].area, j);
					yin = MetaDataArray[i].y_coord[j];
					xin = MetaDataArray[i].x_coord[j];
					//printf("yin: %d\t xin: %d\n", yin, xin);
					im2.u[yin][xin]=255;
				}
			}
		}

		for (y=im.ylo; y<=im.yhi; y++){
			for (x=im.xlo; x<=im.xhi; x++){
				im.u[y][x]=im2.u[y][x];
				if(im2.u[y][x]==255){
					if(im2.u[y+1][x]==0 && im2.u[y+1][x+1]==0 && im2.u[y-1][x+1]==0 && im2.u[y-1][x]==0 && im2.u[y-1][x-1]==0 && im2.u[y][x-1]==0 && im2.u[y+1][x-1]==0){
						im.u[y][x]=0;
					}
				}
				
			}
		}
	
	Vfwrite(&tmf, OVAL2);
	Vfwrite(&im, OVAL);             /* write image file                */
	exit(0);
}

void set_label(int x, int y, int L, bool redo)
{
	if(redo==true){//if redo is set to true
		//erase all data for label
		reset_label(L);
	}
	im.u[y][x] = L;
    //gather metadata
	MetaDataArray[L-1].x_coord[MetaDataArray[L-1].area] = x;
	MetaDataArray[L-1].y_coord[MetaDataArray[L-1].area] = y;
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

	/*Everything below is added for 8connectivity in region growing */
	if(tm.u[y+1][x-1]!=0 && im.u[y+1][x-1]==0){//up left
		set_label(x-1, y+1, L,false);
	}

	if(tm.u[y+1][x+1]!=0 && im.u[y+1][x+1]==0){//up right
		set_label(x+1, y+1, L,false);
	}

	if(tm.u[y-1][x-1]!=0 && im.u[y-1][x-1]==0){//down left
		set_label(x-1, y-1, L,false);
	}

	if(tm.u[y-1][x+1]!=0 && im.u[y-1][x+1]==0){//down right
		set_label(x+1, y-1, L,false);
	}
	
}

void reset_label(int L){
	int k;
	int label_size = MetaDataArray[L-1].area;//get size of coodinate array
	MetaDataArray[L-1].area=0;//reset area
	for (k=0; k<label_size; k++) {//reset all coordinates
		MetaDataArray[L-1].x_coord[k]=0;
		MetaDataArray[L-1].y_coord[k]=0;
	}
	MetaDataArray[L-1].COM_x = 0;
	MetaDataArray[L-1].COM_y = 0;
	
}

