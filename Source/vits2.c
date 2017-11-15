/*********************************************************************/
/* vits:     Threshold image between two most sig. hgram peaks     

1)Get an initial threshold thresh from the command line if specified 
    or 
Compute the average gray value of image and use it as an initial threshold thresh
2)Consider the two image regions R1 and R2 that are determined by thresh. (i.e. pixels above thresh and pixels below thresh) Compute the average gray values ( avg1, avg2) for R1 and R2. Note that the number of pixels in R1 or R2 may be zero.
3)Compute a new threshold using: thresh = (avg1 + avg2)/2
4)Repeat 2-4 until avg1 and avg2 do not change between successive iterations
5)Apply the threshold to the image (values above thresh set to 255 otherwise 0)*/
/*********************************************************************/

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */

VXparam_t par[] =            /* command line structure               */
{
{  "if=",   0,   " input file, vtpeak: threshold between hgram peaks"},
{  "of=",   0,   " output file "},
{  "th=",    0,   " threshhold value"},
{  "-v",    0,   "(verbose) print threshold information"},
{   0,      0,   0} /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  TVAL   par[2].val
#define  VFLAG  par[3].val

main(argc, argv)
int argc;
char *argv[];
{

    Vfstruct (im);                 /* input image structure          */
    int y,x;                       /* index counters                 */
    int i;

    int thresh;                    /* threshold level*/
    int init_thresh;               /* input threshold value from cmd line or average gray level from original image*/
    int loopcount;		/*counts iterations for while block*/ 
    int pixelcount;		/*counts number of pixels in image*/
    int region1;		/*accumulates pixel values in region1 */
    int r1count;		/*number of pixels in region1 */
    int region2;		/*accumulates pixel values in region2 */
    int r2count;		/*number of pixels in region2 */
    int curr_avg1;		/*average of pixels in region 1: region1/r1count */
    int curr_avg2;		/*average of pixels in region 2: region2/r2count */
    int prev_avg1;		/*old value of curr_avg1 from previous iteration */
    int prev_avg2;		/*old value of curr_avg2 from previous iteration */

			     
    VXparse(&argc, &argv, par);    /* parse the command line         */

	init_thresh = (TVAL ? atoi(TVAL):-1);

    while ( Vfread( &im, IVAL) ) {
        if ( im.type != VX_PBYTE ) {
              fprintf (stderr, "error: image not byte type\n");
              exit (1);
        }
	
	/*compute the average gray value of the image if it hasn't been specified*/
	if (init_thresh < 0 || init_thresh > 255){ 
		init_thresh = 0;
		pixelcount = 0;
		printf("Computing Average Gray Value\n");
		//accumulate all gray values from each pixel in image
		for (y=im.ylo; y<=im.yhi; y++){
			for (x=im.xlo; x<=im.xhi; x++){
				init_thresh = init_thresh + im.u[y][x];
				pixelcount=pixelcount+1;
			}
		}
		printf("init_thresh %d\t pix count: %d\n", init_thresh, pixelcount);
		//calculate average gray scale value
		init_thresh = init_thresh/pixelcount;
	}
	/*perform first iteration of the iterative thresholding*/
	printf("init_thresh: %d\n", init_thresh);
	thresh = init_thresh;
	//pixels >= thresh value
	region1 = 0;
	//number of pixels in region1
	r1count = 0;
	//pixels < thresh value
	region2 = 0;
	//number of pixels in region2
	r2count = 0;
	for (y=im.ylo; y<=im.yhi; y++){
		for (x=im.xlo; x<=im.xhi; x++){
			if(im.u[y][x]>=thresh){
				region1 = region1 + im.u[y][x];
				r1count++;
			}
			else{
				region2 = region2 + im.u[y][x];
				r2count++;
				
			}
		}
		}
	printf("r1count: %d\t r2count:%d\n",r1count, r2count);
	printf("region1: %d\t region2:%d\n",region1, region2);
	
	if(region1==0 && r1count==0){/*case where there are no pixels in region 1*/
		thresh = region2/r2count;
		curr_avg1 = 0;
		curr_avg2 = region2/r2count;
	} else if(region2==0 && r2count==0){/*case where there are no pixels in region 2*/
		thresh = region1/r1count;
		curr_avg1 = region1/r1count;
		curr_avg2 = 0;
	} else{
		curr_avg1 = region1/r1count;
		curr_avg2 = region2/r2count; 
		printf("curr_avg1: %d\t curr_avg2: %d\n", curr_avg1, curr_avg2);
		thresh = (curr_avg1 + curr_avg2)/2;
	}
	//declare prev_avg1 and prev_avg2 so that the loop will execute through at least once
	prev_avg1 = 0;
	prev_avg2 = 0;
	loopcount = 0;
	
	//when the threshold values haven't converged, keep repeating procedure from above until convergence
	while(curr_avg1!=prev_avg1 || curr_avg2!=prev_avg2){ 
		loopcount++;
		printf("loopcount: %d\n", loopcount);
		printf("thresh: %d\n", thresh);
		//set the previous averages to the current averages
		prev_avg1 = curr_avg1; 
		prev_avg2 = curr_avg2;
		//accumulator variables
		region1 = 0;
		r1count = 0;
		region2 = 0;
		r2count = 0;
		//populate regions by iterating over original image
		for (y=im.ylo; y<=im.yhi; y++){
			for (x=im.xlo; x<=im.xhi; x++){
				if(im.u[y][x]>=thresh){
					region1 = region1 + im.u[y][x];
					r1count++;
				}
				else{
					region2 = region2 + im.u[y][x];
					r2count++;
					
				}
			}
		}
		printf("r1count: %d\t r2count:%d\n",r1count, r2count);
		printf("region1: %d\t region2:%d\n",region1, region2);

		if(region1==0 && r1count==0){//case where no pixels in region1
			curr_avg1 = 0;
			curr_avg2 = region2/r2count;
			thresh = curr_avg2;

		} else if(region2==0 && r2count==0) {//case where no pixels in region2
			curr_avg1 = region1/r1count;
			curr_avg2 = 0;
			thresh = curr_avg1;

		} else {		
			curr_avg1 = region1/r1count; 
			curr_avg2 = region2/r2count; 
			thresh = (curr_avg1+curr_avg2)/2;
		}
		printf("prev_avg1: %d\t prev_avg2: %d\n", prev_avg1, prev_avg2);
		printf("curr_avg1: %d\t curr_avg2: %d\n", curr_avg1, curr_avg2);
		//exit loop after it executes a certain number of times
		if(loopcount>=200){
			thresh = (curr_avg1 + curr_avg2)/2;
			break;
		}
	}
	printf("Exit While\n");
	printf("thresh: %d\n", thresh);
  
        /* apply the threshold */
        for (y = im.ylo; y <= im.yhi; y++) {
            for (x = im.xlo; x <= im.xhi; x++) {
                 if (im.u[y][x] >= thresh) im.u[y][x] = 255;
                 else                      im.u[y][x] = 0;
            }
        }
  
        Vfwrite( &im, OVAL);
    } /* end of every frame section */
    exit(0);
}
