/*******************************************************************/
/* FilterLinesHough Filter in Acummulator space the points with    */ 	
/* the same slope 						   */
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
	int xcoord[255];//x coordinates of connected line segment
	int ycoord[255];//y coordinates of connected line segment
	float slope;
  	float b;
        char check;
};

struct Line_Segments MetaData[255];//create array to store all line segment data


int compare (const void * p, const void * q)
{

return ( *(int*)p - *(int*)q );
}


main(argc, argv)
	int argc;
	char *argv[];
	{
		int y,x,i,j,k,l;    /* index counter */		        		int y_diff, x_diff;
		int min_index, max_index;
		int count=0;
		int count_v[255];
		int temp_v[255];	

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

b_tolerance=6.0;	

		for (y = im.ylo ; y <= im.yhi ; y++) {
		count=0;
			for (x = im.xlo; x <= im.xhi; x++)  {
				if (tm.u[y][x]!=0){
				count=count+1;
				}
			}
		count_v[y]=count;
		temp_v[y]=count;
		}


		for (j=0; j < 255; j++) {
		printf("count:%d y:%d  ",count_v[j],j);
		}

		qsort (temp_v,255, sizeof(int), compare);

		for (j=0; j < 255; j++) {
		printf("%d   ",temp_v[j]);
		}

		//// select 1st most frecuent values
		for (j=0; j < 255; j++) {
		if((count_v[j]==temp_v[254])&&(count_v[j]!=0)){
		printf("Coord_max_count:%d   ",j);
		for (k=0; k < 255; k++) {
		im2.u[j][k]=tm.u[j][k];}
		}
		}
		//2nd most frecuent values

		for (j=0; j < 255; j++) {
		if((count_v[j]==temp_v[253])&&(count_v[j]!=0)){
		printf("Coord_max_count 2nd:%d   ",j);
		for (k=0; k < 255; k++) {
		im2.u[j][k]=tm.u[j][k];}
		}
		}


	Vfwrite(&im2, OVAL);             /* write image file */
	

	exit(0);
	}


