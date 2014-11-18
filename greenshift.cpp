#include "opencv2/opencv.hpp"

#include <stdlib.h>
#include <array>
#include <string>

#include <windows.h>
#define NUM_BOUND_KEYS 3

using namespace cv;

const char g_szClassName[] = "greenshift";

struct boundKey {
    int id;
    std::string text;
    int width;
    int height;
};
    
struct boundKey boundKeys[NUM_BOUND_KEYS] = {
  {1, "Shift", 40, 18},
  {2, "Ctrl", 40, 18},
  {3, "Alt", 40, 18}
};

boundKey getById(int id) {
  boundKey key;
  for (int i=0; i<NUM_BOUND_KEYS; i++) {
    if (boundKeys[i].id == id) {
      key=boundKeys[i];
      break;
    }
  }
  return key;
}

boundKey activeKey;
bool key_active = FALSE;

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        case WM_CREATE:
        {
            int left = 0;
            for (int i=0; i<NUM_BOUND_KEYS; i++) {
              HFONT hfDefault;
              HWND hStatic;
      
              hStatic = CreateWindowEx(WS_EX_CLIENTEDGE, "Static", boundKeys[i].text.c_str(), 
                  WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY, 
                  0, 0, boundKeys[i].width, boundKeys[i].height, hwnd, (HMENU)boundKeys[i].id, GetModuleHandle(NULL), NULL);
              if(hStatic == NULL)
                  MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

              hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
              SendMessage(hStatic, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
              SetWindowPos(hStatic, 0, left, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
              left = left + boundKeys[i].width + 5;
            }
        }
        break;
        case WM_COMMAND:
        {
            switch (HIWORD(wParam))
            {
              case STN_CLICKED:
              {
                if (key_active) key_active = FALSE;
                else key_active = TRUE;
                activeKey = boundKeys[(int)LOWORD(wParam)];
              }
            }
        }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Message;
    Message.message = ~WM_QUIT;
    Mat frame, hsv, thresholded;
    
    int iLowH = 69;
    int iHighH = 101;
    
    int iLowS = 18; 
    int iHighS = 255;
    
    int iLowV = 72;
    int iHighV = 239;
    
    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        g_szClassName,
        "The title of my window",
        WS_POPUP ,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 60,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    RECT rc;
    
    GetWindowRect ( hwnd, &rc ) ;
    
    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right)-100;
    // int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom)+20;
    int yPos = 2;

    SetWindowPos( hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
    int left = 0;
    HRGN visibleRgn = CreateRectRgn(0,0,0,0);
    for (int i=0; i<NUM_BOUND_KEYS; i++) {
        HRGN tempRgn = CreateRectRgn(
          left,
          0,
          left+boundKeys[i].width,
          boundKeys[i].height
        );
        left = left + boundKeys[i].width + 5;
        CombineRgn(visibleRgn, visibleRgn, tempRgn, RGN_OR);
        DeleteObject(tempRgn);
    }
    SetWindowRgn(hwnd, visibleRgn, FALSE);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    
    namedWindow("thresholded", 1);
        
    waitKey(1000); // Wait for camera so frame won't be empty
    
    while (Message.message != WM_QUIT)
    {
        if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
        {
            // If a message was waiting in the message queue, process it
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else
        { 
        cap >> frame; // get a new frame from capture
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        
        inRange(hsv, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), thresholded); //Threshold the image
        
        //morphological opening (remove small objects from the foreground)
        erode(thresholded, thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(thresholded, thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        
        //morphological closing (fill small holes in the foreground)
        dilate(thresholded, thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(thresholded, thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        if (key_active) {
          tresholded.size() / countNonZero(tresholded) 
        }
        imshow("thresholded", thresholded);
        }
    }
    return Message.wParam;
}