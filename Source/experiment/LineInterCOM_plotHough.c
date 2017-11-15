/*******************************************************************/
/* DrawRows      Draw complete lines from a seed segments of lines. It also draw lines within the same slope of the seed to complete the image */
/*******************************************************************/
#include "stdio.h"
#include "math.h"
#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " image with filtered lines "},
	{    "ig=",    0,   " COM "},
	{    "of=",    0,   " output file "},
{    "of2=",    0,   " output file2 Hough domain "},
	{     0,       0,   0}  /* list termination */
	};
#define  IVAL   par[0].val
#define  IVAL2   par[1].val
#define  OVAL   par[2].val
#define  OVAL2   par[3].val

int getMode(int a[], int);
int getMax(int a[], int);
int getMin(int b[], int);
void plot_line(float,float,int);
void plot_hough(float,float);
void set_label(int, int, int);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (im2);
Vfstruct (im3);                      /* temp image structure         */
Vfstruct (im_com);
Vfstruct (com);
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

		Vfread(&im, IVAL);   
Vfread(&im_com, IVAL2);               /* read image file              */
		Vfembed(&im2, &im, 0,0,0,0);       /* image structure with border  */
		Vfembed(&im3, &im, 0,0,0,0);
		Vfembed(&tm, &im, 1,1,1,1);       /* image structure with border  */
Vfembed(&com, &im_com, 1,1,1,1);

		if ( im.type != VX_PBYTE ) {      /* check image format           */
			fprintf(stderr, "vtemp: no byte image data in input file\n");
			exit(-1);
		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				im.u[y][x]=0; //clear output image
				im2.u[y][x]=0;//clear second output image
im3.u[y][x]=0;//clear second output image				
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
					if(x_diff!=0){
					MetaData[L-1].slope = (float) y_diff/x_diff;}
else{MetaData[L-1].slope=572.9f;}
					
					MetaData[L-1].b= (float) MetaData[L-1].ycoord[max_index]- MetaData[L-1].slope * MetaData[L-1].xcoord[max_index]; 					
					if(MetaData[L-1].slope>0) ++pos;
					else ++neg;
					printf("L: %d\t Slope: %.3f\tx_diff:%d\ty_diff:%d\tb:%.3f\n", L, MetaData[L-1].slope, x_diff, y_diff, MetaData[L-1].b);
					L++;
				}
			}
		}


b_tolerance=5.0;	


float array[1000];
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
int size = 0;
sum2=0;

		for(i=0; i < (L-1); i++) {
		int check;  
	//	printf("truncatedslope: %d\n", trunc(MetaData[i].slope));
	printf("slope: %.2f\n", MetaData[i].slope);
	if(MetaData[i].slope!=572.9f){
	printf("enter\n");
	size++;
	sum2=sum2+MetaData[i].slope;
	}
		}
avg_slope= (float) sum2/size;
printf("Average_slope:%.2f\tL:%d\tSum:%.2f\n", avg_slope,L-1,sum2);
//


//
int yc,xc,b_com;
for (yc =im2.ylo ; yc <= im2.yhi ; yc++) {
	for (xc = im2.xlo; xc <= im2.xhi; xc++)  {
if (com.u[yc][xc]!=0){

b_com=yc-avg_slope*xc;
printf("average:%0.2f\t b_com:%d\t y:%d\t x:%d\n",avg_slope,b_com,yc,xc);
plot_line(b_com,avg_slope,255);
plot_hough(b_com,avg_slope);
}
}
}

//		

	Vfwrite(&im2, OVAL);             /* write image file                */
Vfwrite(&im3, OVAL2); 
	exit(0);
	}


void plot_hough(float b0,float slope0){
	int ro_vx,teta_vx;
	double teta=0;
	double ro=0;
	double b0_d,slope0_d,val;
	b0_d=(double)b0;
	slope0_d=(double)slope0;
	val=180/3.14;
	//printf("   slope:%lf",slope0_d);
	teta=atan(-slope0_d);//3.14; //teta in rad
	//printf("   teta:%lf",teta);
	ro=b0_d*sin(teta);
	//printf("ro:%lf",ro);
	teta_vx=teta*255/3.1416+127.5; //teta_vx in degre
	ro_vx=ro*255/360.62;
	printf("ro_vx:%d	teta:%0.2f    teta_vx:%d	b0:%.2f\n	",ro_vx,teta,teta_vx,b0);
	//printf("ro:%lf	teta_vx:%lf	b0:%.2f\n	",ro,teta,b0);
	im3.u[teta_vx][ro_vx]=255;
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


