	/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},
//	{    "of2=",    0,   " output file 2"},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
//#define	 OVAL2  par[2].val

int getMode(int a[], int);
int getMax(int a[], int);
int getMin(int b[], int);
void plot_line(float,float);
void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (im2);                      /* temp image structure         */
int region_range;
float b_tolerance;

struct Line_Segments{
	int pixels; //index for coordinates
	int xcoord[255];//x coordinates of connected line segment
	int ycoord[255];//y coordinates of connected line segment
	float slope;
  	float b;
        char check;
};

struct Line_Segments MetaData[255];//create array to store all line segment data

int compare (const void * p, const void * q)
{

//  float* dp = (double*)p;   
//float* dq = (double*)q;   

//return ((const int *)p)[1] - ((const int *)q)[1];
return ( *(int*)p - *(int*)q );
}

/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j,k,l,elem;                     	  /* index counters               */
		int L=1;			  /* Label weight		  */
		int first;			  /* variable for pflag block	  */
		int y_diff, x_diff;
		int min_index, max_index;
		

		VXparse(&argc, &argv, par);       /* parse the command line       */

		Vfread(&im, IVAL);                /* read image file              */
		Vfembed(&im2, &im, 0,0,0,0);       /* image structure with border  */
		//Vfembed(&im3, &im, 0,0,0,0);       /* image structure with border  */
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */

		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
				im2.u[y][x]=0;//clear second output image
				//im3.u[y][x]=0;//clear third output image
			}
		}
		//initialize data structure
		//for (i =0; i<255;i++){
		//	MetaData[i].pixels=0;
			
		//}
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

b_tolerance=5.0;	


float array[255];
float temp[255][255];
//
elem=0;
for (j=0; j < (L-1); j++) {
array[j] =MetaData[j].b ;
elem++;
}

qsort (array,elem, sizeof(int), compare);

for (j=0; j < (L-1); j++) {
for (k=0; k < (L-1); k++){
if (array[j] == MetaData[k].b) {
temp[j][0]=MetaData[k].b;
temp[j][1]=MetaData[k].slope;
temp[j][2]=j+1;
}
}
}

for (j=0; j < (L-1); j++) {
MetaData[j].b=temp[j][0];
MetaData[j].slope=temp[j][1];
//MetaData[j].b=temp[j][0];

}

//
			
		for(i=0; i < (L-1); i++) {
//printf("b:%.2f\tslope:%.2f\tL:%.2f\t\n",temp[i][0],temp[i][1],temp[i][2]);
//printf("b:%.2f\t\n",array[i]);		
printf("L: %d\tslope: %.2f\tb: %.2f\t\n", (i+1), MetaData[i].slope, MetaData[i].b);


		}



//
float sum,sum2,distance, avg_slope;
sum=0;
sum2=0;
		for(i=0; i < (L-2); i++) {
sum=sum+MetaData[i+1].b-MetaData[i].b;
		}
		for(i=0; i < (L-1); i++) {
sum2=sum2+MetaData[i].slope;
		}
distance=sum/(L-1);
avg_slope=sum2/(L-1);
printf("Distance:%.2f\tL:%d\tSum:%.2f\n", distance,L-1,sum);
printf("Average_slope:%.2f\tL:%d\tSum:%.2f\n", avg_slope,L-1,sum2);
//


//
for(i=0; i < (L-1); i++) {
plot_line(MetaData[i].b,MetaData[i].slope);
}
//

//
float bmin,bmax;
bmin=MetaData[0].b;
while ((bmin-distance)>=0){
plot_line((bmin-distance),avg_slope);
bmin=bmin-distance;
}
bmax=MetaData[L-2].b;
printf("bmax: %.2f  L:%d",MetaData[L-2].b,L);
while ((bmax+distance)<=510){
plot_line((bmax+distance),avg_slope);
bmax=bmax+distance;
}
//		

	Vfwrite(&im2, OVAL);             /* write image file                */
	//Vfwrite(&im2, OVAL2);             /* write image file                */

	exit(0);
	}

/* Function that returns -1,0,1 depending on whether x */
/* is <0, =0, >0 respectively */
#define sign(x) ((x>0)?1:((x<0)?-1:0))

void plot_line(float b0,float slope0){
int x1,y1,x2,y2;
float t_y2;

//find min ext point
x1=0;
y1=b0;
//find max ext point
x2=255;
t_y2=(slope0*255)+b0;
y2=(int)t_y2;
printf("y1:%d  ",y2);
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
			im2.u[y][x]=255;}//  setPixel(x,y);
    
    while(d >= 0) 
    {
      if(swap) x = x + s1;
      else 
      {
        y = y + s2;
        d = d - 2* dx;
      }
    }
    if(swap) y = y + s2;
    else x = x + s1;
    d = d + 2 * dy;
  }
 // glFlush();


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
//	int filtered_coords1[size];
/*
	for(i=0; i<size; i++){//load coords
		filtered_coords1[i] = coords[i];
		//printf("i: %d\toriginal coords: %d\tfiltered_coords:%d\n", i, coords[i], filtered_coords1[i]);
	}
*/
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
/*
	for(i=0; i<size; i++){
		filtered_coords2[i] = coords[i];
		//printf("i: %d\toriginal coords: %d\tfiltered_coords:%d\n", i, coords[i], filtered_coords2[i]);
	}
*/
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


