/*******************************************************************/
/* cclabel      Compute local max operation on a single byte image */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */
#include <stdbool.h>
#include <math.h>

VXparam_t par[] =             /* command line structure            */
	{ /* prefix, value,   description                         */   
	{    "if=",    0,   " input file  vtemp: local max filter "},
	{    "of=",    0,   " output file "},

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

struct CropsCOM{
	unsigned short area;//pixel count
	unsigned int COM_x;
	unsigned int COM_y;
	unsigned int average;
	unsigned int row_index;
	unsigned int crop_index;
unsigned int max_crop_row;
};

void plot_line(int,int,int,int,int);

struct CropsCOM MetaDataArray[1000];
int compare (const void * p, const void * q)
{


return ( *(int*)p - *(int*)q );
}



/* Write a program called cclabel.c that accepts a segmented image as input and generates a labeled output image in which all pixels of a given connected component are assigned a unique number. Label numbers should be in sequence 1,2,...,n; where n is the number of connected components in the input image. The program cclabel will use a recursive subprocedure called setlabel. */
main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j,k,yin,xin,x_b,y_b;              /* index counters               */
		int L=1;
float av_temp;
int int_temp;
			  /* Label weight		  */
unsigned int crop_count=0;
		bool redo_label = false;	  /* set true if we want to redo set label with same L */
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
		//		im2.u[y][x]=0;
			}
		}
		for (i = 0 ; i < 1000 ; i++) {
			MetaDataArray[i].area=0;
			MetaDataArray[i].COM_x=0;
			MetaDataArray[i].COM_y=0;
			MetaDataArray[i].row_index=0;

		//	MetaDataArray[i].row=0;

		}

		for (y = im.ylo ; y <= im.yhi ; y++) {
			for (x = im.xlo; x <= im.xhi; x++)  {
				if(tm.u[y][x] != 0 && im.u[y][x]==0){ //object label found and current output label not set
					set_label(x,y,L);		

					MetaDataArray[L-1].COM_x = MetaDataArray[L-1].COM_x/MetaDataArray[L-1].area;
					MetaDataArray[L-1].COM_y = MetaDataArray[L-1].COM_y/MetaDataArray[L-1].area;
					MetaDataArray[L-1].average = MetaDataArray[L-1].average/MetaDataArray[L-1].area;
					

//if (tm.u[MetaDataArray[L-1].COM_y][MetaDataArray[L-1].COM_x]==(10*L+30)){}

av_temp=MetaDataArray[L-1].average/10.0f;

int_temp=round(av_temp-2);
MetaDataArray[L-1].row_index=int_temp;

printf("Index: %d\t COM_x: %d\t COM_y: %d\t Area:%d\t Row_index:%d\n", L-1, MetaDataArray[L-1].COM_x, MetaDataArray[L-1].COM_y,MetaDataArray[L-1].area,MetaDataArray[L-1].row_index);
					L++;//increment label after finishing a connected component
				
				}
			}
		}
///////order by row_index
unsigned int array[255];
unsigned int temp[255][255];
//
int elem=0;
for (j=0; j < (L-1); j++) {
array[j] =MetaDataArray[j].row_index ;
elem++;
}

qsort (array,elem, sizeof(int), compare);
int m=0;
for (j=0; j < (L-1); j++) {

for (k=0; k < (L-1); k++){
if(j==0){
if ((array[j] == MetaDataArray[k].row_index)){
temp[m][0]=MetaDataArray[k].area;
temp[m][1]=MetaDataArray[k].COM_x;
temp[m][2]=MetaDataArray[k].COM_y;
temp[m][3]=MetaDataArray[k].average;
temp[m][4]=MetaDataArray[k].row_index;
m++;
}}
else{
if ((array[j] == MetaDataArray[k].row_index)&&(array[j-1]!=array[j])) {
temp[m][0]=MetaDataArray[k].area;
temp[m][1]=MetaDataArray[k].COM_x;
temp[m][2]=MetaDataArray[k].COM_y;
temp[m][3]=MetaDataArray[k].average;
temp[m][4]=MetaDataArray[k].row_index;
m++;
}}
}
}


for (j=0; j < (L-1); j++) {
MetaDataArray[j].area=temp[j][0];
MetaDataArray[j].COM_x=temp[j][1];
MetaDataArray[j].COM_y=temp[j][2];
MetaDataArray[j].average=temp[j][3];
MetaDataArray[j].row_index=temp[j][4];
}

//////// visual order by Row_index
printf("ORDERED:\n");
	for (j = 0; j < L-1; j++) {		
printf("Index: %d\t COM_x: %d\t COM_y: %d\t Row_index:%d\t Area:%d\n", j, MetaDataArray[j].COM_x, MetaDataArray[j].COM_y,MetaDataArray[j].row_index,MetaDataArray[j].area);
}
unsigned int max_row;
max_row=MetaDataArray[L-2].row_index;

printf("\nL:%d\tMAX_ROW:%d\n",L,max_row);
///////
/////order by COM_x within a row
unsigned int array2[255];
unsigned int array2_temp[255];
int ii;
int first_index;
//ii=1;
for (ii=1;ii<=max_row;ii++){
elem=0;
first_index=0;
//clear arrays
		for (i = 0; i < (L); i++) {
		array2_temp[i]=0;
		}
		for (i = 0; i < L; i++) {
			array2[i]=0;		
		}


		for (i = 0; i < L-1; i++) {
			if(MetaDataArray[i].row_index==(ii)){
			array2[i]=MetaDataArray[i].COM_x;		
			elem++;			
			}	
		}
		qsort(array2,L, sizeof(int), compare);	
		for (i = 0; i < L; i++) {printf("COM_x:%d\n",array2[i]);}//visual
		//printf("L:%d",L);
		
		for (i = 0; i < elem; i++) {
		array2_temp[elem-i-1]=array2[L-i-1];
		}	

		for (i = 0; i < elem; i++) {printf("COM2_x:%d\n",array2_temp[i]);}	//visual
///===pass elements to temp
for (j=0; j < elem; j++) {
for (k=0; k < (L-1); k++){
if ((array2_temp[j] == MetaDataArray[k].COM_x)&&(MetaDataArray[k].row_index==ii)){
temp[j][0]=MetaDataArray[k].area;
temp[j][1]=MetaDataArray[k].COM_x;
temp[j][2]=MetaDataArray[k].COM_y;
temp[j][3]=MetaDataArray[k].average;
temp[j][4]=MetaDataArray[k].row_index;
}}}

for (i=0; i < (L-1); i++){
if(MetaDataArray[i].row_index==(ii)){
first_index=i;
}
}
for (j=0; j < elem; j++) {

printf("\nfirst:%d\n",first_index);
MetaDataArray[first_index-j].area=temp[elem-j-1][0];
MetaDataArray[first_index-j].COM_x=temp[elem-j-1][1];
MetaDataArray[first_index-j].COM_y=temp[elem-j-1][2];
MetaDataArray[first_index-j].average=temp[elem-j-1][3];
MetaDataArray[first_index-j].row_index=temp[elem-j-1][4];
}

for (j = 0; j < L-1; j++) {		
printf("II:%d\t Index: %d\t COM_x: %d\t COM_y: %d\t Row_index:%d\n",ii, j, MetaDataArray[j].COM_x, MetaDataArray[j].COM_y,MetaDataArray[j].row_index);
}

}

/////


//count crops within a row
for (j = 0; j < L-1; j++){
		for (i = 0; i < L-1; i++) {
			if(MetaDataArray[i].row_index==(j+1)){
						
			crop_count++;
			//printf("CC:%d\n",crop_count);
			MetaDataArray[i].crop_index=crop_count;	
			}		
		}
for (i = 0; i < L-1; i++) {
if(MetaDataArray[i].row_index==(j+1)){
MetaDataArray[i].max_crop_row=crop_count;}}

		crop_count=0;
}


for (j = 0; j < L-1; j++) {		
printf("Index: %d\t COM_x: %d\t Row_index:%d\t Cr_index:%d\t Max:%d\t Area:%d\n", j, MetaDataArray[j].COM_x,MetaDataArray[j].row_index,MetaDataArray[j].crop_index,MetaDataArray[j].max_crop_row,MetaDataArray[j].area);
}
	
//write file
int index_graph;
float b,slope;
int Row_st[255][255];
int x_add_1,y_add_1,x_add_1_b,y_add_1_b;
float delta_x,delta_y;
/*
for(i=0;i<(L-1);i++){
if(i==0){
Row_st[i][0]=1;
Row_st[i][1]=MetaDataArray[0].max_crop_row; 
k=1;}
else{
if(MetaDataArray[i].max_crop_row != MetaDataArray[i+1].max_crop_row){
Row_st[k][0]=k+1;
Row_st[k][1]=MetaDataArray[i].max_crop_row;
k++;}
}
}
*/
int sum=0;
for(i=0;i<max_row;i++){
sum=MetaDataArray[sum].max_crop_row+sum;
printf("sum:%d\n",sum);
Row_st[i][0]=i+1;
Row_st[i][1]=MetaDataArray[sum-1].max_crop_row; 

}

printf("debug:\n");
for(i=0;i<(L-1);i++){
printf("row: %d\t max:%d\n",Row_st[i][0],Row_st[i][1]);
}
printf("Num_Rows:%d\n",max_row);
k=0;
int areaT=15;
int tempA;
for (ii=1;ii<=max_row;ii++)	{
	for (i = 0; i < Row_st[ii-1][1]; i++) { //Row_st[ii][0]
//printf("area:%d\n",MetaDataArray[k].area);
tempA=MetaDataArray[k].area;
		//if(tempA>=areaT){//filter by area 
				y = MetaDataArray[k].COM_y;
				x = MetaDataArray[k].COM_x;

			if(MetaDataArray[k+1].row_index==MetaDataArray[k].row_index){				
				y_add_1 = MetaDataArray[k+1].COM_y;
				x_add_1 = MetaDataArray[k+1].COM_x;
			}
			else{
				y_add_1 = MetaDataArray[k].COM_y;
				x_add_1 = MetaDataArray[k].COM_x;
			}
			//k++;

			index_graph=((10*MetaDataArray[k].row_index)+MetaDataArray[k].crop_index);
				//tmf.u[y][x]=index_graph;


//plot lines
			printf("i:%d\t Row#:%d\t x:%d y:%d x+1:%d y+1:%d\t Area:%d\n",i,ii,x,y,x_add_1, y_add_1,tempA);

			if((x_add_1==x)&&(y_add_1==y)){
			}
			else{


			plot_line(x,y,x_add_1,y_add_1,index_graph);
			}
		//}
k++;
	}
}

float b0,avg_slope,b0_b;
int delta_y_2,delta_x_2;
avg_slope=0;
int kk=0;
for(i=0;i<(L-1);i++){
if((MetaDataArray[i].crop_index==2)&&(MetaDataArray[i].max_crop_row>1)){
kk++;
delta_y_2=(MetaDataArray[i+1].COM_y-MetaDataArray[i].COM_y);
delta_x_2=(MetaDataArray[i+1].COM_x-MetaDataArray[i].COM_x);
avg_slope=((float)delta_y_2/(float)delta_x_2)+avg_slope;
printf("delta_y:%d\t delta_x:%d\t avgslope:%0.2f\n",delta_y_2,delta_x_2,avg_slope);
}
}
avg_slope=((float)avg_slope/(float)kk);
printf("avgslope:%0.2f\n",avg_slope);
//}

k=0;
//plot lines from left border to first point
x=0;
for(j=1;j<=max_row;j++){
for(i=0;i<(L-1);i++){
if ((MetaDataArray[i].crop_index==1)&&(MetaDataArray[i].row_index==j)){
y_add_1=MetaDataArray[i].COM_y;
x_add_1=MetaDataArray[i].COM_x;
b0=(y_add_1-(avg_slope*x_add_1));
y=b0;

//last point
k=i+MetaDataArray[i].max_crop_row-1;
printf("row:%d\t x:%d y:%d x+1:%d y+1:%d\t k:%d\n",j,x,y,x_add_1, y_add_1,k);

x_add_1_b=255;
y_b=MetaDataArray[k].COM_y;
x_b=MetaDataArray[k].COM_x;
b0_b=(y_b-(avg_slope*x_b));
y_add_1_b=((avg_slope*x_add_1_b)+b0_b);
index_graph=((10*MetaDataArray[k].row_index)+MetaDataArray[k].crop_index);

//

}
}
plot_line(x,y,x_add_1,y_add_1,j*10);
plot_line(x_b,y_b,x_add_1_b,y_add_1_b,index_graph+1);
}
//plot lines from last point to right border
/*
x_add_1=255;
k=0;
for(j=1;j<=max_row;j++){
for(i=0;i<(L-1);i++){
k=
if ((MetaDataArray[i].crop_index==MetaDataArray[k].max_crop_row)&&(MetaDataArray[i].row_index==j)){
y=MetaDataArray[i].COM_y;
x=MetaDataArray[i].COM_x;
b0=(y-(avg_slope*x));
y_add_1=((avg_slope*x_add_1)+b0);

printf("row:%d\t x:%d y:%d x+1:%d y+1:%d\n",j,x,y,x_add_1, y_add_1);
}
}
plot_line(x,y,x_add_1,y_add_1,lastpixelvalue+1);
}

*/


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



/* Function that returns -1,0,1 depending on whether x */
/* is <0, =0, >0 respectively */
#define sign(x) ((x>0)?1:((x<0)?-1:0))

void plot_line(int x1,int y1,int x2,int y2,int value){
//float b0,float slope0, int value){
//int x1,y1,x2,y2;
//float t_y2;

//find min ext point
//x1=0;
//y1=b0;
//find max ext point
//x2=255;
//t_y2=(slope0*255)+b0;
//y2=(int)t_y2;
//printf("y1:%d  ",y2);
//

int dx, dy, x, y, d, s1, s2, swap=0, temp;

  dx = abs(x2 - x1);
  dy = abs(y2 - y1);
  s1 = sign(x2-x1);
  s2 = sign(y2-y1);
//printf("dx:%d\t dy:%d\t s1:%d\t s2:%d\n",dx,dy,s1,s2);
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
//printf("x:%d\t y:%d\n",x,y);
  if (((x>=0)&&(x<=255))&&((y>=0)&&(y<=255))){
			tmf.u[y][x]=value;
}//  setPixel(x,y);
    
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

