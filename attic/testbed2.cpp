#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
  
    Mat frame, hsv_roi, mask, roi_hist, hsv, dst;
    Rect track_window;
        
    VideoCapture cap('slow.flv'); // open the default
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap >> frame; // get a new frame from camera
    
    // setup initial location of window
    track_window = Rect(250, 400, 90, 125); // simply hardcoded the values    
    
    // set up the ROI for tracking    
    cvtColor(frame, hsv_roi, CV_BGR2HSV);
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
    // we compute the histogram from the 0-th channel
    int channels[] = {0};
    
    inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180,255,255), mask);
    calcHist(&hsv_roi, 1, channels, mask,  roi_hist, 1, histSize, ranges, true, false);
    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
    
    // Setup the termination criteria, either 10 iteration or move by atleast 1 pt
    TermCriteria term_crit = TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    
    namedWindow("frame", 1);

    for(;;)
    {
       cap >> frame; // get a new frame from capture
       
       cvtColor(frame, hsv, CV_BGR2HSV);
       calcBackProject(&hsv, 1, channels, roi_hist, dst, ranges);
       
       meanShift(dst, track_window, term_crit);
       
       rectangle(frame, track_window, Scalar( 0, 0, 255 ), 2, 1);
              
       imshow("frame", frame);
        
       if(waitKey(60) >= 0) break;
    }
    // the capture will be deinitialized automatically in VideoCapture destructor
    return 0;
}
