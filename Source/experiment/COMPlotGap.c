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
};


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
		int y,x,i,j,k,yin,xin;              /* index counters               */
		int L=1;			  /* Label weight		  */
unsigned int crop_count=0;
float av_temp;
int int_temp;
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

printf("Index: %d\t COM_x: %d\t COM_y: %d\t Area:%d\t Row_index:%d Average:%0.2f\n", L-1, MetaDataArray[L-1].COM_x, MetaDataArray[L-1].COM_y,MetaDataArray[L-1].area,MetaDataArray[L-1].row_index,av_temp);
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
		crop_count=0;
}


for (j = 0; j < L-1; j++) {		
printf("Index: %d\t COM_x: %d\t COM_y: %d\t Row_index:%d\t Cr_index:%d\n", j, MetaDataArray[j].COM_x, MetaDataArray[j].COM_y,MetaDataArray[j].row_index,MetaDataArray[j].crop_index);
}
	
//write file
		for (i = 0; i < L-1; i++) {
			if(MetaDataArray[i].area!=0){
				y = MetaDataArray[i].COM_y;
				x = MetaDataArray[i].COM_x;
				tmf.u[y][x]=((10*MetaDataArray[i].row_index)+MetaDataArray[i].crop_index);
			}
		}



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

