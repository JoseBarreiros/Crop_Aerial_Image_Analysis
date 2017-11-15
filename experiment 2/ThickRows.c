/*******************************************************************/
/* DrawRows      Draw complete lines from a seed segments of lines. It also draw lines within the same slope of the seed to complete the image */
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
void plot_line(float,float,int);
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

int compare (const void * p, const void * q)
{

return ( *(int*)p - *(int*)q );
}


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
					
					x_diff = MetaData[L-1].xcoord[max_index] - MetaData[L-1].xcoord[min_index];
					
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

}

//
			
		for(i=0; i < (L-1); i++) {
		
printf("L: %d\tslope: %.2f\tb: %.2f\t\n", (i+1), MetaData[i].slope, MetaData[i].b);


		}



//
float sum,sum2,distance, avg_slope;
sum=0;
sum2=0;
float di;
		for(i=0; i < (L-3); i++) {
di=MetaData[i+1].b-MetaData[i].b;
printf("distance:%0.2f",di);
sum=sum+MetaData[i+1].b-MetaData[i].b;
printf("Sum i:%.2f\n" ,sum);
		}
		for(i=0; i < (L-1); i++) {
sum2=sum2+MetaData[i].slope;
		}
distance=abs(sum)/(L-3);
avg_slope=sum2/(L-1);
printf("Distance:%.2f\tL:%d\tSum:%.2f\n", distance,L-1,sum);
printf("Average_slope:%.2f\tL:%d\tSum:%.2f\n", avg_slope,L-1,sum2);
//

int thick, row_distance=4;
thick= (int) distance/2;
printf("thick: %d",thick);
//
for(j=0; j < (L-1); j++) {
if ((thick-row_distance)>=0){
for (i=0; i<=(thick-row_distance);i++){
plot_line(MetaData[j].b+i,avg_slope,10*j+30);
plot_line(MetaData[j].b-i,avg_slope,10*j+30);
}
}
}

//		

	Vfwrite(&im2, OVAL);             /* write image file                */
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
			im2.u[y][x]=L0;}//  setPixel(x,y);
    
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

	return maxIndex;
}

int getMin(int coords[], int size){
	
	int i, min, minIndex;
	
	min=coords[0];
	for(i=0; i<size; i++){
		if(min>=coords[i]){
			min=coords[i];
			minIndex = i;
		}
	}
	
	return minIndex;
}




void set_label(int x, int y, int L)
{
	//set output pixel to label value
	int index = MetaData[L-1].pixels;

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


