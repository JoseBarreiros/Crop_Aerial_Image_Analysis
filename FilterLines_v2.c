/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
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

int getMode(int a[], int);
int getMax(int a[], int);
int getMin(int b[], int);
void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (im2);                      /* temp image structure         */
int region_range;
float b_tolerance;

struct Line_Segments{
	int pixels; //index for coordinates
	int xcoord[1000];//x coordinates of connected line segment
	int ycoord[1000];//y coordinates of connected line segment
	float slope;
  	float b;
        char check;
};

struct Line_Segments MetaData[1000];//create array to store all line segment data



/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j,k,l;                     	  /* index counters               */
		int L=1;			  /* Label weight		  */
		int first;			  /* variable for pflag block	  */
		int y_diff, x_diff;
		int min_index, max_index;
		

		VXparse(&argc, &argv, par);       /* parse the command line       */

		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&im2, &im, 0,0,0,0);       /* image structure with border  */

		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */

		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
				im2.u[y][x]=0;//clear second output image
			}
		}

		int neg=0;//count negative slopes
		int pos=0;//count positive slopes
		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] == 255 && im.u[y][x]==0){ //object label found and current output label not set
					set_label(x, y, L);
					max_index = getMax(MetaData[L-1].xcoord, MetaData[L-1].pixels);
					min_index = getMin(MetaData[L-1].xcoord, MetaData[L-1].pixels);
					y_diff = MetaData[L-1].ycoord[max_index] - MetaData[L-1].ycoord[min_index];
					//printf("y diff: %d\n", y_diff);
					x_diff = MetaData[L-1].xcoord[max_index] - MetaData[L-1].xcoord[min_index];
					//printf("x diff: %d\n", x_diff);
					MetaData[L-1].slope = (float) y_diff/x_diff;
					
					MetaData[L-1].b= (float) MetaData[L-1].ycoord[max_index]- MetaData[L-1].slope * MetaData[L-1].xcoord[max_index]; 					
					if(MetaData[L-1].slope>0) ++pos;
					else ++neg;
					printf("L: %d\t Slope: %.3f\tx_diff:%d\ty_diff:%d\tb:%.3f\n", L, MetaData[L-1].slope, x_diff, y_diff, MetaData[L-1].b);
					L++;
				}
			}
		}

b_tolerance=10.0;	

			
		for(i=0; i < (L-1); i++) {
printf("\ni:%d\n",i);
for(j=1; j < (L-i); j++) {
printf("j:%d ",j);
			if (((MetaData[i].b-b_tolerance)<=MetaData[i+j].b)&&(MetaData[i+j].b<=(MetaData[i].b+b_tolerance))){
				for(k=0; k < MetaData[i].pixels; k++){
					int im2_x = MetaData[i].xcoord[k];
					int im2_y = MetaData[i].ycoord[k];
					im2.u[im2_y][im2_x]=255;
				} 
MetaData[i].check='y';
MetaData[i+j].check='y';
			printf("L: %d\tslope: %.2f\tb: %.2f\tb+1: %.2fcheck: %c\n", (i+1), MetaData[i].slope, MetaData[i].b, MetaData[i+j].b, MetaData[i].check);

			}
}
		}


//if L has not a line match then plot
for(i=0; i < (L-1); i++) {
if (MetaData[i].check!='y'){
				for(k=0; k < MetaData[i].pixels; k++){
					int im2_x = MetaData[i].xcoord[k];
					int im2_y = MetaData[i].ycoord[k];
					im2.u[im2_y][im2_x]=255;
				} 
}
}
//		

	Vfwrite(&im2, OVAL);             /* write image file                */
	

	exit(0);
	}




int getMode(int a[],int size) {
	int maxValue = 0, maxCount = 0, i, j;
	int negative = 0;
	
	if(a[0]<0){
		negative = 1;
		for(i=0; i < size; ++i){
			a[i]=a[i]*-1;
		}
	}

	for (i = 0; i < size; ++i) {
		int count = 0;

		for (j = 0; j < size; ++j) {
			if (a[j] == a[i]) ++count;
			 
		}

		if (count > maxCount) {
		maxCount = count;
		maxValue = a[i];
		}
	}
	if(negative==1) maxValue = maxValue*-1;

	return maxValue;
}

int getMax(int coords[], int size){//gets index of highest number in array
	//printf("Enter getMax\n");
	int i, max, maxIndex;

	max=coords[0];
	for(i=0; i<size; i++){
		if(max<=coords[i]){
			max=coords[i];
			maxIndex = i;

		}
	}
	//printf("maxIndex: %d\n", maxIndex);
	return maxIndex;
}

int getMin(int coords[], int size){
	//printf("Enter getMin\n");
	int i, min, minIndex;
	//int filtered_coords2[size];

	min=coords[0];
	for(i=0; i<size; i++){
		if(min>=coords[i]){
			min=coords[i];
			minIndex = i;
		}
	}
	//printf("minIndex: %d\n", minIndex);
	return minIndex;
}


void set_label(int x, int y, int L)
{
	//set output pixel to label value
	int index = MetaData[L-1].pixels;
//	printf("L: %d\tPixels: %d\n", L, MetaData[L-1].pixels);
	im.u[y][x] = L;
	MetaData[L-1].xcoord[index]=x;
	MetaData[L-1].ycoord[index]=y;
	MetaData[L-1].pixels++;//increment pixel index(# of coordinates)
	
	

	if(tm.u[y+1][x]==255 && im.u[y+1][x]==0){//up
		set_label(x, y+1, L);	
	}

	if(tm.u[y-1][x]==255 && im.u[y-1][x]==0){//down
		set_label(x, y-1, L);	
	}

	if(tm.u[y][x+1]!=0 && im.u[y][x+1]==0){//right
		set_label(x+1, y, L);
	}

	if(tm.u[y][x-1]!=0 && im.u[y][x-1]==0){//left
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
