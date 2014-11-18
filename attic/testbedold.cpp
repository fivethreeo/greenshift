#include "opencv2/opencv.hpp"
#include <array>

using namespace cv;

int position_added = 0;
int window_set = 0;

Rect track_window;
Point pos;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
      
        if (position_added == 0) {
          pos.x=x;
          pos.y=y;
        }
        if (position_added == 1 ) {
          int largex = std::max(pos.x, x);
          int largey = std::max(pos.y, y);
          int smallx = std::min(pos.x, x);
          int smally = std::min(pos.y, y);
          track_window = Rect(smallx, smally, largex-smallx, largey-smally);
        }

        if (position_added < 2) position_added++;
        else position_added = 0;
        
    }
    if  ( event == EVENT_RBUTTONDOWN )
    {
        window_set = 1;
    }    
}

int main(int, char**)
{
  
    Mat frame, hsv_roi, mask, roi_hist, hsv, dst;
    
    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0};
            
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    waitKey(100); // Wait for camera so frame won't be empty

    namedWindow("Selectarea",1);
    setMouseCallback("Selectarea", CallBackFunc, NULL);
    
    for(;;)
    {

       cap >> frame; // get a new frame from camera
       
       if (position_added == 2) {
         rectangle( frame,
           track_window,
           Scalar( 0, 255, 255 ),
           2,
           1);
      }
       
       imshow("Selectarea", frame);
        
       waitKey(33);    

       if (window_set == 1) break;

    }
    
    cap >> frame; // get a new frame from camera
    cvtColor(frame, hsv_roi, CV_BGR2HSV);

    // set up the ROI for tracking
    inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180,255,255), mask);
    
    calcHist(&hsv_roi, 1, channels, Mat(),
    roi_hist, 1, histSize, ranges, true, false);
    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
    
    // Setup the termination criteria, either 10 iteration or move by atleast 1 pt
    TermCriteria term_crit = TermCriteria(  
      TermCriteria::EPS | TermCriteria::COUNT,
      10, 1);
    namedWindow("mask",1);

       imshow("mask", mask);
    for(;;)
    {
       cap >> frame; // get a new frame from camera
       
       cvtColor(frame, hsv, CV_BGR2HSV);
       calcBackProject(&hsv, 1, channels, roi_hist, dst, ranges);
       
       meanShift(dst, track_window, term_crit);
       
       imshow("Hist", dst);
      
       rectangle(frame,
           track_window,
           Scalar( 0, 255, 255 ),
           2,
           1);
              
       imshow("Selectarea", frame);
        
       if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
/*
import numpy as np
import cv2

cap = cv2.VideoCapture('slow.flv')

# take first frame of the video
ret,frame = cap.read()

# setup initial location of window
r,h,c,w = 250,90,400,125  # simply hardcoded the values
track_window = (c,r,w,h)

# set up the ROI for tracking
roi = frame[r:r+h, c:c+w]
hsv_roi =  cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
mask = cv2.inRange(hsv_roi, np.array((0., 60.,32.)), np.array((180.,255.,255.)))
roi_hist = cv2.calcHist([hsv_roi],[0],mask,[180],[0,180])
normalize(roi_hist,roi_hist,0,255,cv2.NORM_MINMAX)

# Setup the termination criteria, either 10 iteration or move by atleast 1 pt
term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

while(1):
    ret ,frame = cap.read()

    if ret == True:
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        dst = cv2.calcBackProject([hsv],[0],roi_hist,[0,180],1)

        # apply meanshift to get the new location
        ret, track_window = cv2.meanShift(dst, track_window, term_crit)

        # Draw it on image
        x,y,w,h = track_window
        img2 = cv2.rectangle(frame, (x,y), (x+w,y+h), 255,2)
        cv2.imshow('img2',img2)

        k = cv2.waitKey(60) & 0xff
        if k == 27:
            break
        else:
            cv2.imwrite(chr(k)+".jpg",img2)

    else:
        break

cv2.destroyAllWindows()
cap.release()
*/