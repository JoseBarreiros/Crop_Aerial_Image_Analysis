#Crop_AerialImageAnalysis.
#Computer Vision-ECE5470-Cornell University. 
#Final Project

Authors:
Jonathan Wu (jzw8@cornell.edu)
Jose Barreiros (jab879@cornell.edu)

This code describe an image processing algorithm to identify regions of replantation in a crop of sugarcane using aerial infrared imagery.

Implemented in C using VisionX library (http://www.via.cornell.edu/visionx/cguide/fpage.html)

Problem:
Identify and highlight gaps between sugarcane crops in each row to designate regions for replantation due to the reduction of productivity in each growing cycle.

Algorithm overview:
1. Crop segmentation
2. Row identification
3. Replantation region selection and annotation.

![alt text](https://github.com/JoseBarreiros/Crop_Aerial_Image_Analysis/blob/master/Media/algorithm1.png)
![alt text](https://github.com/JoseBarreiros/Crop_Aerial_Image_Analysis/blob/master/Media/algorithm2.png)
![alt text](https://github.com/JoseBarreiros/Crop_Aerial_Image_Analysis/blob/master/Media/algorithm3.png)

Results:

![alt text](https://github.com/JoseBarreiros/Crop_Aerial_Image_Analysis/blob/master/Media/results1.png)

Comparison between manual annotation and algorithm output
![alt text](https://github.com/JoseBarreiros/Crop_Aerial_Image_Analysis/blob/master/Media/results2.png)


