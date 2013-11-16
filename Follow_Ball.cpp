

#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>


int main(int argc, char* argv[]){

   // Default capture size - 640x480
    CvCapture * camera = cvCreateCameraCapture (CV_CAP_ANY);
    IplImage *  current_frame = cvQueryFrame (camera);
    CvSize size640x480 = cvSize(current_frame->width, current_frame->height); //tried 640, 480 : 600,500 : 500,600 
    // Open capture device. 0 is /dev/video0, 1 is /dev/video1, etc.
    // Web cam video stream is assinged to this 
    CvCapture *p_capWebCam; 
    //pointer to an image structure.
    //this will be the imput image from webcam
    IplImage *p_imgOriginal; 
    //Pointer to an immage structure 
    //this will be the processed black and white immage 
                            /* Ipl is short for Intel Immage processing library. This is the standard 
                                structure in opencv to work with immages. */
    IplImage *p_imgProcessed; 
    //Necessary storage variable to pass into cvHoughCircles()
    CvMemStorage *p_strStorage; 
    //pointer to an opencv sequence, will be returned by cvHough Circles() and will contain all circles 
    // calling cvGetSeqElem(p_seqCircles, i) willreturn 3 element array of i'th circle (see next variable)
    CvSeq *p_seqCircles;
    //pointer to a 3 element of array of floats
    //[0] => x position of detected object
    //[1] => y position of detected object
    //[2] => Radius of detected object
    float *p_fltXYRadius;         //pointer to a 3 element array of floats 

    int i;                      //loop counter
    char charCheckForEscKey;    //get char to escape

    p_capWebCam = cvCaptureFromCAM(0);  //If you have multiple webcams you might not want to use 0
                                        //However, since i have only one i pass the parameter 0 

    //check if the webcam works 
    if( p_capWebCam == NULL ){
         printf("ERROR: capture is NULL");  //If web cam wasnt plugged in for example we'd exit the program 
         getchar();                         //pause so that user sees the message 
         return(-1);                        //exit the program  
    }

    //declare the two windows 
    cvNamedWindow("Original", CV_WINDOW_AUTOSIZE );     // Origina image from webcam 
    cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);     // Processed image we will use for detecting circles 

    p_imgProcessed = cvCreateImage(size640x480,         //creates the immage for the Processed window
                                    IPL_DEPTH_8U, 1);   //8U is short for 8 bits unsigned  //we use 1 for greyscale if collow we use 3
                                                        //this is a greyscale immage so each pixel can go from 0->255 greyscale 
                                                        //it this was color "Red Green Blue" would be a value for each color: well call this 
                                                        //the depth 

    // Infinate while loop where fram is grabbed from cam and processed 
    while(1){           //for each frame... 

        p_imgOriginal = cvQueryFrame(p_capWebCam);  //get frame from webcam 
        //check to see if the frame capture was sucessfull 
        if(p_imgOriginal == NULL){      //if frame was not capture sucessfully
            printf("ERROR: frame is NULL \n" );
            getchar(); 
            break; 
        } 
        // 44:46 http://www.youtube.com/watch?v=2i2bt-YSlYQ&list=PLbqNFa0YhiNBHuQOwalaP5jNGExrmsGOO
         cvInRangeS(p_imgOriginal,                  //function input        //color
                        CV_RGB(175, 0 , 0),         //this is a macro, we want to capture a red ball at atleast 175 inclusive
                        CV_RGB(255, 200, 200),      //we allow a bit of green and blue: Redis max 256 not inclusive 
                        p_imgProcessed );            //function output       //black and white

         //alocate storage variable 
         p_strStorage   = cvCreateMemStorage(0);    //alocate necessary storage variable to pass int cvHoughCirlces()

         //now we apply agousian smoothing to the processed image. this will make it easyer for next func to pic circles
         cvSmooth(p_imgProcessed,                   //function input 
                    p_imgProcessed,                 //function output 
                    CV_GAUSSIAN,                    //Use Gaussian filter (averages nearby pixels, with closses having more wheight)
                    9,                              //Smothing filter window width 
                    9);                             //Smoothing filter window height  
                                                    //imigin a 9x9 box in the input immage it does stuff smooths out the output

        //fills a sequential structure with all circles in a processed immgage
        p_seqCircles = cvHoughCircles( p_imgProcessed,              //input: hast to be greyscale "no color"
                                        p_strStorage,               //Needs this we need not know why. passing this makes func ret pointer
                                        CV_HOUGH_GRADIENT ,         //Only option for this, two pass algorithim for detecting circles
                                        2,                          // size of this image/ (this value) = accumulator immage 
                                        p_imgProcessed->height/4,   //min distance in pixels between centers of detected circles 
                                        100,                //high threshold of Cany edge detector, called by cvHoughCircles 
                                        50,                 //low threshold of Cany edge detector, called by cvHoughCircles 
                                        10,                 //Minimub circle radius 
                                        400);               //max circle radius 

        for(i = 0; i < p_seqCircles->total; i++){           //for each element in sequential circles structure( i.e. for each object detected)

            p_fltXYRadius = (float*)cvGetSeqElem(p_seqCircles, i); //from the sequential struct, read the i'th value into a pointer to a float 
            printf("ball position x = %f, y = %f, r = %f \n", p_fltXYRadius[0],     //x position at center of circle 
                                                              p_fltXYRadius[1],     //y position at center of circle 
                                                              p_fltXYRadius[2]);    //radius of circle 

            //draw a small green circle at center of detected object
            cvCircle(p_imgOriginal,             //draw on the original image
                cvPoint(cvRound(p_fltXYRadius[0]),  cvRound(p_fltXYRadius[1])),     // center point of circle 
                                                3,                                  //3 pixel radius of circle 
                                                CV_RGB(0, 255, 0),                  //draw pure green
                                                CV_FILLED);                         //thickness, fill in the circle 

            //draw a red circle around a detected object 
            cvCircle(p_imgOriginal,                                                 //draw on the original image
                cvPoint(cvRound(p_fltXYRadius[0]),  cvRound(p_fltXYRadius[1])),     // center point of circle 
                cvRound(p_fltXYRadius[2]),                                           //radius of the circle in pixels
                CV_RGB(255, 0, 0),                                                  //draw pure red
                3);                                                           //thickness of circle in pixels 

        }// end of for loop

        //now we just need to show the images that we already generated 
        cvShowImage("Original", p_imgOriginal);                         //original circle with detectec ball overlay
        cvShowImage("Processed", p_imgProcessed);                       //image after processing 

        cvReleaseMemStorage(&p_strStorage);                             //release what we alocated earlyer 
        charCheckForEscKey = cvWaitKey(10);                             //delay in ms, and get key press, if any
        if( charCheckForEscKey == 27) break;                            // if ascii 27 was pressed jump out of while loop


    }// end of while 

    cvReleaseCapture(&p_capWebCam); 
    cvDestroyWindow("Original"); 
    cvDestroyWindow("Processed"); 

    return(0); 

   }// end of program 