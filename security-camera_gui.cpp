#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

#include <chrono>   //for system time
#include <ctime>
#include <algorithm>    //for string processing

#define CAMERANAME "L240 Security Camera"

int main()
{
    std::chrono::system_clock::time_point sys_time; //sys_time variable to hold the gathered system time from line 32.
    std::string recordingStartTime; //Name that will be used to write video file.
    cv::VideoCapture cap(0);    //Opens camera.

    //Gets current height and width of camera, used in VideoWriter below.
    int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);


    //Records to .avi format.
    cv::VideoWriter writer;
    bool currentlyRecording = false;    //Boolean variable to specify recording mode state.

    if(!cap.isOpened()) //Return error code if camera does not open.
        return -1;

    cv::namedWindow(CAMERANAME);    //Creates graphical window to view video.
    cv::createTrackbar("Record", CAMERANAME, 0, 1); //Creates trackbar to switch from to and from recording mode.
    cv::Mat frame;  //Matrix variable to hold frame data.

    while(true)
    {
        sys_time = std::chrono::system_clock::now();  //Gets current time from system, stores it in sys_time.
        std::time_t converted_time = std::chrono::system_clock::to_time_t(sys_time); //Converts sys_time to type time_t so it can be treated as a string.

        cap >> frame;   //Updates frame.

        //Puts date/time overlay over frame.
        cv::putText(frame, std::ctime(&converted_time), cv::Point(300, 465), cv::FONT_HERSHEY_SIMPLEX, 0.75, CV_RGB(0, 255, 0), 2);

        //Conditional statements to switch to and from recording mode in real-time.
        if (cv::getTrackbarPos("Record", CAMERANAME) == 1 && currentlyRecording == false)
        {
            //Time coversions. (You'd think this would be more straightforward...)
            currentlyRecording = true;
            char buffer [80];
            struct tm *timeinfo = localtime(&converted_time);
            strftime (buffer,80,"%Y-%m-%d_%X", timeinfo);
            recordingStartTime = buffer;
            std::replace( recordingStartTime.begin(), recordingStartTime.end(), ':', '-'); // replace all 'x' to 'y'
            cv::VideoWriter temp("/media/pi/WD_Purple/" + recordingStartTime + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), 7, cv::Size(frame_width, frame_height));
            writer = temp;
            temp.release();
        }

        else if (cv::getTrackbarPos("Record", CAMERANAME) == 0 && currentlyRecording == true)
        {
            currentlyRecording = false;
            recordingStartTime = "";
            writer.release();
        }

        //Writes frame to video file.
        if (currentlyRecording == true)
            writer.write(frame);

        imshow(CAMERANAME, frame);  //Puts image onto frame matrix.

        char c = (char)cv::waitKey(1);  //Press ESC to exit program.
        if( c == 27 )
            break;
    }

    //Memory Deallocation
    cap.release();
    writer.release();
    cv::destroyAllWindows();

    return 0;
}
