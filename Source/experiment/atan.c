#include <stdio.h>
#include <math.h>

#define PI 3.14159265

int main ()
{
   double x, ret, val;
   x = -0.58;
   val = 180.0 / PI;

   ret = atan (x) ;
   printf("The arc tangent of %lf is %lf degrees", x, ret);
   
   return(0);
}
