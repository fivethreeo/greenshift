#include "opencv2/opencv.hpp"

#include <windows.h>
#define IDC_MAIN_EDIT	101

using namespace cv;

const char g_szClassName[] = "myWindowClass";

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
            HFONT hfDefault;
            HWND hEdit;
    
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Static", "", 
                WS_CHILD | WS_VISIBLE, 
                0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
            if(hEdit == NULL)
                MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
    
            hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
        }
        break;
        case WM_SIZE:
        {
            HWND hEdit;
            RECT rcClient;
    
            GetClientRect(hwnd, &rcClient);
    
            hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
            SetWindowPos(hEdit, NULL, 0, 0, rcClient.right/2, rcClient.bottom, SWP_NOZORDER);
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
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

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
        
        imshow("thresholded", thresholded);
        }
    }
    return Message.wParam;
}