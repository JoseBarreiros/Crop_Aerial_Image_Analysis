	/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
	{    "of2=",    0,   " output file 2"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define	 OVAL2  par[2].val

int getMode(int a[], int);
int getMax(int a[], int);
int getMin(int b[], int);
void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (im2);                      /* temp image structure         */

struct Line_Segments{
	int pixels; //index for coordinates
	int xcoord[1000];//x coordinates of connected line segment
	int ycoord[1000];//y coordinates of connected line segment
	float slope;
};

struct Line_Segments MetaData[1000];//create array to store all line segment data



/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j;                     	  /* index counters               */
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
					if(MetaData[L-1].slope>0) ++pos;
					else ++neg;
					printf("L: %d\t Slope: %.3f\tx_diff:%d\ty_diff:%d\n", L, MetaData[L-1].slope, x_diff, y_diff);
					L++;
				}
			}
		}
		/* This if-else block will filter the lines depending on the slopes. Checks if their are more positive or negative numbers, 			then it will find the mode and find values within .1 of that then get the average*/
		float mode;
		float filtered_mean=0;
		/* crops have positive slope */
		if(pos>neg){/* filter out extranneous slopes, multiply slope and floor it to number close  */
			float filtered_slopes[pos];
			int truncated_slopes[pos];
			int posIndex = 0;
			for (i = 0; i < (L-1) ; i++) {
				if(MetaData[i].slope>0){
					filtered_slopes[posIndex] = MetaData[i].slope;
					truncated_slopes[posIndex] = truncf(MetaData[i].slope*10);
					printf("slopes: %.2f\n", filtered_slopes[posIndex]);
					posIndex++;
				}
			}
			mode = 	(float) getMode(truncated_slopes, posIndex)/10.0;
			int real_pos=0; 
			
			//calculate average of slopes <.1 tolerance of the mode of floored slops
			for (i=0; i < (posIndex-1); i++){
				if( abs((100*filtered_slopes[i])-(100*mode))<abs(20*mode)){
					printf("real neg: %.2f\n", filtered_slopes[i]);
					real_pos++;
					filtered_mean = filtered_mean + filtered_slopes[i]*100;
				}
			}
			printf("real pos: %d\n", real_pos);
			filtered_mean = (float) (filtered_mean/real_pos)/100.0;
			printf("filtered_mean: %.2f\n", filtered_mean);

		/* crops rows have negative slope */
		}else{
			float filtered_slopes[neg];
			int truncated_slopes[neg];
			int negIndex =0;
			for (i=0; i < (L-1) ; i++) {
				if(MetaData[i].slope<0){
					filtered_slopes[negIndex] = MetaData[i].slope; 
					truncated_slopes[negIndex] = truncf(MetaData[i].slope*10);//truncate and multiply by 10 for mode calculations 
					printf("truncated slope: %d\tog slope: %.2f\n", truncated_slopes[negIndex], filtered_slopes[negIndex]);
									
					negIndex++;
					
				}
			}

			mode = 	(float) getMode(truncated_slopes, negIndex)/10.0;
			printf("mode: %.2f\n", mode);
			int real_neg=0; 
			
			//calculate average of slopes <.1 tolerance of the mode slope
			for (i=0; i < (negIndex-1); i++){
				if( abs((100*filtered_slopes[i])-(100*mode))<abs(20*mode)){
					printf("real neg: %.2f\n", filtered_slopes[i]);
					real_neg++;
					filtered_mean = filtered_mean + filtered_slopes[i]*100;
				}
			}
			printf("real negs: %d\n", real_neg);
			filtered_mean = (float) (filtered_mean/real_neg)/100.0;
			printf("filtered_mean: %.2f\n", filtered_mean);
			
		}
		//plot lines with slopes that are within .02 of slope		
		for(i=0; i < (L-1); i++) {

			if(abs(MetaData[i].slope*100 - 100*filtered_mean)<=abs(10*filtered_mean)){//<=.03 from mean slope
				//int num_coords = MetaData[i].pixels;
				for(j=0; j < MetaData[i].pixels; j++){
					int im2_x = MetaData[i].xcoord[j];
					int im2_y = MetaData[i].ycoord[j];
					
					im2.u[im2_y][im2_x]=255;
				} 
				//printf("L: %d\tslope: %.2f\n", (i+1), MetaData[i].slope);
			}
		}
	
		
		

	
		

	Vfwrite(&im, OVAL);             /* write image file                */
	Vfwrite(&im2, OVAL2);             /* write image file                */
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
	
	//printf("Exit getMax\n");
	return maxIndex;
}

int getMin(int coords[], int size){
	//printf("Enter getMin\n");
	int i, min, minIndex;
	min=coords[0];
	for(i=0; i<size; i++){
		if(min>=coords[i]){
			min=coords[i];
			minIndex = i;
		}
	}
	//printf("Exit getMin\n");
	return minIndex;
	
}


void set_label(int x, int y, int L)
{
	//set output pixel to label value
	im.u[y][x] = L;
	MetaData[L-1].xcoord[MetaData[L-1].pixels]=x;
	MetaData[L-1].ycoord[MetaData[L-1].pixels]=y;
	MetaData[L-1].pixels++;//increment pixel index(# of coordinates)
	
	
	/*check if the neighboring pixels are object pixels and if
	output label for corresponding pixels have been set yet*/
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
